#ifndef _UDPONBINTERFACE_H
#define _UDPONBINTERFACE_H

#include "genericonbinterface.h"
#include "ethernet.h"
#include "udpsocket.h"
#include "core/timer.h"

namespace Objnet
{
    
class UdpOnbInterface : public GenericOnbInterface
{
public:
    UdpOnbInterface();
    virtual ~UdpOnbInterface();
    
private:
    UdpSocket *m_socket;
    Timer *m_advertiseTimer; // only in node mode
    int m_broadcastCnt;
    
    void advertise();
    
    virtual void reconnect() override;
};

}

#endif