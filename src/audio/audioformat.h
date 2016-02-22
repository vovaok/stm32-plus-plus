#ifndef _AUDIOFORMAT_H
#define _AUDIOFORMAT_H

class AudioFormat
{
private:
    int mChannelCount;
    int mSampleSize;
    int mSampleRate;
    int mBytesPerSample;
    
    void calcPrivateInfo()
    {
        mBytesPerSample = (((mSampleSize - 1) >> 3) + 1);
    }
public:
    AudioFormat() :
      mChannelCount(2),
      mSampleSize(16),
      mSampleRate(48000),
      mBytesPerSample(2)
      {}
    void setChannelCount(int value) {mChannelCount = value; calcPrivateInfo();}
    int channelCount() const {return mChannelCount;}
    void setSampleSize(int value) {mSampleSize = value; calcPrivateInfo();}
    int sampleSize() const {return mSampleSize;}
    void setSampleRate(int value) {mSampleRate = value;}
    int sampleRate() const {return mSampleRate;}
    int bytesPerSample() const {return mBytesPerSample;}
};

#endif