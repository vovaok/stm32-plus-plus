#include "canopenproxy.h"

CanOpenProxy::CanOpenProxy(CanInterface *device, uint8_t nodeId) :
    m_nodeId(nodeId & 0x7F)
{
    m_can = new CanSocket(device);
    m_can->addFilter(nodeId, 0x7F);
    m_can->onReadyRead = EVENT(&CanOpenProxy::readPacket);
    m_can->open();
}

void CanOpenProxy::nmtModuleControl(NMTControl cmd)
{
    ByteArray ba(2, 0);
    ba[0] = cmd;
    ba[1] = m_nodeId;
    sendPacket(NMT_ModuleControl, ba);
}

void CanOpenProxy::nmtErrorControl() // request node state
{
    m_can->interface()->transmitMessage(CanInterface::RTR, NMT_ErrorControl | m_nodeId, nullptr, 0);
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

void CanOpenProxy::sdoRead(uint16_t id, uint8_t subid)
{
    SDO sdo;
    sdo.cmd = cmdReadParam;
    sdo.id = id;
    sdo.subid = subid;
    sdo.value = 0;
    ByteArray ba = ByteArray(reinterpret_cast<const char*>(&sdo), 8);
    sendPacket(SDO_Request | m_nodeId, ba);
}

void CanOpenProxy::sdoWrite(uint16_t id, uint8_t subid, uint32_t value, uint8_t size)
{
    if (size >= 4)
        return;
    
    SDO sdo;
    sdo.cmd = cmdWriteParam;
    sdo.n = 4 - size;
    sdo.id = id;
    sdo.subid = subid;
    sdo.value = value;
    ByteArray ba = ByteArray(reinterpret_cast<const char*>(&sdo), 8);
    sendPacket(SDO_Request | m_nodeId, ba);
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

void CanOpenProxy::sendPacket(uint16_t cob_id, const ByteArray &payload)
{
    ByteArray ba;
    ba.append(reinterpret_cast<const char *>(&cob_id), 2);
    ba.append(payload);
    m_can->write(ba);
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