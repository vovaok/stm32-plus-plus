#ifndef _CIA402_H
#define _CIA402_H

#include "canopenproxy.h"
#include <array>

#define ENTRY(name) struct name: public CanOpenProxy::ODEntryMeta
#define INHERITED(name, parent) struct name: public parent, public CanOpenProxy::ODEntryMeta

namespace CanOpen::CiA402
{

namespace Common
{
    struct OperationMode {
        enum Values: int8_t        
        {
            PPosition = 1,
            PVelocity = 3,
            PTorque   = 4,
            Homing    = 6,
            IPosition = 7,
            CPosition = 8,
            CVelocity = 9,
            CTorque   = 10
        };
    };
}

// Maybe move to cia301dict.h?
ENTRY(ErrorRegister)<uint8_t, 0x1001>
{
    enum Bits: uint8_t
    {
        Generic       = 1 << 0,
        Current       = 1 << 1,
        Voltage       = 1 << 2,
        Temperature   = 1 << 3,
        Communication = 1 << 4,
        Device        = 1 << 5,
        Manufacturer  = 1 << 7
    };
};

ENTRY(ControlWord)<uint16_t, 0x6040>
{
    enum Bits: uint16_t
    {
        /* Common bits for all profiles */
        SwitchOn          = 1 << 0,
        EnableVoltage     = 1 << 1,
        QuickStop         = 1 << 2,
        EnableOperation   = 1 << 3,
        FaultReset        = 1 << 7,
        Halt              = 1 << 8,
    
        /* Bits for Profiled Position (PP) mode */
        PP_NewSetPoint    = 1 << 4,
        PP_ImmSetPoint    = 1 << 5,
        PP_TargetRelative = 1 << 6,
        PP_Blended        = 1 << 9,
        
        /* PT, PV, IP, etc profiles are WIP */
        
        /* Common shortcuts */
        CMD_PowerOn  = SwitchOn | EnableVoltage | QuickStop | EnableOperation,
        CMD_PowerOff = EnableVoltage | QuickStop
    };
};

ENTRY(StatusWord)<uint16_t, 0x6041> 
{
    enum Bits: uint16_t
    {
        /* Common bits for all profiles */
        ReadyOn             = 1 << 0,
        SwitchedOn          = 1 << 1,
        OperationEnabled    = 1 << 2,
        Fault               = 1 << 3,
        VoltageEnabled      = 1 << 4,
        QuickStop           = 1 << 5,
        SwitchOnDisabled    = 1 << 6,
        Warning             = 1 << 7,
        Remote              = 1 << 9,
        TargetReached       = 1 << 10,
        InternalLimit       = 1 << 11,
        
        /* Bits for Profiled Position (PP) mode */
        PP_SetpointAck       = 1 << 12,
        PP_FollowingError    = 1 << 13,
        
        /* PT, PV, IP, etc profiles are WIP */  
    };
};

INHERITED(SetOpMode, Common::OperationMode)<int8_t, 0x6060> {};
INHERITED(GetOpMode, Common::OperationMode)<int8_t, 0x6061> {};

ENTRY(PosRangeLimitMin)<uint32_t, 0x607B, 1> {};
ENTRY(PosRangeLimitMax)<uint32_t, 0x607B, 2> {};

ENTRY(PosSoftLimitMin)<uint32_t, 0x607D, 1> {};
ENTRY(PosSoftLimitMax)<uint32_t, 0x607D, 2> {};

ENTRY(TargetPos)<int32_t, 0x607A> {};

ENTRY(MaxProfVelocity)<uint32_t, 0x607F> {};
ENTRY(ProfVelocity)<uint32_t, 0x6081> {};

ENTRY(MaxAcceleration)<uint32_t, 0x60C5> {};
ENTRY(ProfAcceleration)<uint32_t, 0x6083> {};

ENTRY(MaxDeceleration)<uint32_t, 0x60C6> {};
ENTRY(ProfDeceleration)<uint32_t, 0x6084> {};
ENTRY(QStopDeceleration)<uint32_t, 0x6085> {};

ENTRY(SupportedOpMode)<uint32_t, 0x6502>
{
    enum Bits: uint32_t
    {
        PPosition = 1 << 0,
        PVelocity = 1 << 2,
        PTorque   = 1 << 3,
        Homing    = 1 << 5,
        IPosition = 1 << 6,
        CPosition = 1 << 7,
        CVelocity = 1 << 8,
        CTorque   = 1 << 9
    };
};

template<uint32_t FunctionCode, uint32_t CommIndex, uint32_t MapIndex>
struct PDOEntry
{
public:
    using MapT = std::array<CanOpenProxy::ODEntry<uint32_t>, 8>;
    
private:
    static constexpr MapT makeMap()
    {
        MapT map {};
        
        for (uint8_t i = 0; i < 8; ++i)
            map[i] = {MapIndex, i + 1};
        
        return map;
    }
public:
    ENTRY(cobId)<uint32_t, CommIndex, 1> {};
    
    // Communtication parameters
    ENTRY(transmissonType)<uint8_t, CommIndex, 2>
    {
        enum Values: uint8_t {
            ManufacturerAsync = 0xFE,
            ProfileAsync = 0xFF
        };
    };
    
    ENTRY(mapLen)<uint8_t, MapIndex, 0> {};
    
    static constexpr MapT map = makeMap();
};

template<uint32_t FunctionCode, uint32_t CommIndex, uint32_t MapIndex>
struct TPDOEntry: public PDOEntry<FunctionCode, CommIndex, MapIndex>
{    
    ENTRY(inhibitTime)<uint16_t, CommIndex, 3> {};
    ENTRY(eventTimer)<uint16_t, CommIndex, 5> {};
};

using TPDO1 = TPDOEntry<PDO1_TX, 0x1800, 0x1A00>;
using TPDO2 = TPDOEntry<PDO2_TX, 0x1801, 0x1A01>;
using TPDO3 = TPDOEntry<PDO3_TX, 0x1802, 0x1A02>;
using TPDO4 = TPDOEntry<PDO4_TX, 0x1803, 0x1A03>;

using RPDO1 = PDOEntry<PDO1_RX, 0x1400, 0x1600>;
using RPDO2 = PDOEntry<PDO2_RX, 0x1401, 0x1601>;
using RPDO3 = PDOEntry<PDO3_RX, 0x1402, 0x1602>;
using RPDO4 = PDOEntry<PDO4_RX, 0x1403, 0x1603>;

}
#undef ENTRY
#undef INHERITED

#endif // _CIA402_H