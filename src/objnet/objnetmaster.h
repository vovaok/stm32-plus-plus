#ifndef _OBJNET_MASTER
#define _OBJNET_MASTER

#include <map>
#include "objnetCommonNode.h"
#include "objnetdevice.h"

using namespace std;

namespace Objnet
{

class ObjnetMaster : public ObjnetCommonNode
{
#ifdef QT_CORE_LIB
    Q_OBJECT
#endif
private:
    typedef std::map<unsigned char, ObjnetDevice*> DeviceMap;
    typedef DeviceMap::iterator DeviceIterator;
//    ObjnetDeviceTreeNode mTree;
    DeviceMap mDevices; // map devices by network address
    std::map<unsigned char, unsigned char> mRouteTable; // route table: returns mac by network address
    ObjnetDevice* mLocalnetDevices[16]; // array of devices on the local network accessed by mac
//    std::map<unsigned long, unsigned char> mNetAddrBySerialCache;
    unsigned char mNetAddrByMacCache[16];
    unsigned char mAssignNetAddress; // network address for assigning to nodes (a la DHCP)
    bool mAdjIfConnected; // connection state of adjacent interface
    string mName;
    bool mSwonbMode;
    int mCurMac; // for swonb mode

protected:
    void task();
    void parseMessage(CommonMessage &msg);

    void acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba=0L);
    void parseServiceMessage(CommonMessage &msg);

    unsigned char route(unsigned char netAddress) {return netAddress<0x7F? mRouteTable[netAddress]: 0;}
    unsigned char createNetAddress(unsigned char mac);

public:
#ifdef QT_CORE_LIB
signals:
    void devAdded(unsigned char netAddress, const QByteArray &locData);
    void devConnected(unsigned char netAddress);
    void devDisconnected(unsigned char netAddress);
    void devRemoved(unsigned char netAddress);
    void serviceMessageAccepted(unsigned char netAddress, SvcOID oid, const QByteArray &data);
#else
    Closure<void(unsigned char, const ByteArray&)> onDevAdded;
    Closure<void(unsigned char)> onDevConnected;
    Closure<void(unsigned char)> onDevDisconnected;
    Closure<void(unsigned char)> onDevRemoved;
#endif

#ifdef QT_CORE_LIB
protected slots:
#endif
    void onTimer();

public:
    ObjnetMaster(ObjnetInterface *iface);
    ~ObjnetMaster();

    void reset();

    void setName(string name) {mName = name;}
    void setSwonbMode(bool enabled) {mSwonbMode = enabled;}

    bool isConnected() const {return !mDevices.empty();}

    const DeviceMap &devices() const {return mDevices;}
    ObjnetDevice *device(int netaddr) {return mDevices.count(netaddr)? mDevices[netaddr]: 0L;}
    ObjnetDevice *deviceBySerial(unsigned long serial);
    void addDevice(unsigned char mac, ObjnetDevice *dev);

    void requestName(unsigned char netAddress) {sendServiceMessage(netAddress, svcName);}
    void requestClassId(unsigned char netAddress) {sendServiceMessage(netAddress, svcClass);}
    void requestDevInfo(unsigned char netAddress) {sendServiceMessage(netAddress, svcRequestAllInfo);}
    void requestObjInfo(unsigned char netAddress) {sendServiceMessage(netAddress, svcRequestObjInfo);}
    
    void swonbEnumerate() {mCurMac = 1;}

#ifdef QT_CORE_LIB
public slots:
#endif
    void requestObject(unsigned char netAddress, unsigned char oid);
    void sendObject(unsigned char netAddress, unsigned char oid, const ByteArray &ba);
    void sendServiceRequest(unsigned char netAddress, SvcOID oid, const ByteArray &ba)
    {
        sendServiceMessage(netAddress, oid, ba);
    }
    void sendServiceRequest(StdAID aid, bool propagation, const ByteArray &ba=ByteArray())
    {
        if (propagation)
            aid = static_cast<StdAID>(aid | aidPropagationDown);
        if (ba.size())
            sendGlobalServiceDataMessage(aid, ba);
        else
            sendGlobalServiceMessage(aid);
    }
    void sendUpgrageData(unsigned char seq, const ByteArray &ba)
    {
        CommonMessage msg;
        GlobalMsgId id;
        id.mac = mBusAddress;
        id.svc = 1;
        id.addr = mNetAddress;
        id.res = seq;
        id.aid = aidUpgradeData | aidPropagationDown;
        msg.setGlobalId(id);
        msg.setData(ba);
        mInterface->write(msg);
    }

//    void sendRemoteMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());

#ifdef QT_CORE_LIB
signals:
    void globalMessage(unsigned char aid);
    void globalServiceMessage(unsigned char aid);
#else
    GlobalMessageEvent onGlobalMessage;
    GlobalMessageEvent onGlobalServiceMessage;
#endif
};

}

#endif
