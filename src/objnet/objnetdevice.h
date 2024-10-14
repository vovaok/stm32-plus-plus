#ifndef _OBJNET_DEVICE_H
#define _OBJNET_DEVICE_H

#include <vector>
#include <map>
#include "objnetcommon.h"
#include "objectinfo.h"
#ifndef QT_CORE_LIB
#include "core/timer.h"
#else
#include <QtCore>
#include <QObject>
#define Timer QTimer
#endif

namespace Objnet
{

class ObjnetMaster; // forward declaration

#ifndef QT_CORE_LIB
class ObjnetDevice {
#else
class ObjnetDevice : public QObject
{
    Q_OBJECT
#endif
private:
    // mask should be 0x3FF but now we have this shit for backward compatibility
    static const uint32_t mInfoValidMask = 0x000000FF;
    uint32_t mInfoValidFlags;

    void onObjectValueChanged(unsigned char id);
    
    Timer mSendTimer;
    
    void parseObjectInfo(const ByteArray &ba);
    ObjectInfo *prepareObject(const ObjectInfo::Description &desc);

protected:
//    typedef struct
//    {
//        ObjectInfo::Description desc;
//        ByteArray value;
//    } ObjnetObject;
  
    ObjnetMaster *mMaster;
    std::vector<ObjnetDevice*> mChildren;
    ObjnetDevice *mParent;

    unsigned char mNetAddress;
    unsigned char mBusAddress;
    bool mPresent;
    unsigned char mTimeout;
    unsigned char mTempTimeout;
    bool mAutoDelete; // когда true, при отключении девайса от сети, он удаляется из списка (по умолчанию false, только мастер может установить true при создании объекта)
    bool mIsLocal; // true когда девайс подключён непосредственно к подсети текущего мастера
    bool mConnectionError; // true если подключение к устройству вызывает ошибку
    bool mAlreadyReady = false; // если уже был хоть раз полностью проиничен (чтобы заново не эмитить ready())
    //    bool mStateChanged;
//    unsigned char mChildrenCount;
//    unsigned char mOrphanCount; // not processed children
    
    uint32_t mClass;
    string mName;
    string mFullName;
    uint32_t mSerial;
    unsigned short mVersion;
    string mBuildDate;
    string mCpuInfo;
    uint32_t mBurnCount;
    unsigned char mObjectCount;
    BusType mBusType;

    vector<ObjectInfo*> mObjects;
    map<string, ObjectInfo> mObjMap;
    vector<ByteArray> mObjBuffers;

    Closure<void(unsigned char, unsigned char)> masterRequestObject;
    Closure<void(unsigned char, unsigned char, const ByteArray&)> masterSendObject;
    Closure<void(unsigned char, SvcOID, const ByteArray&)> masterServiceRequest;

    friend class ObjnetMaster;

    void receiveServiceObject(unsigned char oid, const ByteArray &ba);
    void receiveObject(unsigned char oid, const ByteArray &ba);
    void receiveTimedObject(const ByteArray &ba);
    void receiveGroupedObject(const ByteArray &ba);
    void receiveGlobalMessage(unsigned char aid);
    
    virtual void readyEvent() {}
    
#ifdef QT_CORE_LIB
protected slots:
#endif
    void onSendTimer(); 

public:
    ObjnetDevice(unsigned char netaddr = 0);
    virtual ~ObjnetDevice() {}

    bool isPresent() const {return mPresent;}
    bool isValid() const {return (mInfoValidFlags & 3) == 3;}
    bool isInfoValid() const {return (mInfoValidFlags & mInfoValidMask) == mInfoValidMask;}
    bool isObjectInfoValid() const;
    bool isReady() const {return isInfoValid() && isObjectInfoValid();}

    unsigned char netAddress() const {return mNetAddress;}
    unsigned char busAddress() const {return mBusAddress;}

    uint32_t classId() const {return mClass;}
    _String name() const {return _toString(mName);}
    _String fullName() const {return _toString(mFullName);}
    uint32_t serial() const {return mSerial;}
    unsigned short version() const {return mVersion;}
    int majorVersion() const {return mVersion >> 8;}
    int minorVersion() const {return mVersion & 0xFF;}
    _String versionString() const {char s[8]; sprintf(s, "%d.%d", majorVersion(), minorVersion()); return _toString(string(s));}
    _String buildDate() const {return _toString(mBuildDate);}
    _String cpuInfo() const {return _toString(mCpuInfo);}
    int burnCount() const {return mBurnCount;}
    BusType busType() const {return mBusType;}
    _String busTypeName() const;

    int objectCount() const {return mObjectCount;}
    ObjectInfo *objectInfo(unsigned char oid) {if (oid < mObjects.size()) return mObjects[oid]; return 0L;}
    ObjectInfo *objectInfo(_String name)
    {
        if (mObjMap.count(_fromString(name))) 
            return &mObjMap[_fromString(name)];
        return 0L;
    }
    bool hasObject(_String name) {return mObjMap.find(_fromString(name)) != mObjMap.end();}

    void changeName(_String name);
    void changeFullName(_String name);
    void changeBusAddress(unsigned char mac);

    ObjnetDevice *parentDevice() {return mParent;}

    #ifndef QT_CORE_LIB

    #else

    #endif

    template<typename T>
    bool bindVariable(_String name, T &var)
    {
        ObjectInfo &info = mObjMap[_fromString(name)];
        ObjectInfo::Type t = typeOfVar(var);
        if ((t == info.mDesc.wType) || !info.mDesc.flags)
        {
            info.mWritePtr = &var;
            info.mReadPtr = &var;
            info.mDesc.wType = info.mDesc.rType = t;
            return true;
        }
        return false;
    }
    
    template<typename T, int N>
    bool bindVariable(_String name, T (&var)[N])
    {
        ObjectInfo &info = mObjMap[_fromString(name)];
        ObjectInfo::Type t = typeOfVar(var[0]);
        if ((t == info.mDesc.wType && sizeof(var) == info.mDesc.writeSize) || !info.mDesc.flags)
        {
            info.mWritePtr = &var;
            info.mReadPtr = &var;
            info.mDesc.wType = info.mDesc.rType = t;
            return true;
        }
        return false;
    }
    
    template<typename Tr, typename Tw>
    bool bindVariable(_String name, Tr &rVar, Tw &wVar)
    {
        ObjectInfo &info = mObjMap[_fromString(name)];
        ObjectInfo::Type rt = typeOfVar(rVar);
        ObjectInfo::Type wt = typeOfVar(wVar);
        if ((((sizeof(rVar) == info.mDesc.readSize)) && (sizeof(wVar) == info.mDesc.writeSize)) || !info.mDesc.flags)
        {
            info.mWritePtr = &wVar;
            info.mReadPtr = &rVar;
            info.mDesc.wType = wt;
            info.mDesc.rType = rt;//ObjectInfo::Common;
            return true;
        }
        return false;
    }

    template<class T, class P0>
    bool bindMethod(_String name, T *objptr, void (T::*method)(P0))
    {
        ObjectInfo &info = mObjMap[_fromString(name)];
        //if ((t == info.mDesc.wType) || !info.mDesc.flags)
        //{
            //*reinterpret_cast<Closure<void(P0)>*>(&info.mReadPtr) = detail::CreateClosure(method).Init<void (T::*)(P0)>(objptr);//CLOSURE(objptr, method);
            //info.mDesc.wType = info.mDesc.rType = t;
            return true;
        //}
        //return false;
    }

    void requestObject(_String name);
    void sendObject(_String name);
    void autoRequest(_String name, int periodMs);
    void timedRequest(_String name, int periodMs);
    void groupedRequest(std::vector<_String> names);

    void requestMetaInfo();
    void requestInfo(unsigned char oid);
    void requestObjectInfo(unsigned char oid);

#ifdef QT_CORE_LIB
signals:
    void requestObject(unsigned char netAddress, unsigned char oid);
    void sendObject(unsigned char netAddress, unsigned char oid, const QByteArray &ba);
    void serviceRequest(unsigned char netAddress, SvcOID oid, const QByteArray &ba);

    void infoReceived(unsigned char oid);
    void objectReceived(QString name, QVariant value);
    void timedObjectReceived(QString name, uint32_t timestamp, QVariant value);
    void objectGroupReceived(QVariantMap values);
    void autoRequestAccepted(QString name, int periodMs);

    void objectValueChanged(QString name, QVariant value);

    void ready();   
    
public slots:
    void sendObject(QString name, QVariant value);
#else
    
    Closure<void(ObjnetDevice*)> onReady;
    Closure<void(_String name)> onObjectReceived;
    
#endif

#ifdef QT_CORE_LIB
signals:
    void globalMessage(unsigned char aid);
#else
    GlobalMessageEvent onGlobalMessage;
#endif
};

}

#endif
