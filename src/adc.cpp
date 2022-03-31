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
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        mDmaChannel = Dma::ChannelAdc1;
        break;
        
      case 2:
        mAdc = ADC2;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
        mDmaChannel = Dma::ChannelAdc2;
        break;
        
      case 3:
        mAdc = ADC3;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
        mDmaChannel = Dma::ChannelAdc3;
        break;
        
      default:
        return;
    }
    
    // ADC Common Init 
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
    ADC_CommonInit(&ADC_CommonInitStructure); 
    
    ADC_StructInit(&mConfig);
    mConfig.ADC_Resolution = ADC_Resolution_12b;
    mConfig.ADC_ScanConvMode = ENABLE;
    mConfig.ADC_ContinuousConvMode = DISABLE;
    mConfig.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
//    mConfig.ADC_ExternalTrigConv = ;
    mConfig.ADC_DataAlign = ADC_DataAlign_Left;
    mConfig.ADC_NbrOfConversion = mChannelCount;
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
    switch (resolution)
    {
      case Res6bit:
        mConfig.ADC_Resolution = ADC_Resolution_6b;
        mConfig.ADC_DataAlign = ADC_DataAlign_Right;
        break;
        
      case Res8bit:
        mConfig.ADC_Resolution = ADC_Resolution_8b;
        mConfig.ADC_DataAlign = ADC_DataAlign_Right;
        break;
        
      case Res10bit:
        mConfig.ADC_Resolution = ADC_Resolution_10b;
        mConfig.ADC_DataAlign = ADC_DataAlign_Right;
        break;
        
      case Res12bit:
        mConfig.ADC_Resolution = ADC_Resolution_12b;
        mConfig.ADC_DataAlign = ADC_DataAlign_Right;
        break;
        
      case Res16bit:
        mConfig.ADC_Resolution = ADC_Resolution_12b;
        mConfig.ADC_DataAlign = ADC_DataAlign_Left;
        break;
    }
    
    ADC_Init(mAdc, &mConfig);
}

void Adc::selectTrigger(Trigger trigger, Edge edge)
{
    uint32_t adcEdge;
    switch (edge)
    {
        case EdgeRising: adcEdge = ADC_ExternalTrigConvEdge_Rising; break;
        case EdgeFalling: adcEdge = ADC_ExternalTrigConvEdge_Falling; break;
        case EdgeBoth: adcEdge = ADC_ExternalTrigConvEdge_RisingFalling; break;
        default: adcEdge = ADC_ExternalTrigConvEdge_None;
    }
    mConfig.ADC_ExternalTrigConvEdge = adcEdge; 
    mConfig.ADC_ExternalTrigConv = trigger;
    ADC_Init(mAdc, &mConfig);
}
//---------------------------------------------------------------------------
    
void Adc::addChannel(int channel, SampleTime sampleTime)
{
    if (channel < 8)
    {
        Gpio::config(Gpio::PinName(Gpio::PA0 + channel), Gpio::modeAnalog);
    }
    else if (channel < 10)
    {
        Gpio::config(Gpio::PinName(Gpio::PB0 + (channel-8)), Gpio::modeAnalog);
    }
    else if (channel < 16)
    {
        Gpio::config(Gpio::PinName(Gpio::PC0 + (channel-10)), Gpio::modeAnalog);
    }
    else if (channel == TempSensor || channel == VrefInt)
    {
        ADC_TempSensorVrefintCmd(ENABLE);
    }
    else if (channel == Vbat)
    {
        ADC_VBATCmd(ENABLE);
    }
  
    if (mEnabled)
        throw Exception::ResourceBusy;
    
    mConfig.ADC_NbrOfConversion = ++mChannelCount;
    ADC_Init(mAdc, &mConfig);
    ADC_RegularChannelConfig(mAdc, channel, mChannelCount, sampleTime);
    mBuffer.resize(mChannelCount*2*mSampleCount);
    mChannelResultMap[channel] = mChannelCount - 1;
    
    mAdc->CR2 |= ADC_CR2_EOCS; // end of conversion flag is set on sequence complete
}

void Adc::addChannel(int channel, Gpio::PinName pin, SampleTime sampleTime)
{
    Gpio::config(pin, Gpio::modeAnalog);
    
    if (mEnabled)
        throw Exception::ResourceBusy;
    
    mConfig.ADC_NbrOfConversion = ++mChannelCount;
    ADC_Init(mAdc, &mConfig);
    ADC_RegularChannelConfig(mAdc, channel, mChannelCount, sampleTime);
    mBuffer.resize(mChannelCount*2*mSampleCount);
    mChannelResultMap[channel] = mChannelCount - 1;
}
//---------------------------------------------------------------------------

void Adc::setMultisample(int sampleCount)
{
    mSampleCount = sampleCount;
    mBuffer.resize(mChannelCount*2*mSampleCount);
}
//---------------------------------------------------------------------------

void Adc::setEnabled(bool enable)
{
    if (!mDma && enable)
    {
        mDma = Dma::getStreamForPeriph(mDmaChannel);
        mDma->setCircularBuffer(mBuffer.data(), mChannelCount*mSampleCount);
        mDma->setTransferCompleteEvent(mCompleteEvent);
        configDma(mDma);
        mDmaOwner = true;
    }
  
    mEnabled = enable;
    FunctionalState en = enable? ENABLE: DISABLE;
    ADC_Cmd(mAdc, en);    
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
    
    if (mMode==ModeSingle)
        ADC_DMARequestAfterLastTransferCmd(mAdc, ENABLE);
    
    ADC_DMACmd(mAdc, ENABLE);
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
    mConfig.ADC_ContinuousConvMode = enabled? ENABLE: DISABLE;
    ADC_Init(mAdc, &mConfig);
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