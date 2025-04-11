#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H

#include "modbus485.h"
#include "core/timer.h"
#include "core/ringbuffer.h"

//#define PRECISE_TIMER   Tim5
//#include "precisetimer.h"

class ModbusProxy;

class ModbusMaster : public Modbus485
{
public:
    ModbusMaster(Device *device);

    ModbusProxy *createProxy(uint8_t slaveaddr);
    void bindProxy(ModbusProxy *proxy);
    ModbusProxy *getProxy(uint8_t slaveaddr);

    bool isBusy() const {return m_busy;}
    
    void setTimeout(int ms) {m_timer->setInterval(ms);}

protected:
    void writeADU(const Modbus::ADU &adu);
    void parseADU(const Modbus::ADU &adu);
    virtual void requestSent() override;
    virtual void responseUpdated() override;
    uint16_t getCurrentRequestAddress();
    uint16_t getCurrentRequestQuantity();
    friend class ModbusProxy;

private:
    class Packet
    {
    public:
        Packet() : m_adu({0, 0, 0, 0}) {}
        Packet(const Packet &other) = delete;
        void operator =(const Packet &other) = delete;
        void operator =(Packet &&other)
        {
            m_adu = other.m_adu;
            m_ba = std::move(other.m_ba);
        }
        Packet(const Modbus::ADU &adu) :
            m_adu(adu),
            m_ba(adu.data, adu.size)
        {
            m_adu.data = m_ba.data();
            m_adu.size = m_ba.size();
        }
        const Modbus::ADU &adu() const {return m_adu;}
        uint16_t regAddr() const
        {
            return ((uint16_t)m_adu.data[0] << 8) | m_adu.data[1];
        }
        uint16_t regQuantity() const
        {
            return ((uint16_t)m_adu.data[2] << 8) | m_adu.data[3];
        }

    private:
        Modbus::ADU m_adu;
        ByteArray m_ba;
    };

    std::vector<ModbusProxy*> m_proxies;
    RingBuffer<Packet> m_queue;
    Timer *m_timer;
    bool m_busy = false;
//    PreciseTimer precise;

    void writeNextAdu();
    void onTimeout();
};

#endif // MODBUSMASTER_H
