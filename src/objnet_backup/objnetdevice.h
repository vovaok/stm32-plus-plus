#ifndef _OBJNET_DEVICE_H
#define _OBJNET_DEVICE_H

#include "objnetcommon.h"
#ifdef __ICCARM__
#include "timer.h"
#else
#include <QtCore>
#define Timer QTimer
#endif

namespace Objnet
{

class ObjnetDevice
{
protected:
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
    
    //Timer mTimer;
    
    friend class ObjnetMaster;
    
    void setName(string name) {mName = name.substr(0, 8);}
    void setClassId(unsigned long classId) {mClass = classId;}
    
    
public:
    ObjnetDevice(unsigned char netaddr = 0);
    
    bool isPresent() const {return mPresent;}
    unsigned char netAddress() const {return mNetAddress;}
    unsigned long classId() const {return mClass;}
    string name() const {return mName;}
    string fullName() const {return mFullName;}
};

}

#endif