#include "canopenproxy.h"
#include "core/application.h"

CanOpenProxy::CanOpenProxy(CanInterface *device, uint8_t nodeId) :
    m_nodeId(nodeId & 0x7F)
{
    m_can = new CanSocket(device);
    m_can->addFilter(nodeId, 0x7F);
    m_can->onReadyRead = EVENT(&CanOpenProxy::readPacket);
    m_can->open();
    
    m_resendTimer = new Timer();
    m_resendTimer->setInterval(50);
    m_resendTimer->onTimeout = EVENT(&CanOpenProxy::resendSdo);
    
    stmApp()->registerTaskEvent(EVENT(&CanOpenProxy::task));
}

void CanOpenProxy::task()
{
    if (m_nmtErrorControl)
        nmtErrorControl();
    
    while (!m_nmtQueue.empty())
    {
        uint8_t cmd = m_nmtQueue.front();
        ByteArray ba(2, 0);
        ba[0] = cmd;
        ba[1] = m_nodeId;
        bool success = sendPacket(NMT_ModuleControl, ba);
        if (success)
            m_nmtQueue.pop();
        else
            break;
    }
}

void CanOpenProxy::resendSdo()
{    
    while (!m_sdoQueue.empty())
    {
        SDO &sdo = m_sdoQueue.front();
        ByteArray ba = ByteArray(reinterpret_cast<const char*>(&sdo), 8);
        bool success = sendPacket(SDO_Request | m_nodeId, ba);
        if (success)
            m_sdoQueue.pop();
        else
            return;
    }
    m_resendTimer->stop();
}

void CanOpenProxy::nmtModuleControl(NMTControl cmd)
{
    m_nmtQueue.push(cmd);
//    ByteArray ba(2, 0);
//    ba[0] = cmd;
//    ba[1] = m_nodeId;
//    sendPacket(NMT_ModuleControl, ba);
}

void CanOpenProxy::nmtErrorControl() // request node state
{
    m_nmtErrorControl = !m_can->interface()->transmitMessage(CanInterface::RTR, NMT_ErrorControl | m_nodeId, nullptr, 0);
}

void CanOpenProxy::pdoWrite(uint8_t pdo, const ByteArray &value)
{
    uint16_t cob_id = m_nodeId;
    switch (pdo)
    {
    case 1: cob_id |= PDO1_RX; break;
    case 2: cob_id |= PDO2_RX; break;
    case 3: cob_id |= PDO3_RX; break;
    case 4: cob_id |= PDO4_RX; break;
    default: return;
    }
    sendPacket(cob_id, value);
}

void CanOpenProxy::sdoRead(uint16_t id, uint8_t subid, uint8_t size)
{
    if (size > 4)
        return;
    SDO sdo;
    sdo.cmd = cmdReadParam;
    sdo.n = 4 - size;
    sdo.id = id;
    sdo.subid = subid;
    sdo.value = 0;
    sdoEnqueue(std::move(sdo));
}

void CanOpenProxy::sdoRead8(uint16_t id, uint8_t subid)
{
    sdoRead(id, subid, 1);
}

void CanOpenProxy::sdoRead16(uint16_t id, uint8_t subid)
{
    sdoRead(id, subid, 2);
}

void CanOpenProxy::sdoRead32(uint16_t id, uint8_t subid)
{
    sdoRead(id, subid, 4);
}

void CanOpenProxy::sdoWrite(uint16_t id, uint8_t subid, uint32_t value, uint8_t size)
{
    if (size > 4)
        return;
    
    SDO sdo;
    sdo.cmd = cmdWriteParam;
    sdo.n = 4 - size;
    sdo.id = id;
    sdo.subid = subid;
    sdo.value = value;    
    sdoEnqueue(std::move(sdo));
}

void CanOpenProxy::sdoWrite8(uint16_t id, uint8_t subid, uint8_t value)
{
    sdoWrite(id, subid, value, 1);
}

void CanOpenProxy::sdoWrite16(uint16_t id, uint8_t subid, uint16_t value)
{
    sdoWrite(id, subid, value, 2);
}

void CanOpenProxy::sdoWrite32(uint16_t id, uint8_t subid, uint32_t value)
{
    sdoWrite(id, subid, value, 4);
}

bool CanOpenProxy::configPdo(FunctionCode func, std::initializer_list<uint32_t> sdo_list, int interval_ms)
{
    uint16_t comm, map;
    switch (func)
    {
    case PDO1_TX: comm = 0x1800; map = 0x1A00; break;
    case PDO1_RX: comm = 0x1400; map = 0x1600; break;
    case PDO2_TX: comm = 0x1801; map = 0x1A01; break;
    case PDO2_RX: comm = 0x1401; map = 0x1601; break;
    case PDO3_TX: comm = 0x1802; map = 0x1A02; break;
    case PDO3_RX: comm = 0x1402; map = 0x1602; break;
    case PDO4_TX: comm = 0x1803; map = 0x1A03; break;
    case PDO4_RX: comm = 0x1404; map = 0x1603; break;
    default: return false;
    }
    
    uint32_t cob_id = func | nodeId();
    
    // 1. deactivate PDO
    sdoWrite32(comm, 0x01, 0x80000000 | cob_id);

    if (interval_ms)
    {
        // Set Transmission Type = 0xFE (Cyclic mode)
        sdoWrite8(comm, 0x02, 0xFE);
        // Set Event Timer interval (WUT?? ought to be 0.1ms step)
        sdoWrite16(comm, 0x05, interval_ms);
    }
    else
    {
        // Set Transmission Type = 0xFF (Event mode)
        sdoWrite8(comm, 0x02, 0xFF);
    }

    // configure SDO mapping
    sdoWrite8(map, 0x00, 0x00);  // reset map
    int cnt = 0;
    for (uint32_t sdo: sdo_list)
        sdoWrite32(map, ++cnt, sdo); // add SDO to the map
    sdoWrite8(map, 0x00, cnt); // set map size

    // enable PDO
    sdoWrite32(comm, 0x01, cob_id | nodeId());
    
    return true;
}

void CanOpenProxy::sdoEnqueue(SDO &&sdo)
{
    m_sdoQueue.push(std::move(sdo));
    if (!m_resendTimer->isRunning())
        m_resendTimer->start();
}

bool CanOpenProxy::sendPacket(uint16_t cob_id, const ByteArray &payload)
{
    ByteArray ba;
    ba.append(reinterpret_cast<const char *>(&cob_id), 2);
    ba.append(payload);
    return m_can->write(ba) > 0;
}

void CanOpenProxy::readPacket()
{
    ByteArray ba = m_can->readAll();
    uint16_t cob_id = *reinterpret_cast<const uint16_t *>(ba.data());
    handlePacket(cob_id, ba.mid(2));
}

void CanOpenProxy::handlePacket(uint16_t cob_id, const ByteArray &payload)
{
    uint16_t functionCode = cob_id & 0x780;
    uint8_t nodeId = cob_id & 0x7F;
    if (nodeId != m_nodeId) // wut?
        return;
    
    switch (functionCode)
    {
//    case NMT_ModuleControl:
//      break;
      
    case EMERGENCY:
      emergency();
      break;
      
    case TIMESTAMP:
      // not implemented
      break;
      
    case PDO1_TX:
      pdoReceived(1, payload);
      break;
      
    case PDO2_TX:
      pdoReceived(2, payload);
      break;
      
    case PDO3_TX:
      pdoReceived(3, payload);
      break;
      
    case PDO4_TX:
      pdoReceived(4, payload);
      break;
      
    case SDO_Response:
    {
        const SDO &sdo = *reinterpret_cast<const SDO*>(payload.data());
        SDOAbortCode err = static_cast<SDOAbortCode>(payload[6]);

        switch (sdo.cmd & 0xE3)
        {
          case cmdReadParamResponse:
            sdoReceived(sdo.id, sdo.subid, sdo.value);
            break;

          case cmdWriteParamResponse:
            sdoWritten(sdo.id, sdo.subid);
            break;

          case cmdError:
            sdoError(err);
            break;
        }
    } break;
    
    case NMT_ErrorControl:
    {
        uint8_t data = payload[0];
//        if (!((data ^ m_nmtState) & 0x80)) // if not toggled => warning
//            alarma();
        m_nmtState = data;
        nmtStateChanged();
    } break;
      
    default: return;
    }
}