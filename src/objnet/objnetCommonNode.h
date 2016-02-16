#ifndef _OBJNET_COMMON_NODE_H
#define _OBJNET_COMMON_NODE_H

#include <stdarg.h>
#include <map>
#include "objnetInterface.h"
#ifdef __ICCARM__
#include "canInterface.h"
#include "gpio.h"
#include "timer.h"
#else
#define Timer QTimer
#include "quartcaninterface.h"
#endif

namespace Objnet
{
  
#ifdef __ICCARM__
//! �������, ���������� ��������� Objnet.
typedef Closure<void(CommonMessage&)> ObjnetMessageEvent;
#endif
 
/*! ���� ���� Objnet.
    ��������� ��������� ���� (Node) �� ������� � ������������ �������.
*/
#ifdef __ICCARM__
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
    
#ifdef __ICCARM__
//    ObjnetMessageEvent mRetranslateEvent;
    ObjnetMessageEvent mGlobalMessageEvent;
    ObjnetMessageEvent mMessageEvent;
#endif
    
    // fragmented receive buffers   
    std::map<unsigned long, CommonMessageBuffer> mFragmentBuffer;
    unsigned char mFragmentSequenceNumber;
    
    void sendCommonMessage(CommonMessage &msg);
    
protected:
    ObjnetInterface *mInterface; // pointer to media interface
    ObjnetCommonNode *mAdjacentNode; // bridged node (e.g. master to node et vice versa)
    Timer mTimer;
    unsigned char mBusAddress; // physical address (4 bits wide), 0xFF = invalid bus address
    unsigned char mNetAddress; // logical address (8 bits wide), 0xFF = service address, 0x00 = address of master
    bool mConnected;
    
    typedef enum {macAuto=0xFF} eBusAddress;
    
    virtual void task(); 
    
//    void sendGlobalMessage(StdAID aid);
    virtual void parseMessage(CommonMessage &msg) = 0;

    void sendServiceMessage(unsigned char receiver, SvcOID oid, const ByteArray &ba = ByteArray());
    void sendServiceMessage(SvcOID oid, const ByteArray &ba = ByteArray());
    void sendGlobalServiceMessage(StdAID aid);
    virtual void parseServiceMessage(CommonMessage &msg) = 0;
    
    virtual unsigned char route(unsigned char netAddress) = 0;
    
    friend class ObjnetNode;
    friend class ObjnetMaster;
    
    virtual void acceptServiceMessage(SvcOID oid, ByteArray *ba=0L) = 0;
    
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
    
#ifdef __ICCARM__
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
    ObjnetMessageEvent GlobalMessageEvent() const {return mGlobalMessageEvent;}
    
    void setMessageEvent(ObjnetMessageEvent event) {mMessageEvent = event;}
    ObjnetMessageEvent MessageEvent() const {return mMessageEvent;}
#endif
    
    void connect(ObjnetCommonNode *node);
    
    virtual bool isConnected() const = 0;
    
    /*! �������� ���������.
        ���������� ��������� � ���������� ����������� ���������� � ���������� ������� receiver.
        \param mac ���������� ����� ����������. � ������ ���� ������ ����������.
        \param oid ������������� �������, � �������� ��� ���������.
        \param ba  ������ ��� ��������. ���� ������ ��� ������, �� ����� �������� ���� ��������
    */
    void sendMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());
    void sendGlobalMessage(unsigned char aid);
};

};

#endif