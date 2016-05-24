#include "microphone.h"

Microphone::Microphone(const AudioFormat &format, int interface1, ...) :
    AbstractMicrophone(format),
    mAdc(0L),
    mOpened(false)
{
    mTimer = new HardwareTimer(Tim3, format.sampleRate());
    mTimer->selectOutputTrigger(HardwareTimer::TrgUpdate);
    
    int count = format.channelCount();
    mInterfaces = new Adc::Channel[count];
    mInterfaces[0] = (Adc::Channel)interface1;
    va_list vl;
    va_start(vl, interface1);
    for (int i=1; i<count; i++)
    {
        mInterfaces[i] = (Adc::Channel)va_arg(vl, int);
    }
    va_end(vl);
}

Microphone::~Microphone()
{
    delete [] mInterfaces; 
}
//---------------------------------------------------------------------------

void Microphone::open()
{
    if (mOpened)
        return;
    Adc::Resolution res;
    switch (mFormat.sampleSize())
    {
        case 6: res = Adc::Res6bit; break;
        case 8: res = Adc::Res8bit; break;
        case 10: res = Adc::Res10bit; break;
        case 12: res = Adc::Res12bit; break;
        case 16: res = Adc::Res16bit; break;
        default: return;
    }
  
    mAdc = new Adc(1);
    mAdc->setResolution(res);
    mAdc->selectTrigger(Adc::TriggerTim3, Adc::EdgeRising);
    for (int i=0; i<mFormat.channelCount(); i++)
    {
        mAdc->addChannel(mInterfaces[i], Adc::SampleTime_28Cycles);
    }
    
    mDma = Dma::getStreamForPeriph(Dma::ChannelAdc1);
    mDma->setDoubleBuffer(buffer(), buffer() + bufferSize()/2, bufferSize()/(2 * mFormat.bytesPerSample()));
    mAdc->configDma(mDma);    
    
    mAdc->start();
    mDma->start();
    mTimer->start();
    
    mOpened = true;
}

void Microphone::close()
{
    mTimer->stop();
    mDma->stop();
  
    if (mDma)
    {
        delete mDma;
        mDma = 0L;
    }
  
    if (mAdc)
    {
        delete mAdc;
        mAdc = 0L;
    }
    
    mOpened = false;
}
//---------------------------------------------------------------------------