#include "adc303.h"

Adc* Adc::mInstances[3] = {0L, 0L, 0L};

Adc::Adc(int adcBase) :
//    mAdc2(0L), mAdc3(0L),
    mMode(ModeSingle),
    mEnabled(false),
    mResolution(Res16bit),
    mChannelCount(0),
    mSampleCount(1),
    mDma(0L),
    mDmaOwner(false)
{
    for (int i=0; i<sizeof(mChannelResultMap); i++)
        mChannelResultMap[i] = -1;

    mInstances[adcBase - 1] = this;
    
      switch (adcBase)
    {
      case 1:
        mAdc = ADC1;

        RCC->AHBENR |= RCC_AHBENR_ADC12EN;
        mDmaChannel = Dma::Channel1_ADC1; // Dma::ADC1_Stream0
        break;
#if defined(ADC2)
      case 2:
        mAdc = ADC2;
        RCC->AHBENR |= RCC_AHBENR_ADC12EN;
#if defined(STM32F303xC)
        mDmaChannel = Dma::Channel1_2_ADC2 ; // Dma::ADC2_Stream3
#else
        mDmaChannel = Dma::Channel4_ADC2 ;
#endif        
        break;     
#endif
      default:
        return;
    }

    // ADC Common Init    
    RCC->CFGR2|=RCC_CFGR2_ADCPRE12_DIV2;
    mAdc->CR |= ADC_CR_ADVREGEN;
  mAdc->CR |= ADC_CR_ADCAL;  
  while (mAdc->CR & ADC_CR_ADCAL);

    // Scan conversion mode is enabled
    mAdc->CFGR = (mResolution & ADC_CFGR_RES_Msk);
    mAdc->CFGR = (mResolution & ADC_CFGR_ALIGN_Msk);    

}

Adc::~Adc()
{
    if (mDma && mDmaOwner)
        delete mDma;
}

Adc *Adc::instance(int periphNumber)
{
    if (periphNumber < 1 || periphNumber > 3)
        return 0L;
    if (!mInstances[periphNumber-1])
        new Adc(periphNumber);
    return mInstances[periphNumber-1];
}
//---------------------------------------------------------------------------

void Adc::setResolution(Resolution resolution)
{
    mResolution = resolution;
    MODIFY_REG(mAdc->CFGR, ADC_CFGR_RES_Msk, (mResolution & ADC_CFGR_RES_Msk));
    MODIFY_REG(mAdc->CFGR, ADC_CFGR_ALIGN_Msk, (mResolution & ADC_CFGR_ALIGN_Msk));
}

void Adc::selectTrigger(Trigger trigger, Edge edge)
{
    MODIFY_REG(mAdc->CFGR, ADC_CFGR_EXTEN_Msk | ADC_CFGR_EXTSEL_Msk, (uint32_t)trigger | (uint32_t)edge);
}
//---------------------------------------------------------------------------

void Adc::addChannel(Channel channel, SampleTime sampleTime)
{
    if (channel == TempSensor)      
        ADC12_COMMON->CCR |= ADC12_CCR_TSEN;
    else if( channel == VrefInt)
        ADC12_COMMON->CCR |= ADC12_CCR_VREFEN;
    else if (channel == Vbat)
        ADC12_COMMON->CCR |= ADC12_CCR_VBATEN;

    if (mEnabled)
        THROW(Exception::ResourceBusy);

    MODIFY_REG(mAdc->SQR1, ADC_SQR1_L_Msk, mChannelCount << ADC_SQR1_L_Pos);
    mChannelCount++;
    regularChannelConfig(channel, mChannelCount, sampleTime);
    mBuffer.resize(mChannelCount*2*mSampleCount);
    mChannelResultMap[channel] = mChannelCount - 1;
   
}

Adc::Channel Adc::addChannel(Gpio::Config pin, SampleTime sampleTime)
{
    int periphNumber = GpioConfigGetPeriphNumber(pin) & 0x7;
    Channel channel = (Channel)GpioConfigGetPeriphChannel(pin);

//    // for STM32G4:
//    if (GpioConfigGetPeriphNumber(pin) & 0x80)
//        channel += 16;
    if (mInstances[periphNumber - 1] != this)
        THROW(Exception::InvalidPeriph);

    Gpio::config(pin);

    addChannel(channel, sampleTime);

    return channel;
}

void Adc::regularChannelConfig(Channel channel, uint8_t rank, SampleTime sampleTime)
{
    __IO uint32_t *SMPR = &mAdc->SMPR2;
    __IO uint32_t *SQR = 0L;
    int smpr_pos = 0;
    int sqr_pos = 0;

    if (channel > Channel9)
    {
        SMPR = &mAdc->SMPR2;
        smpr_pos = 3 * (channel - 10);
    }
    else
    {
        SMPR = &mAdc->SMPR1;
        smpr_pos = 3 * channel;
    }

    if (rank < 5)
    {
        SQR = &mAdc->SQR1;
        sqr_pos = (6 * (rank));
    }
    else if (rank < 10)
    {
        SQR = &mAdc->SQR2;
        sqr_pos = 6 * (rank - 5);
    }
    else
    {
        SQR = &mAdc->SQR3;
        sqr_pos = 6 * (rank - 10);
    }

    if (SMPR && SQR)
    {
        MODIFY_REG(*SMPR, 0x07 << smpr_pos, sampleTime << smpr_pos);
        MODIFY_REG(*SQR, 0x1f << sqr_pos, channel << sqr_pos);
    }
}
//---------------------------------------------------------------------------

void Adc::setMultisample(int sampleCount)
{
    mSampleCount = sampleCount;
    mBuffer.resize(mChannelCount*2*mSampleCount);
}
//---------------------------------------------------------------------------

int Adc::maxValue() const
{
    switch (mResolution)
    {
        case Res6bit:  return (1<<6) - 1;
        case Res8bit:  return (1<<8) - 1;
        case Res10bit: return (1<<10) - 1;
        case Res12bit: return (1<<12) - 1;
        case Res16bit: return (1<<16) - 1;
        default: return 0;
    }
}

void Adc::setEnabled(bool enable)
{
    if (!mDma && enable)
    {
        mDma = new Dma(mDmaChannel);
        mDma->setCircularBuffer(mBuffer.data(), mChannelCount*mSampleCount);
        if (mCompleteEvent)
            mDma->setTransferCompleteEvent(mCompleteEvent);
        configDma(mDma);
        mDmaOwner = true;
    }

    mEnabled = enable;
    if (enable)
        mAdc->CR |= ADC_CR_ADEN;
    else
        mAdc->CR &= ~ADC_CR_ADEN;

//    if (mAdc2)
//        ADC_Cmd(mAdc2, en);
//    if (mAdc3)
//        ADC_Cmd(mAdc3, en);

    if (mDmaOwner)
        mDma->setEnabled(enable);
}
//---------------------------------------------------------------------------

void Adc::configDma(Dma *dma)
{
    void *address = (unsigned char*)(mMode==ModeSingle? &mAdc->DR: &ADC12_COMMON->CDR);
    int dataSize = mResolution==Res8bit? 1: 2;

    dma->setSource(address, dataSize);
    if (mDma && mDmaOwner)
    {
        mDmaOwner = false;
        delete mDma;
    }
    mDma = dma;

    // Enable the selected ADC DMA request after last transfer
    if (mMode == ModeSingle)
        mAdc->CFGR |= ADC_CFGR_DMACFG;

    mAdc->CFGR |= ADC_CFGR_DMAEN;
}
//---------------------------------------------------------------------------

void Adc::startConversion()
{
    mAdc->CR |= ADC_CR_ADSTART;
//    if (mAdc2)
//        mAdc2->CR2 |= ADC_CR2_SWSTART;
//    if (mAdc3)
//        mAdc3->CR2 |= ADC_CR2_SWSTART;
}

bool Adc::isComplete() const
{
    if (mDma)
        return mDma->isComplete();
    return mAdc->ISR & ADC_ISR_EOC;
}
//---------------------------------------------------------------------------

void Adc::setContinuousMode(bool enabled)
{
    if (enabled)
        mAdc->CFGR |= ADC_CFGR_CONT;
    else
        mAdc->CFGR &= ~ADC_CFGR_CONT;
}
//---------------------------------------------------------------------------

int Adc::result(unsigned char channel)
{
    return resultByIndex(mChannelResultMap[channel]);
}

int Adc::resultByIndex(unsigned char index)
{
    unsigned short *buf = reinterpret_cast<unsigned short*>(mBuffer.data());
    if (index < mChannelCount)
    {
        if (mSampleCount == 1)
            return buf[index];
        else
        {
            int sum = 0;
            for (int i=0; i<mSampleCount; i++)
                sum += buf[i*mChannelCount + index];
            return (sum / mSampleCount);
        }
    }
    return -1;
}

float Adc::averageByIndex(uint8_t index)
{
    unsigned short *buf = reinterpret_cast<unsigned short*>(mBuffer.data());
    if (index < mChannelCount)
    {
        if (mSampleCount == 1)
            return buf[index];
        else
        {
            float sum = 0;
            for (int i=0; i<mSampleCount; i++)
                sum += buf[i*mChannelCount + index];
            return (sum / mSampleCount);
        }
    }
    return 0.NaN;
}

int Adc::lastResultByIndex(unsigned char index)
{
    const uint16_t *buf = samples();
    int sample_idx = sampleCount() - mDma->dataCounter();
    return buf[sample_idx];
}

const unsigned short &Adc::buffer(unsigned char channel) const
{
    return bufferByIndex(mChannelResultMap[channel]);
}

const unsigned short &Adc::bufferByIndex(unsigned char index) const
{
    return reinterpret_cast<const unsigned short*>(mBuffer.data())[index];
}
//---------------------------------------------------------------------------

void Adc::setCompleteEvent(NotifyEvent e)
{
    mCompleteEvent = e;
    if (mDma)
        mDma->setTransferCompleteEvent(mCompleteEvent);
}