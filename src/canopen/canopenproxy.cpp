#include "canopenproxy.h"
#include "core/application.h"

using namespace CanOpen;

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

void CanOpenProxy::sync()
{
    sendPacket(0x080);
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

void CanOpenProxy::sendSdo(uint8_t cmd, uint16_t id, uint8_t sid, uint8_t size, uint32_t value)
{
    assert(size <= 4);
    
    SDO sdo;
    sdo.cmd = cmd;
    sdo.n = 4 - size;
    sdo.id = id;
    sdo.subid = sid;
    sdo.value = value;
    sdoEnqueue(std::move(sdo));
}

void CanOpenProxy::sdoReadImpl(ODEntry e)
{
    sendSdo(cmdReadParam, e.id, e.sid, e.size, 0);
}

void CanOpenProxy::sdoWriteImpl(ODEntry e, uint32_t value)
{
    sendSdo(cmdWriteParam, e.id, e.sid, e.size, value);
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

void CanOpenProxy::configPDOImpl(PDOEntry entry, SDOList sdoList,
                                 uint8_t tType, uint16_t eTime, uint8_t iTime)
{
    auto& isTx = entry.isTx;
    auto& comm = entry.commIndex;
    auto& map  = entry.mapIndex;
    auto& func = entry.functionCode;
    
    uint32_t cob_id = func | nodeId();
    
    // Disable PDO
    sdoWrite(comm, 0x01, 4, 0x80000000 | cob_id);
    
    // Set transmission type
    sdoWrite(comm,  0x02, 1, tType);
              
    if (isTx)
    {
        // Set TPDO-exclusive parameters
        sdoWrite(comm, 0x05, 2, eTime);
        sdoWrite(comm, 0x03, 2, iTime);
    }
    
    // Reset map counter
    sdoWrite(map, 0x00, 1, 0x00);
    
    // Fill mapping table
    int i = 0;
    for (const auto& sdo: sdoList)
    {
        uint32_t mapval = (sdo.id << 16) | (sdo.sid << 8) | (sdo.size << 3);
        sdoWrite(map, ++i, 4, mapval);
    }

    // Set map size
    sdoWrite(map, 0x00, 1, i);

    // Enable PDO
    sdoWrite(comm, 0x01, 4, cob_id);
}
    
void CanOpenProxy::sdoRead(uint16_t id, uint8_t sid, uint8_t size)
{
    sdoReadImpl({id, sid, size});
}

void CanOpenProxy::sdoWrite(uint16_t id, uint8_t sid, uint8_t size, uint32_t value)
{
    sdoWriteImpl({id, sid, size}, value);
}

    