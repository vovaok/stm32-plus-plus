#ifndef _OBJNET_DEVICE_H
#define _OBJNET_DEVICE_H

#include <vector>
#include <map>
#include "objnetcommon.h"
#include "objectinfo.h"
#ifndef QT_CORE_LIB
#include "timer.h"
#else
#include <QtCore>
#include <QObject>
#define Timer QTimer
#endif

namespace Objnet
{


#ifndef QT_CORE_LIB
class ObjnetDevice {
#else
class ObjnetDevice : public QObject
{
    Q_OBJECT
#endif
private:
    bool mClassValid;
    bool mNameValid;
    int mInfoValidCnt;

protected:
//    typedef struct
//    {
//        ObjectInfo::Description desc;
//        ByteArray value;
//    } ObjnetObject;

    unsigned char mNetAddress;
    unsigned char mBusAddress;
    bool mPresent;
    unsigned char mTimeout;
    bool mAutoDelete; // когда true, при отключении девайса от сети, он удаляется из списка (по умолчанию false, только мастер может установить true при создании объекта)
//    bool mStateChanged;
//    unsigned char mChildrenCount;
//    unsigned char mOrphanCount; // not processed children

    unsigned long mClass;
    string mName;
    string mFullName;
    unsigned long mSerial;
    unsigned short mVersion;
    string mBuildDate;
    string mCpuInfo;
    unsigned long mBurnCount;
    unsigned char mObjectCount;

    vector<ObjectInfo*> mObjects;
    map<string, ObjectInfo> mObjMap;
    vector<ByteArray> mObjBuffers;

    void prepareObject(const ObjectInfo::Description &desc); //only master calls

    friend class ObjnetMaster;

    void setName(string name) {mName = name.substr(0, 8); mNameValid = true;}
    void setClassId(unsigned long classId) {mClass = classId; mClassValid = true;}

    void receiveObject(unsigned char oid, const ByteArray &ba);
    void receiveGlobalMessage(unsigned char aid);

public:
    ObjnetDevice(unsigned char netaddr = 0);

    bool isPresent() const {return mPresent;}
    bool isValid() const {return mClassValid && mNameValid;}
    bool isInfoValid() const {return mInfoValidCnt >= 6;}

    unsigned char netAddress() const {return mNetAddress;}
    unsigned char busAddress() const {return mBusAddress;}

    unsigned long classId() const {return mClass;}
    _String name() const {return _toString(mName);}
    _String fullName() const {return _toString(mFullName);}
    unsigned long serial() const {return mSerial;}
    unsigned short version() const {return mVersion;}
    int majorVersion() const {return mVersion >> 8;}
    int minorVersion() const {return mVersion & 0xFF;}
    _String versionString() const {char s[8]; sprintf(s, "%d.%d", majorVersion(), minorVersion()); return _toString(string(s));}
    _String buildDate() const {return _toString(mBuildDate);}
    _String cpuInfo() const {return _toString(mCpuInfo);}
    int burnCount() const {return mBurnCount;}

    int objectCount() const {return mObjectCount;}
    ObjectInfo *objectInfo(unsigned char oid) {if (oid < mObjects.size()) return mObjects[oid]; return 0L;}

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

#ifdef QT_CORE_LIB
signals:
    void requestObject(unsigned char netAddress, unsigned char oid);
    void sendObject(unsigned char netAddress, unsigned char oid, const QByteArray &ba);
    void serviceRequest(unsigned char netAddress, SvcOID oid, const QByteArray &ba);

    void objectReceived(QString name, QVariant value);
    void autoRequestAccepted(QString name, int periodMs);

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
