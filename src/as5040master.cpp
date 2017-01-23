#include "as5040master.h"

As5040Master::As5040Master(Spi *spi) :
    mSpi(spi),
    mCurChannel(0)
{
    Spi::Config conf;
    conf.CPHA = 1;
    conf.CPOL = 0;
    conf.master = 1;
    conf.baudrate = 6; // clock / 128
//    conf.LSBfirst = 0;
//    conf.RXonly = 1;
    conf.frame16bit = 1;
    mSpi->setConfig(conf);
    mSpi->setTransferCompleteEvent(EVENT(&As5040Master::onRead));  
    mSpi->open();
}
//---------------------------------------------------------------------------

unsigned char As5040Master::addChannel(Gpio::PinName csPin, float zeroDeg)
{
    Channel ch;
    ch.cs = new Gpio(csPin);
    ch.cs->setAsOutput();
    ch.cs->write(1);
    ch.raw = 0;
    ch.value = 0;
    ch.zero = lrintf(zeroDeg * (65536.0f / 360.0f));
    mChannels.push_back(ch);
    return mChannels.size() - 1;
}

void As5040Master::start()
{
    if (mChannels.empty())
        return;
      
    mCurChannel = 0;
    mChannels[0].cs->write(0);
    mSpi->transferWordAsync();
}
//---------------------------------------------------------------------------

void As5040Master::onRead(unsigned short rawValue)
{
    Channel &ch = mChannels[mCurChannel];
    ch.cs->write(1);
    ch.raw = rawValue & 0xFFC0; // last 6 bits are flags
    ch.value = ((unsigned long)(ch.raw - ch.zero) & 0xFFFF) * (360.0f / 65536.0f);
    ch.flags.word = rawValue & 0x3f;
    
    mCurChannel++;
    if (mCurChannel < mChannels.size())
    {
        mChannels[mCurChannel].cs->write(0);
        mSpi->transferWordAsync();
    }
    else if (onFinish)
    {
        onFinish();
    }
}
//---------------------------------------------------------------------------

void As5040Master::setZero(unsigned char channel)
{
    if (channel < mChannels.size())
        mChannels[channel].zero = mChannels[channel].raw;
}

void As5040Master::setZero(unsigned char channel, float zeroDeg)
{
    if (channel < mChannels.size())
        mChannels[channel].zero = lrintf(zeroDeg * (65536.0f / 360.0f));
}

float As5040Master::zeroDeg(unsigned char channel) const
{
    if (channel < mChannels.size())
        return mChannels[channel].zero * 360.f / 65536.0f;
    else
        return NAN;
}

bool As5040Master::isValid(unsigned char channel)
{
    if (channel < mChannels.size())
    {
        Flags flags = mChannels[channel].flags;
        return flags.OCF && !(flags.word & 0x001E);
    }
    return false;
}

float As5040Master::valueDeg(unsigned char channel)
{
    if (channel < mChannels.size())
        return mChannels[channel].value;
    return NAN;
}

float As5040Master::valueRad(unsigned char channel)
{
    if (channel < mChannels.size())
        return mChannels[channel].value * (3.1415926f / 180.0f);
    return NAN;
}

As5040Master::Flags As5040Master::flags(unsigned char channel)
{
    if (channel < mChannels.size())
        return mChannels[channel].flags;
    Flags dummy;
    dummy.word = 0;
    return dummy;
}
//---------------------------------------------------------------------------