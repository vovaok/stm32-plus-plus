#ifndef _OBJNET_NODE_H
#define _OBJNET_NODE_H

#include "objnetCommonNode.h"
#ifndef QT_CORE_LIB
#include "cpuid.h"
#include "objnetstorage.h"
#endif

namespace Objnet
{

class ObjnetNode : public ObjnetCommonNode
{
#ifdef QT_CORE_LIB
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
    uint32_t mTimestamp;

    // objnet related parameters:
    uint32_t mClass;
    _String mName;
    _String mFullName;
    uint32_t mSerial;
    unsigned short mVersion;
    _String mBuildDate;
    _String mCpuInfo;
    uint32_t mBurnCount;
    unsigned char mBusType; // BusType
    
    // количество экземпляров узлов (логических узлов) в одном физическом
    static unsigned char mNodesCount;

    // словарь сервисных объектов:
    std::vector<ObjectInfo> mSvcObjects;
    // словарь объектов
    std::vector<ObjectInfo> mObjects;
    
    bool sendObjectInfo(uint8_t remoteAddr, ObjectInfo *obj, const ByteArray &loc=ByteArray());
    
    void objectValueChanged(unsigned char oid);

#ifndef QT_CORE_LIB
protected:
#else
protected slots:
#endif
    void task();

protected:
    void acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba=0L){}
    void parseServiceMessage(CommonMessage &msg);

    void parseMessage(CommonMessage &msg);

    unsigned char route(unsigned char netAddress) {(void)netAddress; return 0;}

    void setSerial(uint32_t serial) {mSerial = serial;}

    void registerSvcObject(const ObjectInfo &info) {mSvcObjects.push_back(info);}

#ifdef QT_CORE_LIB
protected slots:
#endif
    void onTimeoutTimer();
    void onSendTimer();

public:
    ObjnetNode(ObjnetInterface *iface);

#ifndef QT_CORE_LIB
    void setClassId() {mClass = APP_CLASS;}
#endif
    void setClassId(uint32_t classId) {mClass = classId;}
    void setName(_String name) {mName = _toString(_fromString(name).substr(0, 8));}
    void setFullName(_String name) {mFullName = name;}

    _String name() const {return mName;}
    _String fullName() const {return mFullName;}
    uint32_t classId() const {return mClass;}
    uint32_t serial() const {return mSerial;}
    unsigned short version() const {return mVersion;}
    int burnCount() const {return mBurnCount;}
    unsigned char objectCount() const {return (unsigned char)mObjects.size();}

    bool isConnected() const {return mNetState > netnConnecting;}

    ObjectInfo &bindObject(const ObjectInfo &info);
    #define BindObject(obj) bindObject(ObjectInfo(#obj, obj)) // convenient macro
    #define BindObjectEx(obj, flags) bindObject(ObjectInfo(#obj, obj, flags)) // convenient macro
    #define BindDualObject(objRead, objWrite) bindObject(ObjectInfo(#objRead":"#objWrite, objRead, objWrite))
    #define BindMethod(method) bindObject(ObjectInfo(#method, CLOSURE(this, &method)))
    #define BindMethodHidden(method) bindObject(ObjectInfo(#method, CLOSURE(this, &method), ObjectInfo::Hidden))
    #define BindMethodEx(name, object, method) bindObject(ObjectInfo(name, CLOSURE(object, method)))
    #define BindMethodHiddenEx(name, object, method) bindObject(ObjectInfo(name, CLOSURE(object, &method), ObjectInfo::Hidden))
    
    template <typename T> ObjectInfo &bindVariable(string name, T &var)
    {
        return bindObject(ObjectInfo(name, var));
    }
    
    template <typename T> ObjectInfo &bindInput(string name, T &var)
    {
        return bindObject(ObjectInfo(name, var, ObjectInfo::Control));
    }
    
    template <typename T> ObjectInfo &bindOutput(string name, T &var)
    {
        return bindObject(ObjectInfo(name, var, ObjectInfo::Measurement));
    }
    
    template <typename T> ObjectInfo &bindIO(string name, T &var)
    {
        return bindObject(ObjectInfo(name, var, ObjectInfo::Exchange));
    }
    
    template <typename T> ObjectInfo &bindSetting(string name, T &var)
    {
        return bindObject(ObjectInfo(name, var, ObjectInfo::Storage));
    }
    
//    template <class T> unsigned char bindMethod(string name, T *obj, void (T::*method)(void))
//    {
//        Closure<void(void)> c = CLOSURE(obj, method);
//        return bindObject(ObjectInfo(name, c));
//    }

    void sendForced(unsigned char oid);

#ifdef QT_CORE_LIB
signals:
    void polling();
    void upgradeRequest();
    void globalMessage(unsigned char aid);
    void globalDataMessage(unsigned char aid, const QByteArray &ba);
#else
    NotifyEvent onPolling;
    NotifyEvent onUpgradeRequest;
    GlobalMessageEvent onGlobalMessage;
    GlobalDataMessageEvent onGlobalDataMessage;
    Closure<void(string)> onObjectValueChanged;
#endif
};

}

#endif
