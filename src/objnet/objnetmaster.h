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
#ifndef __ICCARM__
    Q_OBJECT
#endif
private:
    typedef std::map<unsigned char, ObjnetDevice*> DeviceMap;
    typedef DeviceMap::iterator DeviceIterator;
//    ObjnetDeviceTreeNode mTree;
    DeviceMap mDevices; // map devices by mac
    std::map<unsigned char, unsigned char> mRouteTable; // route table: returns mac by network address
    unsigned char mAssignNetAddress; // network address for assigning to nodes
    bool mAdjIfConnected; // connection state of adjacent interface
  
protected:
#ifdef __ICCARM__
    void task();
#endif

    void parseMessage(CommonMessage &msg);
    
    void acceptServiceMessage(SvcOID oid, ByteArray *ba=0L);
    void parseServiceMessage(CommonMessage &msg);
    
    unsigned char route(unsigned char netAddress) {return netAddress<0x7F? mRouteTable[netAddress]: 0;}
    unsigned char createNetAddress(unsigned char mac);
    
#ifndef __ICCARM__
signals:
    void devAdded(unsigned char netAddress, const QByteArray &locData);
    void devConnected(unsigned char netAddress);
    void devDisconnected(unsigned char netAddress);
    void devRemoved(unsigned char netAddress);
    void serviceMessageAccepted(unsigned char netAddress, SvcOID oid, const QByteArray &data);
#endif
    
#ifndef __ICCARM__
protected slots:
#endif
    void onTimer();

public:
    ObjnetMaster(ObjnetInterface *iface);
    ~ObjnetMaster();

#ifndef __ICCARM__
    void task();
#endif
    void reset();
    
    bool isConnected() const {return !mDevices.empty();}
    
    const DeviceMap &devices() const {return mDevices;}
    void addDevice(unsigned char mac, ObjnetDevice *dev);
    
    void requestName(unsigned char netAddress) {sendServiceMessage(netAddress, svcName);}
    void requestClassId(unsigned char netAddress) {sendServiceMessage(netAddress, svcClass);}
    
//    void sendRemoteMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());
};

};

#endif