#include "canopendevice.h"

CanOpenDevice::CanOpenDevice(CanInterface *can, uint8_t address) :
    m_led(nullptr),
    m_nodeId(address)
{
    m_can = new CanSocket(can);
    m_can->addFilter(0x600 | address, 0x07F);
    m_can->onReadyRead = EVENT(&CanOpenDevice::readPacket);//bindReceiveEvent(recv_id, EVENT(&CanOpenDevice::receiveStdPacket));
    m_can->open();
    
    deviceType = 0;
    errorRegister = 0;
    memset(&identityObject, 0, sizeof(IdentityObject));
    
    bindVar(0x1000, deviceType);
    bindVar(0x1001, errorRegister);
    createArray(0x1018);
    bindVar(0x1018, identityObject.vendorID);
    bindVar(0x1018, identityObject.productCode);
    bindVar(0x1018, identityObject.revisionNumber);
    bindVar(0x1018, identityObject.serialNumber);
}

void CanOpenDevice::receiveStdPacket(unsigned short id, const ByteArray &ba)
{
    uint16_t cobid = id & 0x780;
    uint8_t addr = id & 0x7F;
    if (addr != m_nodeId)
        return;
    
    ByteArray batx(8, 0);
    
    if (m_led)
        m_led->on();
    
    if (cobid == SDO_Request)
    {
        const SDO &sdo = *reinterpret_cast<const SDO*>(ba.data());
        SDO &sdoTx = *reinterpret_cast<SDO*>(batx.data());

        switch (sdo.cmd & 0xE3)
        {
          case cmdReadParam:            
            if (m_bindings.count(sdo.id))
            {
                std::vector<Binding> &v = m_bindings[sdo.id];
                if (sdo.subid < v.size())
                {    
                    Binding &b = v[sdo.subid];
                    if (b.ptr)
                    {
                        unsigned long value = 0;
                        bool r = paramRequestEvent(sdo.id, sdo.subid);
                        if (r)
                        {
                            for (int i=0; i<b.size; i++)
                                reinterpret_cast<unsigned char*>(&value)[i] = b.ptr[i];
                            sendParam(sdo.id, sdo.subid, value, b.size);
                        }
                        else
                        {
                            sendError(sdo.id, sdo.subid, HardwareError);
                        }
                    }
                    else
                    {
                        sendParam(sdo.id, sdo.subid, b.size, 1);
                    }
                }
                else
                {
                    sendError(sdo.id, sdo.subid, SubindexNotExist);
                }
            }
            else
            {
                sendError(sdo.id, sdo.subid, ObjectNotExist);
            }
            break;

          case cmdWriteParam:
            if (m_bindings.count(sdo.id))
            {
                std::vector<Binding> &v = m_bindings[sdo.id];
                if (sdo.subid < v.size())
                {
                    Binding &b = v[sdo.subid];
                    if (b.ptr)
                    {
                        for (int i=0; i<b.size; i++)
                            b.ptr[i] = reinterpret_cast<const uint8_t*>(&sdo.value)[i];
                        bool r = paramReceiveEvent(sdo.id, sdo.subid);
                        if (r)
                            sendWriteAck(sdo.id, sdo.subid);
                        else
                            sendError(sdo.id, sdo.subid, HardwareError);
                    }
                    else
                        sendError(sdo.id, sdo.subid, SubindexNotExist);
                }
                else
                    sendError(sdo.id, sdo.subid, SubindexNotExist);
            }
            else
                sendError(sdo.id, sdo.subid, ObjectNotExist);
            break;

          default:
            sendError(sdo.id, sdo.subid, UnknownSdoCommand);
        }
    }
    
    if (m_led)
        m_led->off();
}

void CanOpenDevice::createArray(uint16_t id)
{
    m_bindings[id].push_back(Binding());
}

void CanOpenDevice::sendParam(uint16_t id, uint8_t subid, unsigned long value, unsigned char sz)
{  
    SDO sdo;
    sdo.cmd = cmdReadParamResponse;
    sdo.n = 4 - sz;
    sdo.id = id;
    sdo.subid = subid;
    sdo.value = value;
    ByteArray ba = ByteArray(reinterpret_cast<const char*>(&sdo), 8);
    sendPacket(SDO_Response | m_nodeId, ba);
}

void CanOpenDevice::sendWriteAck(uint16_t id, uint8_t subid)
{
    ByteArray ba = ByteArray(8, '\0');
    ba[0] = cmdWriteParamResponse;
    ba[1] = id & 0xFF;
    ba[2] = id >> 8;
    ba[3] = subid;
    sendPacket(SDO_Response | m_nodeId, ba);
}

void CanOpenDevice::sendError(uint16_t id, uint8_t subid, SDOAbortCode err)
{
    ByteArray ba = ByteArray(8, '\0');
    ba[0] = cmdError;
    ba[1] = id & 0xFF;
    ba[2] = id >> 8;
    ba[3] = subid;
    *(uint32_t*)(&ba[4]) = (uint32_t)err;
    sendPacket(SDO_Response | m_nodeId, ba);
}

void CanOpenDevice::readPacket()
{
    ByteArray ba = m_can->readAll();
    uint16_t cob_id = *reinterpret_cast<const uint16_t *>(ba.data());
    receiveStdPacket(cob_id, ba.mid(2));
}

void CanOpenDevice::sendPacket(uint16_t cob_id, const ByteArray &payload)
{
    ByteArray ba;
    ba.append(reinterpret_cast<const char *>(&cob_id), 2);
    ba.append(payload);
    m_can->write(ba);
}
