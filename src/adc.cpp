#include "adc.h"

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
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
        mDmaChannel = Dma::ADC1_Stream4; // Dma::ADC1_Stream0
        break;
        
      case 2:
        mAdc = ADC2;
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
        mDmaChannel = Dma::ADC2_Stream2; // Dma::ADC2_Stream3
        break;
        
      case 3:
        mAdc = ADC3;
        RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
        mDmaChannel = Dma::ADC3_Stream1; // Dma::ADC3_Stream0
        break;
        
      default:
        return;
    }
    
    // ADC Common Init  
    ADC->CCR = (ADC_CCR_ADCPRE_1); // ADC_Prescaler_Div6, independent mode
    
    // Scan conversion mode is enabled
    mAdc->CR1 = (mResolution & ADC_CR1_RES_Msk) | ADC_CR1_SCAN;
    mAdc->CR2 = (mResolution & ADC_CR2_ALIGN_Msk);
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
    MODIFY_REG(mAdc->CR1, ADC_CR1_RES_Msk, (mResolution & ADC_CR1_RES_Msk));
    MODIFY_REG(mAdc->CR2, ADC_CR2_ALIGN_Msk, (mResolution & ADC_CR2_ALIGN_Msk));
}

void Adc::selectTrigger(Trigger trigger, Edge edge)
{
    MODIFY_REG(mAdc->CR2, ADC_CR2_EXTEN_Msk | ADC_CR2_EXTSEL_Msk, (uint32_t)trigger | (uint32_t)edge);
}
//---------------------------------------------------------------------------
    
void Adc::addChannel(Channel channel, SampleTime sampleTime)
{
    if (channel == TempSensor || channel == VrefInt)
        ADC->CCR |= ADC_CCR_TSVREFE;
    else if (channel == Vbat)
        ADC->CCR |= ADC_CCR_VBATE;
  
    if (mEnabled)
        THROW(Exception::ResourceBusy);
    
    MODIFY_REG(mAdc->SQR1, ADC_SQR1_L_Msk, mChannelCount << ADC_SQR1_L_Pos);
    mChannelCount++;
    regularChannelConfig(channel, mChannelCount, sampleTime);
    mBuffer.resize(mChannelCount*2*mSampleCount);
    mChannelResultMap[channel] = mChannelCount - 1;
    
    mAdc->CR2 |= ADC_CR2_EOCS; // end of conversion flag is set on sequence complete
}

Adc::Channel Adc::addChannel(Gpio::Config pin, SampleTime sampleTime)
{
    int periphNumber = GpioConfigGetPeriphNumber(pin);
    Channel channel = (Channel)GpioConfigGetPeriphChannel(pin);
    
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
        SMPR = &mAdc->SMPR1;
        smpr_pos = 3 * (channel - 10);
    }
    else
    {
        SMPR = &mAdc->SMPR2;
        smpr_pos = 3 * channel;
    }
    
    if (rank < 7)
    {
        SQR = &mAdc->SQR3;
        sqr_pos = 5 * (rank - 1);
    }
    else if (rank < 13)
    {
        SQR = &mAdc->SQR2;
        sqr_pos = 5 * (rank - 7);
    }
    else
    {
        SQR = &mAdc->SQR1;
        sqr_pos = 5 * (rank - 13);
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
        mAdc->CR2 |= ADC_CR2_ADON;
    else
        mAdc->CR2 &= ~ADC_CR2_ADON;
      
//    if (mAdc2)
//        ADC_Cmd(mAdc2, en);
//    if (mAdc3)
//        ADC_Cmd(mAdc3, en);
    
    if (mDmaOwner)
        mDma->start();
}
//---------------------------------------------------------------------------

void Adc::configDma(Dma *dma)
{
    void *address = (unsigned char*)(mMode==ModeSingle? &mAdc->DR: &ADC->CDR);
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
        mAdc->CR2 |= ADC_CR2_DDS;
    
    mAdc->CR2 |= ADC_CR2_DMA;
}
//---------------------------------------------------------------------------

void Adc::startConversion()
{
    mAdc->CR2 |= ADC_CR2_SWSTART;
//    if (mAdc2)
//        mAdc2->CR2 |= ADC_CR2_SWSTART;
//    if (mAdc3)
//        mAdc3->CR2 |= ADC_CR2_SWSTART;
}

bool Adc::isComplete() const
{
    if (mDma)
        return mDma->isComplete();
    return mAdc->SR & ADC_SR_EOC;
}
//---------------------------------------------------------------------------

void Adc::setContinuousMode(bool enabled)
{
    if (enabled)
        mAdc->CR2 |= ADC_CR2_CONT;
    else
        mAdc->CR2 &= ~ADC_CR2_CONT;
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