#ifndef _CANOPENMASTER_H
#define _CANOPENMASTER_H
//
//#include "canopencommon.h"
//#include "cansocket.h"
//
//using namespace CanOpen;
//
//class CanOpenProxy;
//
//class CanOpenMaster
//{
//public:
//    CanOpenMaster(CanInterface *device);
//    
//    void nmtSendCommand(uint8_t cmd);
//    void nmtRequest();
//    
//    void sdoRead(uint8_t nodeId, uint16_t id, uint8_t subid);
//    void sdoWrite(uint8_t nodeId, uint16_t id, uint8_t subid, uint32_t value, uint8_t size);
//    
//    void pdoWrite(uint8_t nodeId, uint8_t nPdo, const ByteArray value);
//    
//private:
//    CanSocket *m_can;
//    std::map<uint8_t, CanOpenProxy *> m_proxies;
//};

#endif // _CANOPENMASTER_H
