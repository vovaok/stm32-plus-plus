#include "dac37x.h"

Dac::Dac(Gpio::Config out1, Gpio::Config out2) :
    mResolution(Res12bit),
    mEnabled(false)
{
    switch (out1)
    {
    case Gpio::DAC1_OUT1_PA4:
        mDac = DAC1;
        mChannels = Channel1;
        break;
        
    case Gpio::DAC1_OUT2_PA5:
        mDac = DAC1;
        mChannels = Channel2;
        break;
        
    case Gpio::DAC2_OUT1_PA6:
        mDac = DAC2;
        mChannels = Channel1;
        break;
    }
    
    if (out2 == out1)
        throw Exception::resourceBusy;
      
    switch (out2)
    {
    case Gpio::DAC1_OUT1_PA4:
        if (mDac != DAC1)
            throw Exception::invalidPeriph;
        mChannels = ChannelBoth;
        break;
        
    case Gpio::DAC1_OUT2_PA5:
        if (mDac != DAC1)
            throw Exception::invalidPeriph;
        mChannels = ChannelBoth;
        break;
        
    case Gpio::DAC2_OUT1_PA6:
        throw Exception::invalidPeriph;
        break;
    }
    
    if (mDac == DAC1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC1, ENABLE);
    else
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC2, ENABLE);
    
    Gpio::config(out1);
    Gpio::config(out2);
    
    setResolution(mResolution);
    
    mDac->CR |= DAC_CR_BOFF1;
    mDac->CR |= DAC_CR_BOFF2;
}

Dac::~Dac()
{
  
}
//---------------------------------------------------------------------------

void Dac::setEnabled(bool enabled)
{
    mEnabled = enabled;
    unsigned long mask = 0;
    
    if (mChannels & Channel1)
        mask |= DAC_CR_EN1;
    if (mChannels & Channel2)
        mask |= DAC_CR_EN2;
    
    if (enabled)
        mDac->CR |= mask;
    else
        mDac->CR &= ~mask;
}

void Dac::setResolution(Resolution resolution)
{
    mResolution = resolution;
    switch (mResolution)
    {
    case Res8bit: 
        switch (mChannels)
        {
            case Channel1: mData = &mDac->DHR8R1; break;
            case Channel2: mData = &mDac->DHR8R2; break;
            case ChannelBoth: mData = &mDac->DHR8RD; break;
        }
        break;
    case Res12bit: 
        switch (mChannels)
        {
            case Channel1: mData = &mDac->DHR12R1; break;
            case Channel2: mData = &mDac->DHR12R2; break;
            case ChannelBoth: mData = &mDac->DHR12RD; break;
        }
        break;
    case Res16bit: 
        switch (mChannels)
        {
            case Channel1: mData = &mDac->DHR12L1; break;
            case Channel2: mData = &mDac->DHR12L2; break;
            case ChannelBoth: mData = &mDac->DHR12LD; break;
        }
        break;
    }
}

void Dac::setValue(unsigned short value1, unsigned short value2)
{
    unsigned long val;
    if (mResolution == Res8bit)
        val = value1 | (value2 << 8);
    else
        val = value1 | ((unsigned long)value2 << 16);
    *mData = val;
}
//---------------------------------------------------------------------------