#ifndef _I2C_H
#define _I2C_H

#include "gpio.h"
#include "rcc.h"

//! @todo check I2C revision in other families
#if defined(STM32F3) || defined(STM32F7) || defined(STM32F0)
#define I2C_REV03
#endif

//! I2C master only implementation
//!
class I2c
{
public:
    I2c(Gpio::Config pinSDA, Gpio::Config pinSCL);

    void setBusClock(int clk_Hz);
//    void setAddress(uint8_t address);
    void setSMBusHostMode();

    void open();
    void close();

     //! @todo maybe make this inherited from ::Device?
    class Device // : public ::Device
    {
    public:
        void setRegSize(int size)
        {
            if (size > 0 && size <= 4)
                m_regSize = size;
        }

//        void setBusClock(int clk_Hz)
//        {
//            m_busClock = clk_Hz;
//        }

        bool readReg(uint32_t regAddr, uint8_t *data, int size)
        {
            bool r = true;
//            m_i2c->setBusClock(m_busClock);
            r &= m_i2c->writeRegAddr(m_addr, regAddr, m_regSize);
            r &= m_i2c->read(m_addr, data, size);
            return r;
        }

        bool writeReg(uint32_t regAddr, const uint8_t *data, int size)
        {
            bool r = true;
//            m_i2c->setBusClock(m_busClock);
            r &= m_i2c->writeRegAddr(m_addr, regAddr, m_regSize);
            r &= m_i2c->write(m_addr, data, size);
            return r;
        }

    private:
        friend class I2c;
        Device(uint8_t address) : m_addr(address) {}
        I2c *m_i2c;
        uint8_t m_addr;
        // int m_busClock = 100000;
        int m_regSize = 1; // the size of register address
    };

    I2c::Device *createDevice(uint8_t address);

//    // high-level interface (some implementation)
//    bool readReg(uint8_t hw_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size);
//    bool writeReg(uint8_t hw_addr, uint8_t reg_addr, const uint8_t *buffer, uint8_t size);

protected: //! @todo reimplement dependent classes to high-level interface (using I2c::Device)
public:
    //! writeRegAddr() writes START, 7-bit chip address, R/W=0,
    //! then <regSize> bytes of <regAddr> with big endian order
    bool writeRegAddr(uint8_t address, uint32_t regAddr, int regSize);

    //! write() writes START, 7-bit chip address, R/W=0,
    //! then <size> bytes of <data>
    bool write(uint8_t address, const uint8_t *data, int size);

    //! read() writes START, 7-bit chip address, R/W=1,
    //! then reads <size> bytes of <data>
    bool read(uint8_t address, uint8_t *data, int size);

    typedef enum
    {
        DirectionTransmitter    = 0x00,
        DirectionReceiver       = 0x01
    } Direction;

    bool startTransmission(Direction dir, uint8_t slaveAddress);
    bool stopTransmission();
    bool writeData(uint8_t data);
    bool readData(uint8_t *buf);
    void setAcknowledge(bool state);

private:
    I2C_TypeDef *m_dev;

    // low-level interface:




    // the legacy of the SPL:
    typedef enum
    {
        EventMasterModeSelect                       = 0x00030001,
        EventMasterTransmitterModeSelected          = 0x00070082,
        EventMasterReceiverModeSelected             = 0x00030002,
        EventMasterModeAddress10                    = 0x00030008,
        EventMasterByteReceived                     = 0x00030040,
        EventMasterByteTransmitting                 = 0x00070080,
        EventMasterByteTransmitted                  = 0x00070084,
        EventSlaveReceiverAddressMatched            = 0x00020002,
        EventSlaveTransmitterAddressMatched         = 0x00060082,
        EventSlaveReceiverSecondAddressMatched      = 0x00820000,
        EventSlaveTransmitterSecondAddressMatched   = 0x00860080,
        EventSlaveGenerallCallAddressMatched        = 0x00120000,
        EventSlaveByteReceived                      = 0x00020040,
        EventSlaveStopDetected                      = 0x00000010,
        EventSlaveByteTransmitted                   = 0x00060084,
        EventSlaveByteTransmitting                  = 0x00060080,
        EventSlaveAckFailure                        = 0x00000400
    } Event;

    bool checkEvent(Event e);
};

#endif
