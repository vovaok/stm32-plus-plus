#ifndef _OBJNET_NODE_H
#define _OBJNET_NODE_H

#include "objnetCommonNode.h"

namespace Objnet
{

class ObjnetNode : public ObjnetCommonNode
{
private:
    typedef enum
    {
        netnStart = 0,      //!< исходное состояние, посылка сообщения Hello
        netnConnecting,     //!< ожидание ответа мастера в течение ххх мс
        netnAccepted,       //!< ответ от мастера принят
        //netnEnumeration,    //!< присвоение логического адреса
        netnReady           //!< узел готов
    } NetState;
    
    NetState mNetState; // current node net state
    
    // objnet related parameters:
    unsigned long mClass;
    string mName;
    string mFullName;
    unsigned long mSerial;
    
    // словарь сервисных объектов:
    std::vector<ObjectInfo> mSvcObjects;
    // словарь объектов
    std::vector<ObjectInfo> mObjects;
  
protected:  
    void task();
    void acceptServiceMessage(SvcOID oid, ByteArray *ba=0L);
    void parseServiceMessage(CommonMessage &msg);
    
    void parseMessage(CommonMessage &msg);
    
    unsigned char route(unsigned char netAddress) {(void)netAddress; return 0;}
    
    void setClassId(unsigned long classId) {mClass = classId;}
    void setSerial(unsigned long serial) {mSerial = serial;}
    
    void registerSvcObject(const ObjectInfo &info) {mSvcObjects.push_back(info);}
  
public:
    ObjnetNode(ObjnetInterface *iface);
    
    void setName(string name) {mName = name.substr(0, 8);}
    string name() const {return mName;}
    void setFullName(string name) {mFullName = name;}
    string fullName() const {return mFullName;}
    unsigned long classId() const {return mClass;}
    unsigned long serial() const {return mSerial;}
    
    bool isConnected() const {return mNetState > netnConnecting;}
    
    void registerObject(const ObjectInfo &info) {mObjects.push_back(info);}
};

}

#endif
