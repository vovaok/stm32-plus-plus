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
    DeviceMap mDevices; // map devices by network address
    std::map<unsigned char, unsigned char> mRouteTable; // route table: returns mac by network address
    ObjnetDevice* mLocalnetDevices[16]; // array of devices on the local network accessed by mac
    unsigned char mNetAddrByMacCache[16];
    unsigned char mAssignNetAddress; // network address for assigning to nodes (a la DHCP)

    string mName;
    
    bool mSwonbMode;
    int mSearchMac; // for swonb mode
    
    void onNak(unsigned char mac);
    void onError(unsigned char mac, ObjnetInterface::Error error);

    ObjnetDevice *createDevice(unsigned char mac, ByteArray &location);
    void connectDevice(unsigned char netaddr);
    void disconnectDevice(unsigned char netaddr);
    void removeDevice(unsigned char netaddr);

protected:
    void task();
    
    virtual bool parseServiceMessage(const CommonMessage &msg) override;
    void parseMessage(const CommonMessage &msg);

    unsigned char route(unsigned char netAddress) {return netAddress<0x7F? mRouteTable[netAddress]: 0;}
    unsigned char createNetAddress(unsigned char mac);
    
    void adjacentConnected();

public:
    Closure<void(const CommonMessage&)> onServiceMessage;
    
    ObjnetDevice *createStaticDevice(unsigned char busAddress);
    void registerDevice(ObjnetDevice *dev, unsigned char busAddress);

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
    void setPollingInterval(int ms) {mTimer.setInterval(ms);}

    bool isConnected() const {return !mDevices.empty();}

    const DeviceMap &devices() const {return mDevices;}
    ObjnetDevice *device(int netaddr) {return mDevices.count(netaddr)? mDevices[netaddr]: 0L;}
    ObjnetDevice *deviceBySerial(uint32_t serial);

    void requestName(unsigned char netAddress) {sendServiceMessage(netAddress, svcName);}
    void requestClassId(unsigned char netAddress) {sendServiceMessage(netAddress, svcClass);}
    void requestSerial(unsigned char netAddress) {sendServiceMessage(netAddress, svcSerial);}
    void requestDevInfo(unsigned char netAddress) {sendServiceMessage(netAddress, svcRequestAllInfo);}
//    void requestObjInfo(unsigned char netAddress) {sendServiceMessage(netAddress, svcRequestObjInfo);}
    
    void swonbTryConnect(unsigned char mac) {sendServiceMessageToMac(mac, svcHello);}

#ifdef QT_CORE_LIB
public slots:
#endif
    void requestObject(unsigned char netAddress, unsigned char oid);
    void sendObject(unsigned char netAddress, unsigned char oid, const ByteArray &ba);
    void sendServiceRequest(unsigned char netAddress, SvcOID oid, const ByteArray &ba)
    {
        #warning There is copying of the data!
        sendServiceMessage(netAddress, (SvcOID)oid, ByteArray(ba));
    }
    void sendGlobalRequest(StdAID aid, bool propagation, const ByteArray &ba=ByteArray())
    {
        if (propagation)
            aid = static_cast<StdAID>(aid | aidPropagationDown);
        if (ba.size())
            sendGlobalServiceMessage(aid, ba);
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
        id.payload = seq;
        id.aid = aidUpgradeData | aidPropagationDown;
        msg.setGlobalId(id);
//        msg.setData(ba);
        msg.copyData(ba);
        mInterface->write(msg);
    }

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
