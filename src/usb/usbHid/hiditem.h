#ifndef _HIDITEM_H
#define _HIDITEM_H

#include "hidusage.h"

namespace Usb
{
namespace Hid
{
  
typedef enum
{
    Data            = (0<<0),
    Constant        = (1<<0),
    Array           = (0<<1),
    Variable        = (1<<1),
    Absolute        = (0<<2),
    Relative        = (1<<2),
    NoWrap          = (0<<3),
    Wrap            = (1<<3),
    Linear          = (0<<4),
    NonLinear       = (1<<4),
    PreferredState  = (0<<5),
    NoPreffered     = (1<<5),
    NoNullPosition  = (0<<6),
    NullState       = (1<<6),
    NonVolatile     = (0<<7),
    Volatile        = (1<<7),
    BitField        = (0<<8),
    BufferedBytes   = (1<<8)
} IoFlag;

typedef long IoFlags;

typedef enum
{
    Physical        = 0x00,
    Application     = 0x01,
    Logical         = 0x02,
    Report          = 0x03,
    NamedArray      = 0x04,
    UsageSwitch     = 0x05,
    UsageModifier   = 0x06,
    //VendorDefined   = 0x80
} CollectionType;

typedef enum
{
    CloseSet        = 0x00,
    OpenSet         = 0x01
} DelimiterCmd;
//---------------------------------------------------------------------------
  
class Item : public ByteArray
{
public:
    typedef enum
    {
        // Main items
        Input               = 0x80,
        Output              = 0x90,
        Feature             = 0xB0,
        Collection          = 0xA0,
        EndCollection       = 0xC0,
        // Global items
        UsagePage           = 0x04,
        LogicalMinimum      = 0x14,
        LogicalMaximum      = 0x24,
        PhysicalMinimum     = 0x34,
        PhysicalMaximum     = 0x44,
        UnitExponent        = 0x54,
        Unit                = 0x64,
        ReportSize          = 0x74,
        ReportId            = 0x84,
        ReportCount         = 0x94,
        Push                = 0xA4,
        Pop                 = 0xB4,
        // Local items
        Usage               = 0x08,
        UsageMinimum        = 0x18,
        UsageMaximum        = 0x28,
        DesignatorIndex     = 0x38,
        DesignatorMinimum   = 0x48,
        DesignatorMaximum   = 0x58,
        StringIndex         = 0x78,
        StringMinimum       = 0x88,
        StringMaximum       = 0x98,
        Delimiter           = 0xA8
    } ItemType;
  
private:
//    unsigned char bSize: 2;
//    unsigned char bType: 2;
//    unsigned char bTag: 4;
  
    void create(ItemType item, char *data, unsigned char size)
    {
        if (size > 4) // long item
        {
          
        }
        else // short item
        {
            while (!data[size-1] && size>1) // find zero bytes
                --size;
            if (size == 3)
                size = 4;
            append(((char)item) | ((size==4)? 3: size));
            if (size)
                append(data, size);
        }
    }
    
public:
    Item(ItemType item, char *data=0, unsigned char size=0)
    {
        create(item, data, size);
    }
    
    Item(ItemType item, long data)
    {
        create(item, reinterpret_cast<char*>(&data), 4);
    }
};
//---------------------------------------------------------------------------

//---------------------------- MAIN items -----------------------------------
class Input: public Item
{
public: Input(IoFlags flags) : Item(Item::Input, (long)flags) {}
};

class Output: public Item
{
public: Output(IoFlags flags) : Item(Item::Output, (long)flags) {}
};

class Feature: public Item
{
public: Feature(IoFlags flags) : Item(Item::Feature, (long)flags) {}
};

class Collection : public Item
{
public: Collection(CollectionType type) : Item(Item::Collection, (long)type) {}
};

class EndCollection : public Item
{
public: EndCollection() : Item(Item::EndCollection) {}
};

//---------------------------------------------------------------------------

//--------------------------- GLOBAL items ----------------------------------

class UsagePage : public Item
{
public: UsagePage(UsagePageName usagePage) : Item(Item::UsagePage, (long)usagePage) {}
};

class LogicalMinimum : public Item
{
public: LogicalMinimum(int value) : Item(Item::LogicalMinimum, (long)value) {}
};

class LogicalMaximum : public Item
{
public: LogicalMaximum(int value) : Item(Item::LogicalMaximum, (long)value) {}
};

class PhysicalMinimum : public Item
{
public: PhysicalMinimum(int value) : Item(Item::PhysicalMinimum, (long)value) {}
};

class PhysicalMaximum : public Item
{
public: PhysicalMaximum(int value) : Item(Item::PhysicalMaximum, (long)value) {}
};

class UnitExponent : public Item
{
public: UnitExponent(int exponent) : Item(Item::UnitExponent, (long)exponent) {}
};

class Unit : public Item
{
public:
    typedef enum
    {
        none    = 0x00000000,
        cm      = 0x00000011,
        g       = 0x00000101,
        s       = 0x00001001,
        K       = 0x00010001,
        A       = 0x00100001,
        Cd      = 0x01000001,
        rad     = 0x00000012,
        inch    = 0x00000013,
        deg     = 0x00000014,
        cm_s    = 0x0000F011,
        cm_s2   = 0x0000E011,
        rad_s   = 0x0000F012,
        rad_s2  = 0x0000E012,
        N       = 0x0000E111,
        Nm      = 0x0000F111,
        J       = 0x0000E121,
        V       = 0x00F0D121
    } Code;
    Unit(Code unit) : Item(Item::Unit, (long)unit) {}
};

class ReportSize : public Item
{
public: ReportSize(int bits) : Item(Item::ReportSize, (long)bits) {}
};

class ReportId : public Item
{
public: ReportId(unsigned char id) : Item(Item::ReportId, (long)id) {}
};
                                          
class ReportCount : public Item
{
public: ReportCount(int count) : Item(Item::ReportCount, (long)count) {}
};

class Push : public Item
{
public: Push() : Item(Item::Push) {}
};

class Pop : public Item
{
public: Pop() : Item(Item::Pop) {}
};
//---------------------------------------------------------------------------

//---------------------------- LOCAL items ----------------------------------
  
class Usage : public Item
{
public: Usage(unsigned short usage) : Item(Item::Usage, (long)usage) {}  
};

class UsageMinimum : public Item
{
public: UsageMinimum(unsigned short usage) : Item(Item::UsageMinimum, (long)usage) {}  
};

class UsageMaximum : public Item
{
public: UsageMaximum(unsigned short usage) : Item(Item::UsageMaximum, (long)usage) {}  
};

class DesignatorIndex : public Item
{
public: DesignatorIndex(int index) : Item(Item::DesignatorIndex, (long)index) {}
};

class DesignatorMinimum : public Item
{
public: DesignatorMinimum(int index) : Item(Item::DesignatorMinimum, (long)index) {}
};

class DesignatorMaximum : public Item
{
public: DesignatorMaximum(int index) : Item(Item::DesignatorMaximum, (long)index) {}
};

class StringIndex : public Item
{
public: StringIndex(unsigned char index) : Item(Item::StringIndex, (long)index) {}
};

class StringMinimum : public Item
{
public: StringMinimum(unsigned char index) : Item(Item::StringMinimum, (long)index) {}
};

class StringMaximum : public Item
{
public: StringMaximum(unsigned char index) : Item(Item::StringMaximum, (long)index) {}
};

class Delimiter : public Item
{
public: Delimiter(DelimiterCmd cmd) : Item(Item::Delimiter, (long)cmd) {}
};
//---------------------------------------------------------------------------

}
}

#endif