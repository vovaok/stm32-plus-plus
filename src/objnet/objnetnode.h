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
        netnStart = 0,      //!< �������� ���������, ������� ��������� Hello
        netnConnecting,     //!< �������� ������ ������� � ������� ��� ��
        netnDisconnecting,  //!< ���� �����, ��� ��� ����� �� ��� � �����������
        netnAccepted,       //!< ����� �� ������� ������
        //netnEnumeration,    //!< ���������� ����������� ������
        netnReady,          //!< ���� �����
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

    // ������� ��������� ��������:
    std::vector<ObjectInfo> mSvcObjects;
    // ������� ��������
    std::vector<ObjectInfo> mObjects;

#ifndef QT_CORE_LIB
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

    void setSerial(unsigned long serial) {mSerial = serial;}

    void registerSvcObject(const ObjectInfo &info) {mSvcObjects.push_back(info);}

#ifdef QT_CORE_LIB
protected slots:
#endif
    void onTimeoutTimer();
    void onSendTimer();

public:
    ObjnetNode(ObjnetInterface *iface);
    
    void setClassId(unsigned long classId) {mClass = classId;}
    void setName(string name) {mName = name.substr(0, 8);}
    void setFullName(string name) {mFullName = name;}
    
    string name() const {return mName;}
    string fullName() const {return mFullName;}
    unsigned long classId() const {return mClass;}
    unsigned long serial() const {return mSerial;}
    unsigned short version() const {return mVersion;}
    int burnCount() const {return mBurnCount;}
    unsigned char objectCount() const {return mObjects.size();}

    bool isConnected() const {return mNetState > netnConnecting;}

    unsigned char bindObject(const ObjectInfo &info);
    #define BindObject(obj) bindObject(ObjectInfo(#obj, obj)) // convenient macro
    #define BindObjectEx(obj, flags) bindObject(ObjectInfo(#obj, obj, flags)) // convenient macro
    #define BindDualObject(objRead, objWrite) bindObject(ObjectInfo(#objRead":"#objWrite, objRead, objWrite))
    #define BindMethod(method) bindObject(ObjectInfo(#method, CLOSURE(this, &method)))
    #define BindMethodEx(name, object, method) bindObject(ObjectInfo(name, CLOSURE(object, &method)))
    
    void sendForced(unsigned char oid);
    
#ifndef QT_CORE_LIB
    NotifyEvent onPolling;
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
