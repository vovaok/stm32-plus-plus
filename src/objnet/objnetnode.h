#ifndef _OBJNET_NODE_H
#define _OBJNET_NODE_H

#include "objnetCommonNode.h"
#ifndef QT_CORE_LIB
#include "cpuid.h"
#include "objnetstorage.h"
#else
#include <QElapsedTimer>
#endif

#include <array>

#if !defined(QT_CORE_LIB)
    #include "core/timer.h"
#else
    #define Timer QTimer
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
    uint32_t m_timestampOrigin = 0; // timestamp to subtract

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
    std::array<ObjectInfo, 11> mSvcObjects;
    // словарь объектов
    std::vector<ObjectInfo> mObjects;
    
    // автоотправка группированных объектов (пока только одна группа)
    ByteArray m_autoGroup;
    uint16_t m_autoGroupInterval = 0;
    int m_autoGroupTime = 0;
    uint8_t m_autoGroupAddr = 0;
    
    bool sendObjectInfo(uint8_t remoteAddr, ObjectInfo *obj, const ByteArray &loc=ByteArray());
    
    void objectValueChanged(unsigned char oid);
    
#ifdef QT_CORE_LIB
    QElapsedTimer etimer;
#endif

#ifndef QT_CORE_LIB
protected:
#else
protected slots:
#endif
    void task() override;

protected:
//    void acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba=0L){}
    virtual bool parseServiceMessage(const CommonMessage &msg) override;

    void parseMessage(const CommonMessage &msg) override;

    unsigned char route(unsigned char netAddress) override {(void)netAddress; return 0;}

    void setSerial(uint32_t serial) {mSerial = serial;}

    void bindSvcObject(SvcOID oid, ObjectInfo &&obj);
    
    uint32_t getTimestamp();

#ifdef QT_CORE_LIB
protected slots:
#endif
    void onTimeoutTimer();
    void onSendTimer();

public:
    ObjnetNode(ObjnetInterface *iface);

#if defined(APP_CLASS)
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

    bool isConnected() const override {return mNetState > netnConnecting;}

    ObjectInfo &bindObject(ObjectInfo &&info);

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
    
    template <class Func>
    inline ObjectInfo &bindMethod(string name, Func f, ObjectInfo::Flags flags=ObjectInfo::NoFlags)
    {
        return bindObject(ObjectInfo(std::move(name), std::move(f), flags));
    }
    
    // methods as data
    template <typename Func, typename = std::enable_if_t<is_callable_v<Func>>>
    inline ObjectInfo &bindInput(string name, Func f)
    {
        return bindObject(ObjectInfo(std::move(name), std::move(f), ObjectInfo::Control));
    }
    
    template <typename Func, typename = std::enable_if_t<is_callable_v<Func>>>
    inline ObjectInfo &bindOutput(string name, Func f)
    {
        return bindObject(ObjectInfo(std::move(name), std::move(f), ObjectInfo::Measurement));
    }
    
    template <typename Func, typename = std::enable_if_t<is_callable_v<Func>>>
    inline ObjectInfo &bindIO(string name, Func f)
    {
        return bindObject(ObjectInfo(std::move(name), std::move(f), ObjectInfo::Exchange));
    }
    
    // bind to object method
    template <class Obj, class R, class... Args>
    inline ObjectInfo &bindMethod(string name, Obj *obj, R (Obj::*method)(Args...), ObjectInfo::Flags flags=ObjectInfo::NoFlags)
    {
        auto f = [obj, method](Args... args) -> R { return (obj->*method)(args...); };
        return bindObject(ObjectInfo(std::move(name), std::move(f), flags));
    }
    
    // bind to object const method
    template <class Obj, class R, class... Args>
    inline ObjectInfo &bindMethod(string name, Obj *obj, R (Obj::*method)(Args...) const, ObjectInfo::Flags flags=ObjectInfo::NoFlags)
    {
        auto f = [obj, method](Args... args) -> R { return (obj->*method)(args...); };
        return bindObject(ObjectInfo(std::move(name), std::move(f), flags));
    }
    
    // methods as data
    template <class Obj, typename T>
    inline ObjectInfo &bindInput(string name, Obj *obj, void (Obj::*method)(T))
    {
        return bindMethod<Obj, void, T>(std::move(name), obj, method, ObjectInfo::Control);
    }
    
    template <class Obj, typename R>
    inline ObjectInfo &bindOutput(string name, Obj *obj, R (Obj::*method)())
    {
        return bindMethod<Obj, R>(std::move(name), obj, method, ObjectInfo::Measurement);
    }
    
    template <class Obj, typename R>
    inline ObjectInfo &bindOutput(string name, Obj *obj, R (Obj::*method)() const)
    {
        return bindMethod<Obj, R>(std::move(name), obj, method, ObjectInfo::Measurement);
    }
    
    template <class Obj, typename R, typename T>
    inline ObjectInfo &bindIO(string name, Obj *obj, R (Obj::*method)(T))
    {
        return bindMethod<Obj, R, T>(std::move(name), obj, method, ObjectInfo::Exchange);
    }
    


    void sendForced(unsigned char oid);
    
    //! Get object by name
    //! @param name The name of the object
    //! @return found object or nullptr if no such name in the dictionary
    ObjectInfo *object(_String name);
    
    //! Immediately read and send the given object
    //! This is convenient but slow
    void sendForced(_String name);

    NotifyEvent onPolling;
    Closure<void(uint8_t)> onServiceMessageAccepted;    
    
#ifdef QT_CORE_LIB
signals:
    void polling();
    void upgradeRequest();
    void globalMessage(unsigned char aid);
    void globalDataMessage(unsigned char aid, const QByteArray &ba);
#else
    NotifyEvent onUpgradeRequest;
    GlobalMessageEvent onGlobalMessage;
    GlobalDataMessageEvent onGlobalDataMessage;
    Closure<void(string)> onObjectValueChanged;
#endif
    
};

}

#endif
