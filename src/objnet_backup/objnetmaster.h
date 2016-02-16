#ifndef _OBJNET_MASTER
#define _OBJNET_MASTER

#include <map>
#include "objnetCommonNode.h"
#include "objnetdevice.h"

using namespace std;

namespace Objnet
{

//class ObjnetDeviceTreeNode
//{
//private:
//    typedef std::map<unsigned char, ObjnetDeviceTreeNode>::iterator Iterator;
//    typedef union
//    {
//        unsigned char byte;
//        struct
//        {
//            unsigned char mac: 4;
//            unsigned char cnt: 4;
//        }; 
//    } TreeByte;
//    
//    static unsigned char mAddressToAssign;
//    ObjnetDeviceTreeNode *mParent;
//    std::map<unsigned char, ObjnetDeviceTreeNode> mChildren;
//    unsigned char mAddress;
//    bool mPresent;
//    bool mStateChanged;
// 
//public:
//    ObjnetDeviceTreeNode(ObjnetDeviceTreeNode *parent = 0L) :
//      mParent(parent),
//      mAddress(mAddressToAssign++),
//      mPresent(true),
//      mStateChanged(true)
//    {
//    }
//    
//    ObjnetDeviceTreeNode &operator[](unsigned char mac) {return mChildren[mac];}
//    //void operator <<(unsigned char mac) {mChildren[mac] = ObjnetDeviceTreeNode(this);}
//    
//    bool isPresent() const {return mPresent;}
//    bool isStateChanged() {if (mStateChanged) {mStateChanged = false; return true;} return false;}
//    unsigned char address() const {return mAddress;}
//    void assignAddress(unsigned char address) {mAddress = address;}
//    
//    void serialize(ByteArray &ba, unsigned char mac)
//    {
//        mac = (mac & 0xF) | (mChildren.size() << 4);
//        ba.append(mac);
//        for (Iterator it=mChildren.begin(); it!=mChildren.end(); ++it)
//            it->second.serialize(ba, it->first);
//    }
//    
//    void unserialize(const ByteArray &ba, int pos=0)
//    {
//        if (pos < ba.size())
//        {
//            TreeByte b = {ba[pos++]};
//            ObjnetDeviceTreeNode &child = mChildren[b.mac];
//            child.mParent = this;
//            if (!child.mPresent)
//                child.mStateChanged = true;
//            child.mPresent = true;
//            for (int i=0; i<b.cnt; i++, pos++)
//                child.unserialize(ba, pos);
//        }
//    }
//    
//    void setDisconnected()
//    {
//        mPresent = false;
//        for (Iterator it=mChildren.begin(); it!=mChildren.end(); ++it)
//            it->second.setDisconnected();
//    }
//};
  
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
    
    unsigned char route(unsigned char netAddress) {return netAddress < 128? mRouteTable[netAddress]: 0;}
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
    
    void requestName(unsigned char netAddress) {sendServiceMessage(svcName, route(netAddress), ByteArray(), netAddress);}
    void requestClassId(unsigned char netAddress) {sendServiceMessage(svcClass, route(netAddress), ByteArray(), netAddress);}
    
    void sendRemoteMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());
};

};

#endif