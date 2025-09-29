#ifndef _CANOPEN_OBJDICT_H
#define _CANOPEN_OBJDICT_H

#include "canopenproxy.h"

// Macros for a shoter syntax
#define ENTRY(name) struct name: public CanOpenProxy::ODEntryMeta
#define INHERITED(name, parent) struct name: public parent, public CanOpenProxy::ODEntryMeta

namespace CanOpen::ObjDict
{

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

using TPDO1 = CanOpenProxy::PDOEntryMeta<PDO1_TX, 0x1800, 0x1A00, true>;
using TPDO2 = CanOpenProxy::PDOEntryMeta<PDO2_TX, 0x1801, 0x1A01, true>;
using TPDO3 = CanOpenProxy::PDOEntryMeta<PDO3_TX, 0x1802, 0x1A02, true>;
using TPDO4 = CanOpenProxy::PDOEntryMeta<PDO4_TX, 0x1803, 0x1A03, true>;

using RPDO1 = CanOpenProxy::PDOEntryMeta<PDO1_RX, 0x1400, 0x1600, false>;
using RPDO2 = CanOpenProxy::PDOEntryMeta<PDO2_RX, 0x1401, 0x1601, false>;
using RPDO3 = CanOpenProxy::PDOEntryMeta<PDO3_RX, 0x1402, 0x1602, false>;
using RPDO4 = CanOpenProxy::PDOEntryMeta<PDO4_RX, 0x1403, 0x1603, false>;

ENTRY(AnalogInputMiliVolts)<int16_t, 0x2205, 1> {};
ENTRY(AnalogInputA2DTicks)<int16_t, 0x2205, 2> {};

namespace CiA402
{

struct OperationMode
{
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
        CmdPowerOff  = EnableVoltage | QuickStop,
        CmdPowerOn   = SwitchOn | EnableVoltage | QuickStop,
        CmdEnable    = CmdPowerOn | EnableOperation,
        CmdQuickStop = CmdEnable & ~QuickStop,
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

INHERITED(SetOpMode, OperationMode)<int8_t, 0x6060> {};
INHERITED(GetOpMode, OperationMode)<int8_t, 0x6061> {};

ENTRY(PositionActualValue)<int32_t, 0x6064> {};
ENTRY(VelocityActualValue)<int32_t, 0x606C> {};

// Profiled Torque Mode:
ENTRY(TargetTorque)<int16_t, 0x6071> {};
ENTRY(MaxTorque)<uint16_t, 0x6072> {};
ENTRY(MaxCurrent)<uint16_t, 0x6073> {};
ENTRY(TorqueDemandValue)<int16_t, 0x6074> {};
ENTRY(MotorRatedCurrent)<uint32_t, 0x6075> {};
ENTRY(MotorRatedTorque)<uint32_t, 0x6076> {};
ENTRY(TorqueActualValue)<int16_t, 0x6077> {};
ENTRY(CurrentActualValue)<int16_t, 0x6078> {};

ENTRY(PosRangeLimitMin)<uint32_t, 0x607B, 1> {};
ENTRY(PosRangeLimitMax)<uint32_t, 0x607B, 2> {};

ENTRY(PosSoftLimitMin)<uint32_t, 0x607D, 1> {};
ENTRY(PosSoftLimitMax)<uint32_t, 0x607D, 2> {};

ENTRY(TargetPos)<int32_t, 0x607A> {};

ENTRY(MaxProfileVelocity)<uint32_t, 0x607F> {};
ENTRY(ProfileVelocity)<uint32_t, 0x6081> {};

ENTRY(MaxAcceleration)<uint32_t, 0x60C5> {};
ENTRY(ProfileAcceleration)<uint32_t, 0x6083> {};

ENTRY(MaxDeceleration)<uint32_t, 0x60C6> {};
ENTRY(ProfileDeceleration)<uint32_t, 0x6084> {};
ENTRY(QStopDeceleration)<uint32_t, 0x6085> {};

ENTRY(DcVoltage)<uint32_t, 0x6079> {};

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

}
}
#undef ENTRY
#undef INHERITED

#endif // _CANOPEN_OBJDICT_H