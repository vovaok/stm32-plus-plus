#ifndef _OBJNET_COMMON_H
#define _OBJNET_COMMON_H

#include <string>
#include <typeinfo>
#ifndef QT_CORE_LIB
#include "core/core.h"
#else
#include <QtCore>
#include "closure.h"
#define ByteArray QByteArray
#endif

/*! @brief Object Network.
    This is a part of STM32++ library providing network connection between stm32 devices
*/
namespace Objnet
{

typedef Closure<void(unsigned char)> GlobalMessageEvent;  
  
using namespace std;

//! Standard ActionID enumeration.
typedef enum
{
    aidPropagationUp    = 0x40, //!< пересылка сообщени€ через мастера на уровень выше
    aidPropagationDown  = 0x80, //!< пересылка сообщени€ через узлы на уровень ниже
    aidPollNodes        = 0x00, //!< опрос узлов, можно добавить пересылку с помощью »Ћ»
    aidConnReset        = 0x01 | aidPropagationDown, //!< сброс состо€ни€ узлов до disconnected, установка соединени€ заново
//    aidEnumerate        = 0x02, //!< построение карты сети

    aidUpgradeStart     = 0x30, //!< запуск обновлени€ прошивки, в данных класс устройства
    aidUpgradeConfirm   = 0x31, //!< подтверждение начала прошивки, чтобы не было случайностей
    aidUpgradeEnd       = 0x32, //!< окончание обновлени€ прошивки
    aidUpgradeData      = 0x34, //!< собственно, сама прошивка (см. протокол)
    aidUpgradeRepeat    = 0x38, //!< запрос повтора страницы
} StdAID;

//! Service ObjectID enumeration.
typedef enum
{
    svcClass            = 0x00, //!< узел говорит, из каких он слоЄв общества
    svcName             = 0x01, //!< узел сообщает, как его величать
    svcFullName         = 0x02, //!< полное им€ узла
    svcSerial           = 0x03, //!< серийный номер
    svcVersion          = 0x04, //!< верси€ прошивки
    svcBuildDate        = 0x05, //!< дата и врем€ сборки
    svcCpuInfo          = 0x06, //!< информаци€ о процессоре
    svcBurnCount        = 0x07, //!< без комментариев..
    svcObjectCount      = 0x08, //!< число объектов
    // можно добавить что-нибудь ещЄ..
    svcObjectInfo       = 0x80, //!< описание объекта

    svcEcho             = 0xF0, //!< узел напоминает мастеру, что он здесь
    svcHello            = 0xF1, //!< сообщение узла, что он пришЄл с приветом, либо мастер играет в юща и спрашивает узла: "ј вы, собственно, кто?"
    svcWelcome          = 0xF2, //!< мастер приглашает в гости
    svcWelcomeAgain     = 0xF3, //!< мастер уже приглашал в гости
    svcConnected        = 0xF4, //!< мастер сообщает вверх, что пришЄл девайс
    svcDisconnected     = 0xF5, //!< мастер пон€л, что девайс отключилс€, и передаЄт по цепочке главному
    svcKill             = 0xF6, //!< мастеру надоело, что девайс в отключке, он его убивает и рассказывает об этом всем
    svcRequestAllInfo   = 0xF8, //!< мастер хочет знать всЄ о девайсе и даже больше
    svcRequestObjInfo   = 0xF9, //!< мастер хочет знать дескрипторы всех зареганных объектов девайса
    svcAutoRequest      = 0xFA, //!< мастер говорит узлу, что хочет от него порой получать объект без лишних слов
} SvcOID;

typedef enum
{
    netAddrUniversal    = 0x7F,
    netAddrInvalid      = 0xFF
} NetAddress;

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
    /*!  онструктор обнул€ет. */
    LocalMsgId() :
      oid(0), sender(0), frag(0), addr(0), svc(0), mac(0), local(1)
    {
    }
    /*! Ќе€вный конструктор. */
    LocalMsgId(const unsigned long &data) :
      local(1)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! ѕриведение типа к unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! ѕриведение типа из unsigned long */
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
    /*!  онструктор обнул€ет. */
    GlobalMsgId() :
      aid(0), res(0), addr(0), svc(0), mac(0), local(0)
    {
    }
    /*! Ќе€вный конструктор. */
    GlobalMsgId(const unsigned long &data) :
      local(0)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! ѕриведение типа к unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! ѕриведение типа из unsigned long */
    void operator =(const unsigned long &data) {*reinterpret_cast<unsigned long*>(this) = data;}
};

}

#endif
