#include "speaker.h"

Speaker::Speaker(const AudioFormat &format, int interfaceNumber) :
    AbstractSpeaker(format),
    mDac(0L),
    mOpened(false)
{
    mTimer = 0L;
    if (interfaceNumber==2)
    {
        mChannelConfig = Dac::Channel2;
        //mMutePin = new Led('c', 13);
        mTimer = new HardwareTimer(Tim7, format.sampleRate());
    }
    else
    {
        mChannelConfig = Dac::Channel1;
        //mMutePin = new Led('c', 13);
        mTimer = new HardwareTimer(Tim6, format.sampleRate());
    }
    
    //mMutePin->off();
    
    mTimer->selectOutputTrigger(HardwareTimer::TrgUpdate);
}
//---------------------------------------------------------------------------

void Speaker::open()
{
    if (mOpened)
        return;
    Dac::Resolution res;
    switch (mFormat.sampleSize())
    {
        case 8: res = Dac::Res8bit; break;
        case 12: res = Dac::Res12bit; break;
        case 16: res = Dac::Res16bit; break;
        default: return;
    }
  
    if (mFormat.channelCount() == Mono)
        mDac = new Dac(mChannelConfig, res);
    else if (mFormat.channelCount() == Stereo)
        mDac = new Dac(Dac::ChannelBoth, res);
    
    mDma = Dma::getStreamForPeriph(mChannelConfig==Dac::Channel2? Dma::ChannelDac2: Dma::ChannelDac1);
#ifdef SPEAKER_USE_DOUBLE_BUFFER
    mDma->setDoubleBuffer(buffer(), buffer() + bufferSize()/2, bufferSize()/(2 * mFormat.bytesPerSample()));
#else
    mDma->setSingleBuffer(buffer(), bufferSize()/(2 * mFormat.bytesPerSample()));
#endif
    mDac->configDma(mDma);
    mDac->selectTrigger(mChannelConfig==Dac::Channel2? Dac::TriggerTim7: Dac::TriggerTim6);     
    
#ifdef SPEAKER_USE_DOUBLE_BUFFER
    mDma->start();
#endif
    mTimer->start();
    
    mOpened = true;
}

void Speaker::close()
{
    mTimer->stop();
    mDma->stop();
  
    if (mDma)
    {
        delete mDma;
        mDma = 0L;
    }
  
    if (mDac)
    {
        delete mDac;
        mDac = 0L;
    }
    
    mOpened = false;
}
    
void Speaker::mute(bool muting)
{
    //mMutePin->setState(muting);
}

bool Speaker::isMuted()
{
    return 0;//mMutePin->state(); 
}
//---------------------------------------------------------------------------

//void Speaker::writeSample(int sample)
//{
//    if (mDac)
//        mDac->setValue(sample);
//}
//---------------------------------------------------------------------------


