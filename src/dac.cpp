#include "dac.h"

Dac::Dac(Channels channels, Resolution resolution) :
    mChannels(channels),
    mEnabled(true)
{
    mResolution = resolution;

    // DAC Periph clock enable
#if !defined(STM32F37X)
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
#else
    RCC->APB1ENR |= RCC_APB1ENR_DAC1EN;
#endif
    
    if (mChannels & Channel1)
        Gpio::config(Gpio::DAC_OUT1_PA4);
    if (mChannels & Channel2)
        Gpio::config(Gpio::DAC_OUT2_PA5);
        
    uint8_t idx = (uint8_t)mResolution | mChannels;
    switch (idx)
    {
        case (uint8_t)Channel1    | Res8bit:  DR = &DAC->DHR8R1; break;
        case (uint8_t)Channel1    | Res12bit: DR = &DAC->DHR12R1; break;
        case (uint8_t)Channel1    | Res16bit: DR = &DAC->DHR12L1; break;
        case (uint8_t)Channel2    | Res8bit:  DR = &DAC->DHR8R2; break;
        case (uint8_t)Channel2    | Res12bit: DR = &DAC->DHR12R2; break;
        case (uint8_t)Channel2    | Res16bit: DR = &DAC->DHR12L2; break;
        case (uint8_t)ChannelBoth | Res8bit:  DR = &DAC->DHR8RD; break;
        case (uint8_t)ChannelBoth | Res12bit: DR = &DAC->DHR12RD; break;
        case (uint8_t)ChannelBoth | Res16bit: DR = &DAC->DHR12LD; break;
    }
    
    
#warning TODO: maybe implement DAC start/stop functions?
    setEnabled(true);
}

Dac::~Dac()
{
    setEnabled(false);
//    if (mChannels & Channel1)
//        Gpio::release(Gpio::DAC_OUT1_PA4);
//    if (mChannels & Channel2)
//        Gpio::release(Gpio::DAC_OUT1_PA5);
}
//---------------------------------------------------------------------------

void Dac::selectTrigger(Trigger trigger)
{
    if (mChannels & Channel1)
        MODIFY_REG(DAC->CR, (DAC_CR_TSEL1_Msk | DAC_CR_TEN1_Msk), trigger);
    if (mChannels & Channel2)
        MODIFY_REG(DAC->CR, (DAC_CR_TSEL2_Msk | DAC_CR_TEN2_Msk), trigger << 16);
}

void Dac::setValue(unsigned short value1, unsigned short value2)
{
    switch (mChannels)
    {
      case Channel1:
        *DR = value1;
        break;
      case Channel2:
        *DR = value2;
        break;
      case ChannelBoth:
        if (mResolution == Res8bit)
            *DR = (value1 & 0xFF) | ((value2 & 0xFF) << 8);
        else
            *DR = value1 | (value2 << 16);
        break;
    };
}

void Dac::setEnabled(bool enabled)
{
    uint32_t cr = 0;
    if (mChannels & Channel1)
        cr |= DAC_CR_EN1;
    if (mChannels & Channel2)
        cr |= DAC_CR_EN2;
    
    mEnabled = enabled;
    if (enabled)
        DAC->CR |= cr;
    else
        DAC->CR &= ~cr;
}
//---------------------------------------------------------------------------

void Dac::configDma(Dma *dma)
{
    void *address = (void *)DR;  
    int dataSize = mResolution==Res8bit? 1: 2;
    if (mChannels == ChannelBoth)
        dataSize *= 2;
       
    dma->setSink(address, dataSize);
    
    uint32_t cr = 0;
    if (mChannels & Channel1)
        cr |= DAC_CR_DMAEN1;
    if (mChannels & Channel2)
        cr |= DAC_CR_DMAEN2;
    DAC->CR |= cr;
}
//---------------------------------------------------------------------------
