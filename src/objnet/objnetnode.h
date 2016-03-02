#ifndef _OBJNET_NODE_H
#define _OBJNET_NODE_H

#include "objnetCommonNode.h"

namespace Objnet
{

class ObjnetNode : public ObjnetCommonNode
{
#ifndef __ICCARM__
    Q_OBJECT
#endif
private:
    typedef enum
    {
        netnStart = 0,      //!< исходное состояние, посылка сообщения Hello
        netnConnecting,     //!< ожидание ответа мастера в течение ххх мс
        netnDisconnecting,  //!< узел понял, что его никто не ждёт и отключается
        netnAccepted,       //!< ответ от мастера принят
        //netnEnumeration,    //!< присвоение логического адреса
        netnReady,          //!< узел готов
    } NetState;

    Timer mSendTimer;
    NetState mNetState; // current node net state
    int mNetTimeout;
    unsigned char mCurrentRemoteAddress;
    int mObjInfoSendCount; // variable for counting of info objects sended

    // objnet related parameters:
    unsigned long mClass;
    string mName;
    string mFullName;
    unsigned long mSerial;
    unsigned short mVersion;
    string mBuildDate;
    string mCpuInfo;
    unsigned long mBurnCount;

    // словарь сервисных объектов:
    std::vector<ObjectInfo> mSvcObjects;
    // словарь объектов
    std::vector<ObjectInfo> mObjects;

#ifdef __ICCARM__
protected:
#else
protected slots:
#endif
    void task();

protected:
    void acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba=0L);
    void parseServiceMessage(CommonMessage &msg);

    void parseMessage(CommonMessage &msg);

    unsigned char route(unsigned char netAddress) {(void)netAddress; return 0;}

    void setClassId(unsigned long classId) {mClass = classId;}
    void setSerial(unsigned long serial) {mSerial = serial;}

    void registerSvcObject(const ObjectInfo &info) {mSvcObjects.push_back(info);}

#ifndef __ICCARM__
protected slots:
#endif
    void onTimeoutTimer();
    void onSendTimer();

public:
    ObjnetNode(ObjnetInterface *iface);
    
    void setName(string name) {mName = name.substr(0, 8);}
    void setFullName(string name) {mFullName = name;}
    void setVersion(unsigned short version) {mVersion = version;}
    void setBurnCount(int burnCount) {mBurnCount = burnCount;}
    
    string name() const {return mName;}
    string fullName() const {return mFullName;}
    unsigned long classId() const {return mClass;}
    unsigned long serial() const {return mSerial;}
    unsigned short version() const {return mVersion;}
    int burnCount() const {return mBurnCount;}
    unsigned char objectCount() const {return mObjects.size();}

    bool isConnected() const {return mNetState > netnConnecting;}

    void bindObject(const ObjectInfo &info) {mObjects.push_back(info); mObjects.back().mDesc.id = mObjects.size() - 1;}
    #define BindObject(obj) bindObject(ObjectInfo(#obj, obj)) // convenient macro
    #define BindObjectEx(obj, flags) bindObject(ObjectInfo(#obj, obj, flags)) // convenient macro
    #define BindDualObject(objRead, objWrite) bindObject(ObjectInfo(#objRead":"#objWrite, objRead, objWrite))
    #define BindMethod(method) bindObject(ObjectInfo(#method, CLOSURE(this, &method)))
    #define BindMethodEx(name, object, method) bindObject(ObjectInfo(name, CLOSURE(object, &method)))
    
#ifdef __ICCARM__
    NotifyEvent onPolling;
#endif
};

}

#endif
