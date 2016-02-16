#ifndef _OBJNET_COMMON_H
#define _OBJNET_COMMON_H

#include <string>
#include <typeinfo>
#ifdef __ICCARM__
#include "core/core.h"
#endif

/*! @brief Object Network.
    This is a part of STM32++ library providing network connection between stm32 devices
*/
namespace Objnet
{
  
using namespace std;

//! Standard ActionID enumeration.
typedef enum
{
    aidPropagationUp    = 0x40, //!< propagation up through master
    aidPropagationDown  = 0x80, //!< propagation down through nodes
    aidPollNodes        = 0x00, //!< poll net nodes, propagation can be OR'ed if needed
    aidConnReset        = 0x01 | aidPropagationDown, //!< reset the connection state of nodes to disconnected state, so performing connection sequence again
//    aidEnumerate        = 0x01, //!< enumerate net nodes, makes nodes send subnet tree info
} StdAID;

//! Standard ObjectID enumeration.
typedef enum
{
    oidStdOffset        = 0xE0,                //!< beginning of standard ObjectID's
//    oidPing             = 0x00 + oidStdOffset, //!< Node returns empty message
//    oidName             = 0x01 + oidStdOffset, //!< Node returns its name by request
//    oidEnumerate        = 0x02 + oidStdOffset, //!< Master sends net address (logical) of this node, node response enumeration success
    
} StdOID;

//! Service ObjectID enumeration.
typedef enum
{    
    svcClass            = 0x00, //!< узел говорит, из каких он слоёв общества
    svcName             = 0x01, //!< узел сообщает, как его величать
    svcFullName         = 0x02, //!< полное имя узла
    svcSerial           = 0x03, //!< серийный номер
    svcVersion          = 0x04, //!< версия прошивки
    svcBuildDate        = 0x05, //!< дата и время сборки
    svcCpuInfo          = 0x06, //!< информация о процессоре
    svcBurnCount        = 0x07, //!< без комментариев..
    svcObjectCount      = 0x08, //!< число объектов
    
    svcEcho             = 0xF0, //!< узел напоминает мастеру, что он здесь
    svcHello            = 0xF1, //!< сообщение узла, что он пришёл с приветом, либо мастер играет в юща и спрашивает узла: "А вы, собственно, кто?"
    svcWelcome          = 0xF2, //!< мастер приглашает в гости
    svcWelcomeAgain     = 0xF3, //!< мастер уже приглашал в гости
    svcConnected        = 0xF4, //!< мастер сообщает вверх, что пришёл девайс
    svcDisconnected     = 0xF5, //!< мастер понял, что девайс отключился, и передаёт по цепочке главному
    svcKill             = 0xF6, //!< мастеру надоело, что девайс в отключке, он его убивает и рассказывает об этом всем
} SvcOID;

//! Local Message ID type.
struct LocalMsgId
{
    unsigned char oid;          //!< object ID
    unsigned char sender: 7;    //!< sender network address (logical)
    unsigned char frag: 1;      //!< message is fragmented
    unsigned char addr: 7;      //!< receiver network address (logical)
    unsigned char svc: 1;       //!< message is service
    unsigned char mac: 4;       //!< receiver bus address (physical)
    const unsigned char local: 1; //!< local area message (inside the bus) = 1
    /*! Конструктор обнуляет. */
    LocalMsgId() :
      oid(0), sender(0), frag(0), addr(0), svc(0), mac(0), local(1)
    {
    }
    /*! Неявный конструктор. */
    LocalMsgId(const unsigned long &data) :
      local(1)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! Приведение типа к unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! Приведение типа из unsigned long */
    void operator =(const unsigned long &data) {*reinterpret_cast<unsigned long*>(this) = data;}
};

//! Global Message ID type.
struct GlobalMsgId
{
    unsigned char aid;      //!< action ID
    unsigned char res: 8;   //!< reserved
    unsigned char addr: 7;  //!< own network address (logical)
    unsigned char svc: 1;   //!< message is service
    unsigned char mac: 4;   //!< own bus address (physical)
    unsigned char local: 1; //!< local area message (inside the bus) = 0
    /*! Конструктор обнуляет. */
    GlobalMsgId() :
      aid(0), res(0), addr(0), svc(0), mac(0), local(0)
    {
    }
    /*! Неявный конструктор. */
    GlobalMsgId(const unsigned long &data) :
      local(0)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! Приведение типа к unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! Приведение типа из unsigned long */
    void operator =(const unsigned long &data) {*reinterpret_cast<unsigned long*>(this) = data;}
};

class ObjectInfo
{
private:
    void *mReadPtr, *mWritePtr;
    size_t mReadSize, mWriteSize;
    string mName;
    
    friend class ObjnetNode;
    
public:
    ObjectInfo() :
        mReadPtr(0L), mWritePtr(0L), mReadSize(0), mWriteSize(0)
    {
    }
    
    ObjectInfo &bindVariableRO(string name, void *ptr, size_t size)
    {
        mReadPtr = ptr;
        mReadSize = size;
        mWritePtr = 0;
        mWriteSize = 0;
        mName = name;
        return *this;
    }
    
    ObjectInfo &bindVariableWO(string name, void *ptr, size_t size)
    {
        mReadPtr = 0;
        mReadSize = 0;
        mWritePtr = ptr;
        mWriteSize = size;
        mName = name;
        return *this;
    }
    
    ObjectInfo &bindVariableRW(string name, void *ptr, size_t size)
    {
        mReadPtr = mWritePtr = ptr;
        mReadSize = mWriteSize = size;
        mName = name;
        return *this;
    }
    
    ObjectInfo &bindVariableInOut(string name, void *inPtr, size_t inSize, void *outPtr, size_t outSize)
    {
        mReadPtr = outPtr;
        mReadSize = outSize;
        mWritePtr = inPtr;
        mWriteSize = inSize;
        mName = name;
        return *this;
    }
};   
    
}

#endif