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
    aidPropagationUp    = 0x40, //!< ��������� ��������� ����� ������� �� ������� ����
    aidPropagationDown  = 0x80, //!< ��������� ��������� ����� ���� �� ������� ����
    aidPollNodes        = 0x00, //!< ����� �����, ����� �������� ��������� � ������� ���
    aidConnReset        = 0x01 | aidPropagationDown, //!< ����� ��������� ����� �� disconnected, ��������� ���������� ������
//    aidEnumerate        = 0x02, //!< ���������� ����� ����

    aidUpgradeStart     = 0x30, //!< ������ ���������� ��������, � ������ ����� ����������
    aidUpgradeConfirm   = 0x31, //!< ������������� ������ ��������, ����� �� ���� ������������
    aidUpgradeEnd       = 0x32, //!< ��������� ���������� ��������
    aidUpgradeData      = 0x34, //!< ����������, ���� �������� (��. ��������)
    aidUpgradeRepeat    = 0x38, //!< ������ ������� ��������
} StdAID;

//! Service ObjectID enumeration.
typedef enum
{
    svcClass            = 0x00, //!< ���� �������, �� ����� �� ���� ��������
    svcName             = 0x01, //!< ���� ��������, ��� ��� ��������
    svcFullName         = 0x02, //!< ������ ��� ����
    svcSerial           = 0x03, //!< �������� �����
    svcVersion          = 0x04, //!< ������ ��������
    svcBuildDate        = 0x05, //!< ���� � ����� ������
    svcCpuInfo          = 0x06, //!< ���������� � ����������
    svcBurnCount        = 0x07, //!< ��� ������������..
    svcObjectCount      = 0x08, //!< ����� ��������
    // ����� �������� ���-������ ���..
    svcObjectInfo       = 0x80, //!< �������� �������

    svcEcho             = 0xF0, //!< ���� ���������� �������, ��� �� �����
    svcHello            = 0xF1, //!< ��������� ����, ��� �� ������ � ��������, ���� ������ ������ � ��� � ���������� ����: "� ��, ����������, ���?"
    svcWelcome          = 0xF2, //!< ������ ���������� � �����
    svcWelcomeAgain     = 0xF3, //!< ������ ��� ��������� � �����
    svcConnected        = 0xF4, //!< ������ �������� �����, ��� ������ ������
    svcDisconnected     = 0xF5, //!< ������ �����, ��� ������ ����������, � ������� �� ������� ��������
    svcKill             = 0xF6, //!< ������� �������, ��� ������ � ��������, �� ��� ������� � ������������ �� ���� ����
    svcRequestAllInfo   = 0xF8, //!< ������ ����� ����� �� � ������� � ���� ������
    svcRequestObjInfo   = 0xF9, //!< ������ ����� ����� ����������� ���� ���������� �������� �������
    svcAutoRequest      = 0xFA, //!< ������ ������� ����, ��� ����� �� ���� ����� �������� ������ ��� ������ ����
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
    /*! ����������� ��������. */
    LocalMsgId() :
      oid(0), sender(0), frag(0), addr(0), svc(0), mac(0), local(1)
    {
    }
    /*! ������� �����������. */
    LocalMsgId(const unsigned long &data) :
      local(1)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! ���������� ���� � unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! ���������� ���� �� unsigned long */
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
    /*! ����������� ��������. */
    GlobalMsgId() :
      aid(0), res(0), addr(0), svc(0), mac(0), local(0)
    {
    }
    /*! ������� �����������. */
    GlobalMsgId(const unsigned long &data) :
      local(0)
    {
        *reinterpret_cast<unsigned long*>(this) = data;
    }
    /*! ���������� ���� � unsigned long */
    operator unsigned long&() {return *reinterpret_cast<unsigned long*>(this);}
    /*! ���������� ���� �� unsigned long */
    void operator =(const unsigned long &data) {*reinterpret_cast<unsigned long*>(this) = data;}
};

}

#endif
