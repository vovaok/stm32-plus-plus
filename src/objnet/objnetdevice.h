#ifndef _OBJNET_DEVICE_H
#define _OBJNET_DEVICE_H

#include <vector>
#include <map>
#include "objnetcommon.h"
#include "objectinfo.h"
#ifdef __ICCARM__
#include "timer.h"
#else
#include <QtCore>
#include <QObject>
#define Timer QTimer
#endif

namespace Objnet
{


#ifdef __ICCARM__
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

    vector<ObjectInfo*> mObjects;
    map<string, ObjectInfo> mObjMap;
    vector<ByteArray> mObjBuffers;

    void prepareObject(const ObjectInfo::Description &desc); //only master calls

    friend class ObjnetMaster;

    void setName(string name) {mName = name.substr(0, 8); mNameValid = true;}
    void setClassId(unsigned long classId) {mClass = classId; mClassValid = true;}

    void receiveObject(unsigned char oid, const ByteArray &ba);

public:
    ObjnetDevice(unsigned char netaddr = 0);

    bool isPresent() const {return mPresent;}
    bool isValid() const {return mClassValid && mNameValid;}
    bool isInfoValid() const {return mInfoValidCnt >= 6;}

    unsigned char netAddress() const {return mNetAddress;}

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

    int objectCount() const {return mObjects.size();}
    ObjectInfo *objectInfo(unsigned char oid) {if (oid < mObjects.size()) return mObjects[oid]; return 0L;}

    #ifdef __ICCARM__

    #else

    #endif

    template<typename T>
    bool bindVariable(_String name, T &var)
    {
        ObjectInfo &info = mObjMap[_fromString(name)];
        ObjectInfo::Type t = typeOfVar(var);
        if ((t == info.mDesc.type) || !info.mDesc.flags)
        {
            info.mWritePtr = info.mReadPtr = &var;
            info.mDesc.type = t;
            return true;
        }
        return false;
    }

    void requestObject(_String name);
    void sendObject(_String name);

#ifndef __ICCARM__
signals:
    void requestObject(unsigned char netAddress, unsigned char oid);
    void sendObject(unsigned char netAddress, unsigned char oid, const QByteArray &ba);

    void objectReceived(QString name, QVariant value);
#endif
};

}

#endif
