#include "cc1200.h"

/* Address Config = No address check */
/* Bit Rate = 1000 */
/* Carrier Frequency = 868.900024 */
/* Deviation = 399.169922 */
/* Device Address = 0 */
/* Manchester Enable = false */
/* Modulation Format = 4-GFSK */
/* Packet Bit Length = 0 */
/* Packet Length = 255 */
/* Packet Length Mode = Variable */
/* RX Filter BW = 1666.666667 */
/* Symbol rate = 500 */
/* Whitening = false */

CC1200::RF_config_param CC1200::RF_config[] =
{
    {0x0001, 0x06},
    {0x0008, 0xA8},
    {0x0009, 0x13},
    {0x000A, 0x99},
    {0x000B, 0x05},
    {0x000C, 0x26},
    {0x000E, 0x8A},
    {0x000F, 0x00},
    {0x0010, 0x02},
    {0x0011, 0x42},
    {0x0012, 0x05},
    {0x0013, 0xC9},
    {0x0014, 0x99},
    {0x0015, 0x99},
    {0x0016, 0x2F},
    {0x0017, 0xEC},
    {0x001B, 0x16},
    {0x001C, 0x84},
    {0x001D, 0x00},
    {0x0020, 0x12},
    {0x0026, 0x00},
    {0x0028, 0x20},
    {CC1200_RFEND_CFG1, 0x3F}, // go to RX state after RX
    {CC1200_RFEND_CFG0, 0x30}, // go to RX state after TX    
//    {0x002B, 0x36},
    {0x002E, 0xFF},
    {0x2F00, 0x18},
    {0x2F02, 0x03},
    {0x2F05, 0x00},
    {0x2F0C, 0x56},
    {0x2F0D, 0xE3},
    {0x2F0E, 0xD7},
    {0x2F10, 0xEE},
    {0x2F11, 0x10},
    {0x2F12, 0x04},
    {0x2F13, 0x50},
    {0x2F16, 0x40},
    {0x2F17, 0x0E},
    {0x2F19, 0x03},
    {0x2F1B, 0x33},
    {0x2F1C, 0xF7},
    {0x2F1D, 0x0F},
    {0x2F1F, 0x00},
    {0x2F20, 0x6E},
    {0x2F21, 0x1C},
    {0x2F22, 0xAC},
    {0x2F27, 0xB5},
    {0x2F2F, 0x0D},
    {0x2F32, 0x0E},
    {0x2F36, 0x03}, 
  
//    {0x0001, 0x06},
//    {0x0008, 0xA8},
//    {0x000A, 0x47},
//    {0x000B, 0x2F},
//    {0x000C, 0x1E},
//    {0x000E, 0x8A},
//    {0x000F, 0x00},
//    {0x0010, 0x01},
//    {0x0011, 0x42},
//    {0x0012, 0x05},
//    {0x0013, 0xC9},
//    {0x0014, 0x99},
//    {0x0015, 0x99},
//    {0x0016, 0x2F},
//    {0x0017, 0xF8},
//    {0x001A, 0x60},
//    {0x001B, 0x12},
//    {0x001C, 0x84},
//    {0x001D, 0x00},
//    {0x0020, 0x12},
//    {0x0026, 0x00},
//    {0x0028, 0x20},
////    {CC1200_RFEND_CFG1, 0x30}, // go to RX state after RX
////    {CC1200_RFEND_CFG0, 0x30}, // go to RX state after TX
//    {0x002B, 0x61},
//    {0x002E, 0xFF},
//    {0x2F01, 0x23},
//    {0x2F05, 0x00},
//    {0x2F0C, 0x56},
//    {0x2F0D, 0xE3},
//    {0x2F0E, 0xD7},
//    {0x2F10, 0xEE},
//    {0x2F11, 0x10},
//    {0x2F12, 0x04},
//    {0x2F13, 0xA3},
//    {0x2F16, 0x40},
//    {0x2F17, 0x0E},
//    {0x2F19, 0x03},
//    {0x2F1B, 0x33},
//    {0x2F1C, 0xF7},
//    {0x2F1D, 0x0F},
//    {0x2F1F, 0x00},
//    {0x2F20, 0x6E},
//    {0x2F21, 0x1C},
//    {0x2F22, 0xAC},
//    {0x2F27, 0xB5},
//    {0x2F2F, 0x0D},
//    {0x2F32, 0x0E},
//    {0x2F36, 0x03},
};
//---------------------------------------------------------------------------

CC1200::CC1200(Spi *spi, Gpio::PinName csPin, Gpio::PinName resetPin) :
    mSpi(spi),
    pinCS(0L), pinReset(0L),
    pinGpio0(0L), pinGpio2(0L), pinGpio3(0L)
{
    pinCS = new Gpio(csPin);
    pinCS->setAsOutput();
    pinCS->set();
    pinReset = new Gpio(resetPin);
    pinReset->setAsOutput();
    // perform hard reset
    pinReset->reset();
    for (int i=0; i<10000; i++);
    pinReset->set();
    for (int i=0; i<10000; i++);
        
    Spi::Config conf;
    conf.CPHA = 0;
    conf.CPOL = 0;
    conf.master = 1;
    conf.baudrate = 4; // clock / 32
    conf.enable = 0;
    conf.LSBfirst = 0;
    conf.RXonly = 0;
    conf.frame16bit = 0;
    mSpi->setConfig(conf);
    mSpi->open();
    
    uploadConfig(RF_config, sizeof(RF_config)/sizeof(RF_config[0]));
}

void CC1200::setGpioPins(Gpio::PinName gpio0, Gpio::PinName gpio2, Gpio::PinName gpio3)
{
    delete pinGpio0, pinGpio2, pinGpio3;
    pinGpio0 = new Gpio(gpio0);
    pinGpio2 = new Gpio(gpio2);
    pinGpio3 = new Gpio(gpio3);
}

void CC1200::select()
{
    pinCS->reset();
    for (int w=0; w<100; w++);
}

void CC1200::deselect()
{
    pinCS->set();
    for (int w=0; w<100; w++);
}
//---------------------------------------------------------------------------

unsigned long CC1200::getSyncWord()
{
    unsigned char buf[5] = {(0x80 | 0x40 | 0x04), 0x00, 0x00, 0x00, 0x00};
    select();
    mSpi->transfer(buf, 5);
    deselect();
    unsigned long result = (buf[1]<<24) | (buf[2]<<16) | (buf[3]<<8) | buf[4];
    return result;
}
//---------------------------------------------------------------------------

void CC1200::sendCommand(Command cmd)
{
    select();
    mStatus = mSpi->transferWord((unsigned char)cmd);
    deselect();
}
//---------------------------------------------------------------------------

void CC1200::writeReg(unsigned short addr, unsigned char value)
{
    select();
    if (addr < 0x2F)
    {
        mSpi->transferWord(addr);
    }
    else
    {
        mSpi->transferWord(0x2F);
        mSpi->transferWord(addr & 0xFF);
    }
    mStatus = mSpi->transferWord(value);
    deselect();
}

unsigned char CC1200::readReg(unsigned short addr)
{
    unsigned char result;
    select();
    if (addr < 0x2F)
    {
        mStatus = mSpi->transferWord(0x80 | addr);
    }
    else
    {
        mStatus = mSpi->transferWord(0x80 | 0x2F);
        mSpi->transferWord(addr & 0xFF);
    }
    result = mSpi->transferWord(0x00);
    deselect();
    return result;
}
//---------------------------------------------------------------------------

void CC1200::uploadConfig(RF_config_param *data, int count)
{
    for (int i=0; i<count; i++)
        writeReg(data[i].address, data[i].value);
}
//---------------------------------------------------------------------------

void CC1200::write(const unsigned char *data, unsigned char size)
{
    select();
    mStatus = mSpi->transferWord(0x40 | 0x3F);
    // if (variable_length_mode)
    mSpi->transferWord(size);
    for (int i=0; i<size; i++)
        mSpi->transferWord(data[i]);
    deselect();
}

void CC1200::read(unsigned char *data, unsigned char size)
{  
    select();
    mStatus = mSpi->transferWord(0x80 | 0x40 | 0x3F);
    for (int i=0; i<size; i++)
        data[i] = mSpi->transferWord(0x00);
    deselect();
}
//---------------------------------------------------------------------------

void CC1200::startTx()
{
    sendCommand(STX);
}

void CC1200::startRx()
{
    sendCommand(SRX);
}
//---------------------------------------------------------------------------

void CC1200::send(const unsigned char *data, unsigned char size)
{
    sendCommand(SFSTXON); // enable frequency synthesizer
    write(data, size);
    sendCommand(STX);
}
//---------------------------------------------------------------------------

unsigned char CC1200::getRxSize()
{
    return readReg(CC1200_NUM_RXBYTES);
}

unsigned char CC1200::getStatus()
{
    sendCommand(SNOP);
    return mStatus;
}
//---------------------------------------------------------------------------

void CC1200::setAddress(unsigned char addr)
{
    writeReg(CC1200_DEV_ADDR, addr);
    writeReg(CC1200_PKT_CFG1, 0x13); // address check, 0x00 broadcast; CRC enabled, initialized to 0xFFFF, append_status enabled
}
//---------------------------------------------------------------------------
