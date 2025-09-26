#include "ads1220.h"
#include "core/application.h"

Ads1220::Ads1220(Spi *spi, Gpio::PinName csPin) :
    mSpi(spi),
    mResetPin(0L),
    mDrdyPin(0L),
    mEnabled(false)
{
    mTempChannel.offset = 0;
    mTempChannel.factor = 1.0f / 32.0f;

    mCsPin = new Gpio(csPin);
    mCsPin->setAsOutput();

    deselect();
//    Spi::Config conf;
//    conf.CPHA = 1;
//    conf.CPOL = 0;
//    conf.master = 1;
//    conf.baudrate = 5;
//    mSpi->setConfig(conf);
    mSpi->setMasterMode();
    mSpi->setCPOL_CPHA(0, 1);
    mSpi->setBaudratePrescaler(5);
    mSpi->setDataSize(8);
    mSpi->open();

    mConf.word = 0;
    mConf.PGA_BYPASS = Ads1220_PGA_Enabled;
    mConf.GAIN   = Ads1220_Gain_128;
    mConf.MUX    = Ads1220_MUX_AIN0_AIN3;
    mConf.BCS    = Ads1220_BCS_Off;
    mConf.TS     = Ads1220_TS_Disabled;
    mConf.CM     = Ads1220_CM_SingleShot;
    mConf.MODE   = Ads1220_MODE_Turbo;
    mConf.DR     = Ads1220_DR_175_SPS; // x2 in TURBO mode
    mConf.IDAC   = Ads1220_IDAC_Off;
    mConf.PSW    = Ads1220_PSW_AlwaysOpen;
    mConf.FILTER = Ads1220_Filter_None;
    mConf.VREF   = Ads1220_VREF_AVDD_AVSS;
    mConf.DRDYM  = Ads1220_DRDYM_DRDY;
    mConf.I2MUX  = Ads1220_IDAC_Disabled;
    mConf.I1MUX  = Ads1220_IDAC_Disabled;

    //    writeConfig(mConf);
    for (int i=0; i<4; i++)
    {
        writeReg(i, mConf.reg[i]);
        unsigned char d = readReg(i);
        if (d != mConf.reg[i])
        {
//            throw Exception::badSoBad;
        }
    }

    stmApp()->registerTaskEvent(EVENT(&Ads1220::task));
}

void Ads1220::setDrdyPin(Gpio::PinName pin)
{
    mDrdyPin = new Gpio(pin);
    mDrdyPin->setAsInput();
}

void Ads1220::start()
{
    //mConf.CM = Ads1220_CM_Continuous;
    for (int i=0; i<4; i++)
        writeReg(i, mConf.reg[i]);

    sendOpcode(ADS1220_START);
    mEnabled = true;
}

void Ads1220::stop()
{
    sendOpcode(ADS1220_POWERDOWN);
//    mConf.CM = Ads1220_CM_SingleShot;
//    writeReg(ADS1220_CONFIG1, mConf.reg[1]);
    mEnabled = false;
}

void Ads1220::start(unsigned char ch)
{
    if (mConf.TS)
    {
        mConf.TS = 0;
        writeReg(ADS1220_CONFIG1, mConf.reg[1]);
    }
    mConf.MUX = ch;
    writeReg(ADS1220_CONFIG0, mConf.reg[0]);
}

void Ads1220::measureTemp()
{
    mConf.TS = 1;
    writeReg(ADS1220_CONFIG1, mConf.reg[1]);
    if (isDataReady())
    {
        //while(1); //wut?
    }
}

void Ads1220::task()
{
    if (mEnabled)
    {
        if (isDataReady())
        {
            int value = readValue();

            Channel *ch;
            if (mConf.TS)
            {
                ch = &mTempChannel;
                ch->rawValue = value;
                ch->value = value / (float)(1 << 15);
            }
            else
            {
                ch = &mChannels[mConf.MUX];
                float gain = 1 << mConf.GAIN;
                float vref = 2.048f; // internal reference
                if (mConf.VREF != Ads1220_VREF_Internal)
                    vref = ch->factor;
                ch->rawValue = value;
                ch->value = (float)(value - ch->offset) * vref / gain / (1 << 23);
            }
        }
    }
}

void Ads1220::select()
{
    mCsPin->reset();
    for (int i=0; i<100; i++);
}

void Ads1220::deselect()
{
    for (int i=0; i<100; i++);
    mCsPin->set();
    for (int i=0; i<100; i++);
}

void Ads1220::sendOpcode(Ads1220_opcode cmd)
{
    select();
    mSpi->write(cmd);
    deselect();
}

long Ads1220::readValue()
{
    long value = 0;
    select();
    mSpi->write(ADS1220_RDATA);
    for (unsigned char i=0; i<3; i++)
        ((unsigned char*)&value)[2-i] = mSpi->read();
    deselect();
    if (value & 0x00800000)
        value |= 0xFF000000;
    return value;
}

unsigned char Ads1220::readReg(unsigned char address)
{
    select();
    mSpi->write(ADS1220_RREG | ((address & 0x3) << 2));
    unsigned char result = mSpi->write(0xFF);
    deselect();
    return result;
}

void Ads1220::writeReg(unsigned char address, unsigned char data)
{
    select();
    mSpi->write(ADS1220_WREG | ((address & 0x3) << 2));
    mSpi->write(data);
    deselect();
}

void Ads1220::writeConfig(const Ads1220_conf &conf)
{
    select();
    mSpi->write(ADS1220_WREG | 0x03); // write 4 bytes from address 0
    for (unsigned char i=0; i<4; i++)
        mSpi->write(conf.reg[i]);
    deselect();
}

bool Ads1220::isDataReady()
{
    if (!mDrdyPin)
        return true;
    return !mDrdyPin->read();
}

void Ads1220::setConfig(const Ads1220_conf &conf)
{
    for (int i=0; i<4; i++)
    {
        if (conf.reg[i] != mConf.reg[i])
        {
            mConf.reg[i] = conf.reg[i];
            writeReg(i, mConf.reg[i]);
        }
    }
}
