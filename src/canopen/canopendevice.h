#ifndef CANOPENDEVICE_H
#define CANOPENDEVICE_H

#include "canopencommon.h"
#include "cansocket.h"
#include "led.h"
#include <map>
#include <vector>

using namespace CanOpen;

class CanOpenDevice
{
public:
    explicit CanOpenDevice(CanInterface *can, uint8_t address);
    
    uint8_t nodeId() const {return m_nodeId;}
    
    void setLed(Led *led) {m_led = led;}
    
    template<typename T>
    void bindVar(uint16_t id, T &var);
    
    void createArray(uint16_t id);
    
protected:
    uint32_t deviceType;
    uint8_t errorRegister;
    IdentityObject identityObject;
    virtual bool paramRequestEvent(uint16_t id, uint8_t subid) {return false;}
    virtual bool paramReceiveEvent(uint16_t id, uint8_t subid) {return false;}
    
private:
    CanSocket *m_can;
    Led *m_led;
    uint8_t m_nodeId;
    class Binding
    {
    public:
        uint8_t *ptr;
        uint8_t size;
//        CanOpen::Type type;
        Binding() : ptr(0L), size(0) {}
        template <typename T> Binding(T &var) : ptr((uint8_t*)&var), size(sizeof(T)){}
    };
    std::map<uint16_t, std::vector<Binding> > m_bindings;

    void sendParam(uint16_t id, uint8_t subid, unsigned long value, unsigned char sz);
    void sendWriteAck(uint16_t id, uint8_t subid);
    void sendError(uint16_t id, uint8_t subid, SDOAbortCode err);

    void sendPacket(uint16_t cob_id, const ByteArray &payload);
    void readPacket();
    void receiveStdPacket(unsigned short id, const ByteArray &ba);    
};

template<typename T>
void CanOpenDevice::bindVar(uint16_t id, T &var)
{
//    unsigned char sz = sizeof(T);
    if (sizeof(T) > 4)
        return; // TODO

    if (m_bindings[id].empty())
    {
        m_bindings[id].push_back(var);
    }
    
    if (!m_bindings[id][0].ptr)
    {
        m_bindings[id][0].size = m_bindings[id].size();
        m_bindings[id].push_back(var);
    }
    else
    {
        m_bindings[id][0] = var;
    }
}

#endif // CANOPENDEVICE_H
