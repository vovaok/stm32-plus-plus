#pragma once

#include "i2c.h"

enum class SBS: uint8_t
{
    // standard SBS command set:
    ManufacturerAccess = 0x00,
    RemainingCapacityAlarm = 0x01,
    RemainingTimeAlarm = 0x02,
    BatteryMode = 0x03,
    AtRate = 0x04,
    AtRateTimeToFull = 0x05,
    AtRateTimeToEmpty = 0x06,
    AtRateOK = 0x07,
    Temperature = 0x08,
    Voltage = 0x09,
    Current = 0x0a,
    AverageCurrent = 0x0b,
    MaxError = 0x0c,
    RelativeStateOfCharge = 0x0d,
    AbsoluteStateOfCharge = 0x0e,
    RemainingCapacity = 0x0f,
    FullChargeCapacity = 0x10,
    RunTimeToEmpty = 0x11,
    AverageTimeToEmpty = 0x12,
    AverageTimeToFull = 0x13,
    ChargingCurrent = 0x14,
    ChargingVoltage = 0x15,
    BatteryStatus = 0x16,
    CycleCount = 0x17,
    DesignCapacity = 0x18,
    DesignVoltage = 0x19,
    SpecificationInfo = 0x1a,
    ManufactureDate = 0x1b,
    SerialNumber = 0x1c,
    ManufacturerName = 0x20,
    DeviceName = 0x21,
    DeviceChemistry = 0x22,
    ManufacturerData = 0x23,
    Authenticate = 0x2f,
    // OptionalMfgFunction 4-1:
    CellVoltage4 = 0x3c,
    CellVoltage3 = 0x3d,
    CellVoltage2 = 0x3e,
    CellVoltage1 = 0x3f,
        
    // extended SBS command set:
    AFEData = 0x45,
    FETControl = 0x46,
    StateOfHealth = 0x4f,
    SafetyStatus = 0x51,
    PFStatus = 0x53,
    OperationStatus = 0x54,
    ChargingStatus = 0x55,
    ResetData = 0x57,
    PackVoltage = 0x5a,
    AverageVoltage = 0x5d,
    UnSealKey = 0x60,
    FullAccessKey = 0x61,
    PFKey = 0x62,
    AuthenKey3 = 0x63,
    AuthenKey2 = 0x64,
    AuthenKey1 = 0x65,
    AuthenKey0 = 0x66,
    ManufacturerInfo = 0x70,
    SenseResistor = 0x71,
    DataFlashSubClassID = 0x77,
    DataFlashSubClassPage1 = 0x78,
    DataFlashSubClassPage2 = 0x79,
    DataFlashSubClassPage3 = 0x7a,
    DataFlashSubClassPage4 = 0x7b,
    DataFlashSubClassPage5 = 0x7c,
    DataFlashSubClassPage6 = 0x7d,
    DataFlashSubClassPage7 = 0x7e,
    DataFlashSubClassPage8 = 0x7f
};

class SmartBattery
{
public:
    SmartBattery(I2c *i2c);
    
    bool isValid() {return m_valid;}
    
    //! Specific bq20z75 commands, @todo place them into the separate class
    enum SystemData : uint16_t
    {
        DeviceType = 0x0001,
        FirmwareVersion = 0x0002,
        HardwareVersion = 0x0003,
        DFChecksum = 0x0004,
        ManufacturerStatus = 0x0006,
        ChemistryID = 0x0008,
        SafetyStatus = 0x0051,
        PFStatus = 0x0053,
        OperationStatus = 0x0054,
        ChargingStatus = 0x0055,
        ResetData = 0x0057,
        PackVoltage = 0x005a,
        AverageVoltage = 0x005d,
    };
    
    enum SystemControl
    {
        Shutdown = 0x0010,
        Sleep = 0x0011,
        SealDevice = 0x0020,
        ITEnable = 0x0021,
        SAFEActivation = 0x0030,
        SAFEClear = 0x0031,
        CalibrationMode = 0x0040,
        Reset = 0x0041,
        BootROM = 0x0f00,
    };
    
    uint16_t readWord(SBS cmd);
    uint16_t readSys(SystemData cmd);
    ByteArray readBlock(SBS cmd);
    
    void writeWord(SBS cmd, uint16_t word);
    void enterKey(uint32_t key);
    void sendControl(SystemControl cmd);
    
    ByteArray readSubclass(uint8_t id, uint8_t page);
    void writeSubclass(uint8_t id, uint8_t page, ByteArray ba);

private:
    I2c::Device *m_dev = nullptr;
    bool m_valid = false;
};
