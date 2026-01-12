#include "modbusmaster.h"
#include "modbusproxy.h"

using namespace Modbus;

ModbusMaster::ModbusMaster(Device *device) :
    Modbus485(device),
    m_queue(16)
{
    onAduReceived = EVENT(&ModbusMaster::parseADU);
    m_timer = new Timer();
    m_timer->onTimeout = EVENT(&ModbusMaster::onTimeout);
    m_timer->setInterval(100);
}

ModbusProxy *ModbusMaster::createProxy(uint8_t slaveaddr)
{
    ModbusProxy *proxy = new ModbusProxy(slaveaddr);
    proxy->m_master = this;
    m_proxies.push_back(proxy);
    return proxy;
}

ModbusProxy *ModbusMaster::getProxy(uint8_t slaveaddr)
{
    for (ModbusProxy *proxy: m_proxies)
    {
        if (proxy->address() == slaveaddr)
            return proxy;
    }
    return nullptr;
}

void ModbusMaster::bindProxy(ModbusProxy *proxy)
{
    proxy->m_master = this;
    m_proxies.push_back(proxy);
}

void ModbusMaster::writeADU(const ADU &adu)
{
    m_queue.push_back(adu);

    if (!isBusy())
        writeNextAdu();
}

void ModbusMaster::parseADU(const ADU &adu)
{
    m_timer->stop();
    m_busy = false;
 
  //  ModbusProxy *p = getProxy(adu.addr);
  //  if (p)
  //      p->parsePDU(adu.func, reinterpret_cast<const uint8_t *>(adu.data), adu.size);
    
    for (ModbusProxy *proxy: m_proxies)
    {
        if (proxy->address() == adu.addr)
            proxy->parsePDU(adu.func, reinterpret_cast<const uint8_t *>(adu.data), adu.size);
    }
    m_queue.pop_front();
    writeNextAdu();
}

void ModbusMaster::requestSent()
{
    m_timer->start();
}

void ModbusMaster::responseUpdated()
{
    if (m_timer->isActive())
        m_timer->start(); // restart
}

uint16_t ModbusMaster::getCurrentRequestAddress()
{
    return m_queue.front().regAddr();
}

uint16_t ModbusMaster::getCurrentRequestQuantity()
{
    return m_queue.front().regQuantity();
}

void ModbusMaster::writeNextAdu()
{
    if (!m_queue.isEmpty())
    {
        Modbus485::writeADU(m_queue.front().adu());
        m_busy = true;
//        m_timer->start(); // restart timer later, when request will be sent 
    }
}

void ModbusMaster::onTimeout()
{
    m_timer->stop();
    m_busy = false;
    const Packet &pkt = m_queue.front();
  //  ModbusProxy *p = getProxy(pkt.adu().addr);
  //  if (p)
   //     p->errorEvent(eNone); // no exception, just slave is not responding...
    
    for (ModbusProxy *proxy: m_proxies)
    {
        if (proxy->address() == pkt.adu().addr)
            proxy->errorEvent(eNone);
    }
    
    m_queue.pop_front();
    writeNextAdu();
}
