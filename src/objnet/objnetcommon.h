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
#include "objnetclasses.h"

/*! @brief Object Network.
    This is a part of STM32++ library providing network connection between stm32 devices
*/
namespace Objnet
{

typedef Closure<void(unsigned char)> GlobalMessageEvent;
typedef Closure<void(unsigned char, const ByteArray&)> GlobalDataMessageEvent;

using namespace std;

//! Standard ActionID enumeration.
typedef enum
{
    aidPropagationUp    = 0x40, //!< пересылка сообщения через мастера на уровень выше
    aidPropagationDown  = 0x80, //!< пересылка сообщения через узлы на уровень ниже
    aidPollNodes        = 0x00, //!< опрос узлов, можно добавить пересылку с помощью ИЛИ
    aidConnReset        = 0x01, //!< сброс состояния узлов до disconnected, установка соединения заново
//    aidEnumerate        = 0x02, //!< построение карты сети

    aidUpgradeStart     = 0x30, //!< запуск обновления прошивки, в данных класс устройства
    aidUpgradeConfirm   = 0x31, //!< подтверждение начала прошивки, чтобы не было случайностей
    aidUpgradeEnd       = 0x32, //!< окончание обновления прошивки
    aidUpgradeSetPage   = 0x33, //!< установка текущей страницы
    aidUpgradeData      = 0x34, //!< собственно, сама прошивка (см. протокол)
    aidUpgradeAddress   = 0x35, //!< адрес начала прошивки
    aidUpgradeProbe     = 0x36, //!< мастер после отправки страницы спрашивает, всё ли ок
    aidUpgradeRepeat    = 0x38, //!< запрос повтора страницы
    aidUpgradePageDone  = 0x39, //!< прошиваемые устройства говорят, что всё хорошо
    aidUpgradeAccepted  = 0x3A, //!< прошиваемые устройства обозначают себя
    aidUpgradeReady     = 0x3B, //!< прошиваемые устройства готовы принимать прошивку
} StdAID;

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
    // можно добавить что-нибудь ещё..
    svcBusType          = 0x09, //!< тип шины физического уровня
    svcBusAddress       = 0x0A, //!< физический адрес (можно поменять)
    svcObjectInfo       = 0x80, //!< описание объекта
    
    svcTimedObject      = 0x90, //!< ответ узла синхронизованным объектом
    svcGroupedObject    = 0xA0, //!< ответ узла группированным объектом

    svcEcho             = 0xF0, //!< узел напоминает мастеру, что он здесь
    svcHello            = 0xF1, //!< сообщение узла, что он пришёл с приветом, либо мастер играет в юща и спрашивает узла: "А вы, собственно, кто?"
    svcWelcome          = 0xF2, //!< мастер приглашает в гости
    svcWelcomeAgain     = 0xF3, //!< мастер уже приглашал в гости
    svcConnected        = 0xF4, //!< мастер сообщает вверх, что пришёл девайс
    svcDisconnected     = 0xF5, //!< мастер понял, что девайс отключился, и передаёт по цепочке главному
    svcKill             = 0xF6, //!< мастеру надоело, что девайс в отключке, он его убивает и рассказывает об этом всем
    svcRequestAllInfo   = 0xF8, //!< мастер хочет знать всё о девайсе и даже больше
    svcRequestObjInfo   = 0xF9, //!< мастер хочет знать дескрипторы всех зареганных объектов девайса
    svcAutoRequest      = 0xFA, //!< мастер говорит узлу, что хочет от него порой получать объект без лишних слов
    svcTimedRequest     = 0xFB, //!< мастер говорит узлу, что хочет получать объект с timestamp'ом
    svcGroupedRequest   = 0xFC, //!< мастер говорит узлу, что хочет получать группированный объект
    svcUpgradeRequest   = 0xFE, //!< мастер просит включить бутлоадер
    svcFail             = 0xFF, //!< узел сообщает мастеру, что это фиаско братан (типа запрос не поддерживается)
} SvcOID;

typedef enum
{
    BusUnknown  = 0,
    BusCan      = 1,
    BusUsbHid   = 2,
    BusWifi     = 3,
    BusSwonb    = 4,
    BusVirtual  = 5,
    BusRadio    = 6
} BusType;

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
    unsigned char _not_used: 3; //!< reserved bits in CAN 2.0B spec
    /*! Конструктор обнуляет. */
    LocalMsgId() :
      oid(0), sender(0), frag(0), addr(0), svc(0), mac(0), local(1), _not_used(0)
    {
    }
    /*! Неявный конструктор. */
    LocalMsgId(const unsigned long &data) :
      local(1), _not_used(0)
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
    unsigned char payload;  //!< contains data if its size is 1 
    unsigned char addr: 7;  //!< own network address (logical)
    unsigned char svc: 1;   //!< message is service
    unsigned char mac: 4;   //!< own bus address (physical)
    unsigned char local: 1; //!< local area message (inside the bus) = 0
    unsigned char _not_used: 3; //!< reserved bits in CAN 2.0B spec
    /*! Конструктор обнуляет. */
    GlobalMsgId() :
      aid(0), payload(0), addr(0), svc(0), mac(0), local(0), _not_used(0)
    {
    }
    /*! Неявный конструктор. */
    GlobalMsgId(const unsigned long &data) :
      local(0), _not_used(0)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! Приведение типа к unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! Приведение типа из unsigned long */
    void operator =(const unsigned long &data) {*reinterpret_cast<unsigned long*>(this) = data;}
};

}

#endif
