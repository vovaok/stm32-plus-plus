#ifndef _OBJNET_COMMON_NODE_H
#define _OBJNET_COMMON_NODE_H

#include <stdarg.h>
#include <map>
#include "objnetInterface.h"
#include "objectinfo.h"

#if !defined(QT_CORE_LIB) && !defined(NRF52840_XXAA)
//    #include "canonbinterface.h"
    #include "gpio.h"
#endif

#if !defined(QT_CORE_LIB)
    #include "core/timer.h"
#else
    #define Timer QTimer
#endif

namespace Objnet
{

#ifndef QT_CORE_LIB
//! Событие, содержащее сообщение Objnet.
typedef Closure<void(const CommonMessage&)> ObjnetMessageEvent;
#endif

/*! Узел сети Objnet.
    Реализует поведение узла (Node) на сетевом и транспортном уровнях.
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

    bool m_receiveBusy = false;
    CommonMessage m_currentMsg;
//    CommonMessage mSheduledMsg; // to be sent later

    // fragmented receive buffers
//    std::map<uint32_t, CommonMessageBuffer> mFragmentBuffer;
    CommonMessageBufferList mFragmentBuffer;
    unsigned char mFragmentSequenceNumber;

    std::map<unsigned char, unsigned char> mNatTable;

    void onNewMessage();
    bool handleMessage(const CommonMessage &msg);
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
    virtual void parseMessage(const CommonMessage &msg) = 0;

//    bool sendServiceMessage(unsigned char receiver, SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendServiceMessage(unsigned char receiver, SvcOID oid, ByteArray &&ba = ByteArray());
    bool sendServiceMessage(unsigned char receiver, SvcOID oid, unsigned char data);
//    void sendServiceMessageSheduled(unsigned char receiver, SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendServiceMessage(SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendServiceMessage(SvcOID oid, unsigned char data);
    bool sendServiceMessageToMac(unsigned char mac, SvcOID oid, const ByteArray &ba = ByteArray());
    bool sendGlobalServiceMessage(StdAID aid, unsigned char payload=0);
    bool sendGlobalServiceMessage(StdAID aid, const ByteArray &ba);
    virtual bool parseServiceMessage(const CommonMessage &msg) = 0;

    virtual unsigned char route(unsigned char netAddress) = 0;
    virtual unsigned char natRoute(unsigned char addr) {return mNatTable.count(addr)? mNatTable[addr]: 0x7F;}
    void addNatPair(unsigned char supernetAddr, unsigned char subnetAddr);
    void removeNatPair(unsigned char supernetAddr, unsigned char subnetAddr);

    friend class ObjnetNode;
    friend class ObjnetMaster;

    virtual void adjacentConnected() {} // called by adjacent interface

#ifndef QT_CORE_LIB
protected:
#else
protected slots:
#endif
    virtual void task();

public:
    /*! Конструктор узла сети Objnet.
        \param iface указатель на интерфейс, через который будет осуществляться доступ к сети.
        Экземпляр ObjnetNode становится владельцем объекта интерфейса, и сам позаботится об его удалении.
    */
    ObjnetCommonNode(ObjnetInterface *iface);

    /*! Деструктор.
        Удаляет объект интерфейса.
    */
    ~ObjnetCommonNode();

    /*! Установка физического адреса непосредственно.
        \param [in] address новый адрес.
    */
    void setBusAddress(unsigned char address = 0xFF);

    /*! Текущий физический адрес.
        \return физический адрес в подсети.
    */
    unsigned char busAddress() const {return mBusAddress;}

#if !defined(QT_CORE_LIB ) && !defined (NRF52840_XXAA)
    /*! Установка физического адреса по сигналам на ногах.
        В функцию передаётся разрядность адреса и имена ножек
        в порядке от младшего бита к старшему.
        \param [in] bits разрядность адреса (количество ног).
        \param [in] a0 имя ноги, отвечающей за 0 разряд адреса, например, Gpio::PE2.
        \param [in] ... остальные ноги (если есть).
    */
    void setBusAddressFromPins(int bits, Gpio::PinName a0 = Gpio::noPin, ...);
    void setBusAddressFromPins(Gpio::PinName a0, Gpio::PinName a1=Gpio::noPin, Gpio::PinName a2=Gpio::noPin, Gpio::PinName a3=Gpio::noPin);
    void setBusAddressFromPinsPD(Gpio::PinName a0, Gpio::PinName a1=Gpio::noPin, Gpio::PinName a2=Gpio::noPin, Gpio::PinName a3=Gpio::noPin);
    void setBusAddress(unsigned char startAddr, Gpio::PinName a0, Gpio::PinName a1=Gpio::noPin, Gpio::PinName a2=Gpio::noPin, Gpio::PinName a3=Gpio::noPin);

//    void setRetranslateEvent(ObjnetMessageEvent event) {mRetranslateEvent = event;}
//    ObjnetMessageEvent retranslateEvent() const {return mRetranslateEvent;}

    void setGlobalMessageEvent(ObjnetMessageEvent event) {mGlobalMessageEvent = event;}
    ObjnetMessageEvent globalMessageEvent() const {return mGlobalMessageEvent;}

    void setMessageEvent(ObjnetMessageEvent event) {mMessageEvent = event;}
    ObjnetMessageEvent messageEvent() const {return mMessageEvent;}
#endif

    void connect(ObjnetCommonNode *node);

    virtual bool isConnected() const = 0;

    /*! Отправка сообщения.
        Отправляет сообщение с указанными параметрами устройству с логическим адресом receiver.
        \param mac Физический адрес получателя. В режиме узла просто опускается.
        \param oid Идентификатор объекта, к которому идёт обращение.
        \param ba  Данные для передачи. Если объект без данных, то можно опустить этот параметр
        \return true в случае успешной передачи, если false - не удалось, нужно повторить
    */
    bool sendMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba = ByteArray());
    bool sendGlobalMessage(unsigned char aid);
    bool sendGlobalMessage(unsigned char aid, const ByteArray &ba);

    ObjnetInterface *objnetInterface() {return mInterface;}
};

};

#endif
