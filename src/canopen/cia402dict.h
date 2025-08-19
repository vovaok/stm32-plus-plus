#ifndef _CIA402_H
#define _CIA402_H

#include "canopenproxy.h"

#define ENTRY const struct : CanOpenProxy::ODEntry
#define INHERITED(parent) const struct: parent, CanOpenProxy::ODEntry 

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
ENTRY<uint8_t, 0x1001>
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
} ErrorRegister;

ENTRY<uint16_t, 0x6040>
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
} ControlWord;

ENTRY<uint16_t, 0x6041> 
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
} StatusWord;

INHERITED(Common::OperationMode)<int8_t, 0x6060> {} SetOpMode;
INHERITED(Common::OperationMode)<int8_t, 0x6061> {} GetOpMode;

ENTRY<uint32_t, 0x607B, 1> {} PosRangeLimitMin;
ENTRY<uint32_t, 0x607B, 2> {} PosRangeLimitMax ;

ENTRY<uint32_t, 0x607D, 1> {} PosSoftLimitMin;
ENTRY<uint32_t, 0x607D, 2> {} PosSoftLimitMax;

ENTRY<int32_t, 0x607A> {} TargetPos;

ENTRY<uint32_t, 0x607F> {} MaxProfVelocity;
ENTRY<uint32_t, 0x6081> {} ProfVelocity;

ENTRY<uint32_t, 0x60C5> {} MaxAcceleration;
ENTRY<uint32_t, 0x6083> {} ProfAcceleration;

ENTRY<uint32_t, 0x60C6> {} MaxDeceleration;
ENTRY<uint32_t, 0x6084> {} ProfDeceleration;
ENTRY<uint32_t, 0x6085> {} QStopDeceleration;

ENTRY<uint32_t, 0x6502>
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
} SupportedOpMode;

}

#undef ENTRY
#undef INHERITED

#endif // _CIA402_H