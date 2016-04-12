#ifndef _OBJNET_COMMON_NODE_H
#define _OBJNET_COMMON_NODE_H

#include <stdarg.h>
#include <map>
#include "objnetInterface.h"
#include "objectinfo.h"
#ifndef QT_CORE_LIB
#include "canInterface.h"
#include "gpio.h"
#include "timer.h"
#else
#define Timer QTimer
//#include "quartcaninterface.h"
#endif

namespace Objnet
{

#ifndef QT_CORE_LIB
//! �������, ���������� ��������� Objnet.
typedef Closure<void(CommonMessage&)> ObjnetMessageEvent;
#endif

/*! ���� ���� Objnet.
    ��������� ��������� ���� (Node) �� ������� � ������������ �������.
*/
#ifndef QT_CORE_LIB
class ObjnetCommonNode
{
#else
class ObjnetCommonNode : public QObject
{
    Q_OBJECT
#endif
private:
    int mLocalFilter;
    int mGlobalFilter;

#ifndef QT_CORE_LIB
//    ObjnetMessageEvent mRetranslateEvent;
    ObjnetMessageEvent mGlobalMessageEvent;
    ObjnetMessageEvent mMessageEvent;
#endif

    // fragmented receive buffers
    std::map<unsigned long, CommonMessageBuffer> mFragmentBuffer;
    unsigned char mFragmentSequenceNumber;

    std::map<unsigned char, unsigned char> mNatTable;

    bool sendCommonMessage(CommonMessage &msg);

protected:
    ObjnetInterface *mInterface; // pointer to media interface
    ObjnetCommonNode *mAdjacentNode; // bridged node (e.g. master to node et vice versa)
    Timer mTimer;
    unsigned char mBusAddress; // physical address (4 bits wide), 0xFF = invalid bus address
    unsigned char mNetAddress; // logical address (7 bits wide), 0xFF = service address, 0x00 = address of master
    bool mConnected;

    typedef enum {macAuto=0xFF} eBusAddress;

//    void sendGlobalMessage(StdAID aid);
    virtual void parseMessage(CommonMessage &msg) = 0;

    bool sendServiceMessage(unsigned char receiver, SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendServiceMessage(SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendGlobalServiceMessage(StdAID aid);
    virtual void parseServiceMessage(CommonMessage &msg) = 0;

    virtual unsigned char route(unsigned char netAddress) = 0;
    virtual unsigned char natRoute(unsigned char addr) {return mNatTable.count(addr)? mNatTable[addr]: 0x7F;}
    void addNatPair(unsigned char supernetAddr, unsigned char subnetAddr);
    void removeNatPair(unsigned char supernetAddr, unsigned char subnetAddr);

    friend class ObjnetNode;
    friend class ObjnetMaster;

    virtual void acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba=0L) = 0;

#ifndef QT_CORE_LIB
protected:
#else
protected slots:
#endif
    virtual void task();

public:
    /*! ����������� ���� ���� Objnet.
        \param iface ��������� �� ���������, ����� ������� ����� �������������� ������ � ����.
        ��������� ObjnetNode ���������� ���������� ������� ����������, � ��� ����������� �� ��� ��������.
    */
    ObjnetCommonNode(ObjnetInterface *iface);

    /*! ����������.
        ������� ������ ����������.
    */
    ~ObjnetCommonNode();

    /*! ��������� ����������� ������ ���������������.
        \param [in] address ����� �����.
    */
    void setBusAddress(unsigned char address = 0xFF);

    /*! ������� ���������� �����.
        \return ���������� ����� � �������.
    */
    unsigned char busAddress() const {return mBusAddress;}

#ifndef QT_CORE_LIB
    /*! ��������� ����������� ������ �� �������� �� �����.
        � ������� ��������� ����������� ������ � ����� �����
        � ������� �� �������� ���� � ��������.
        \param [in] bits ����������� ������ (���������� ���).
        \param [in] a0 ��� ����, ���������� �� 0 ������ ������, ��������, Gpio::PE2.
        \param [in] ... ��������� ���� (���� ����).
    */
    void setBusAddressFromPins(int bits, Gpio::PinName a0 = Gpio::noPin, ...);

//    void setRetranslateEvent(ObjnetMessageEvent event) {mRetranslateEvent = event;}
//    ObjnetMessageEvent retranslateEvent() const {return mRetranslateEvent;}

    void setGlobalMessageEvent(ObjnetMessageEvent event) {mGlobalMessageEvent = event;}
    ObjnetMessageEvent globalMessageEvent() const {return mGlobalMessageEvent;}

    void setMessageEvent(ObjnetMessageEvent event) {mMessageEvent = event;}
    ObjnetMessageEvent messageEvent() const {return mMessageEvent;}
#endif

    void connect(ObjnetCommonNode *node);

    virtual bool isConnected() const = 0;

    /*! �������� ���������.
        ���������� ��������� � ���������� ����������� ���������� � ���������� ������� receiver.
        \param mac ���������� ����� ����������. � ������ ���� ������ ����������.
        \param oid ������������� �������, � �������� ��� ���������.
        \param ba  ������ ��� ��������. ���� ������ ��� ������, �� ����� �������� ���� ��������
        \return true � ������ �������� ��������, ���� false - �� �������, ����� ���������
    */
    bool sendMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());
    bool sendGlobalMessage(unsigned char aid);

    ObjnetInterface *objnetInterface() {return mInterface;}
};

};

#endif
