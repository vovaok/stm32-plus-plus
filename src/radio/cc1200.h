#ifndef _CC1200_H
#define _CC1200_H

#include "gpio.h"
#include "spi.h"
#include "cc1200_const.h"

class CC1200
{
public:
    typedef struct
    {
        unsigned short address;
        unsigned char value;
    } RF_config_param;
    
    typedef enum
    {
        SRES    = 0x30,
        SFSTXON = 0x31,
        SXOFF   = 0x32,
        SCAL    = 0x33,
        SRX     = 0x34,
        STX     = 0x35,
        SIDLE   = 0x36,
        SAFC    = 0x37,
        SWOR    = 0x38,
        SPWD    = 0x39,
        SFRX    = 0x3A,
        SFTX    = 0x3B,
        SWORRST = 0x3C,
        SNOP    = 0x3D
    } Command;
    
    typedef enum
    {
        IDLE            = 0x00,
        RX              = 0x10,
        TX              = 0x20,
        FSTXON          = 0x30,
        CALIBRATE       = 0x40,
        SETTLING        = 0x50,
        RX_FIFO_ERROR   = 0x60,
        TX_FIFO_ERROR   = 0x70,
    } Status;
    
    typedef struct
    {
        unsigned char size;
        unsigned char address;
    } PayloadHeader;
    
    typedef struct
    {
        signed char RSSI;
        unsigned char LQI: 7;
        unsigned char CRC_OK: 1;
    } AppendedStatus;
    
private:
    Spi *mSpi;
    Gpio *pinCS, *pinReset;
    Gpio *pinGpio0, *pinGpio2, *pinGpio3;
    
    static RF_config_param RF_config[];
  
    unsigned char mStatus;
    bool mBusy;
    unsigned long mIrqState;
    
    void select();
    void deselect();
  
public:
    CC1200(Spi *spi, Gpio::PinName csPin, Gpio::PinName resetPin);
    void setGpioPins(Gpio::PinName gpio0, Gpio::PinName gpio2, Gpio::PinName gpio3);
    
    unsigned long getSyncWord();
    
    unsigned char status() const {return mStatus;}
    unsigned char getStatus();
    
    void sendCommand(Command cmd);
    
    void writeReg(unsigned short addr, unsigned char value);
    unsigned char readReg(unsigned short addr);
    void uploadConfig(RF_config_param *data, int count);
    
    void write(const unsigned char *data, unsigned char size);
    void read(unsigned char *data, unsigned char size);
    
    void startTx();
    void startRx();
    
    void send(const unsigned char *data, unsigned char size);
    unsigned char getRxSize();
    
    void setAddress(unsigned char addr);
    
//    bool getRxTxFlag();
    void setRxTxEvent(const NotifyEvent &e);
};

#endif