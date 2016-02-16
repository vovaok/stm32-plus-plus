#include "audiodev.h"

AudioDevice::AudioDevice(const AudioFormat &format) :
    mDma(0L),
    mBuffer(0L),
    mBufferSize(0)
{
    mFormat = format;
    int bufsize = (mFormat.sampleRate() * mFormat.bytesPerSample() * mFormat.channelCount() * 2) / 1000;
    setBufferSize(bufsize);
    for (int i=0; i<mBufferSize; i++)
        mBuffer[i] = 0;
}

AudioDevice::~AudioDevice()
{
    if (mBuffer)
        delete [] mBuffer;
}
//---------------------------------------------------------------------------

void AudioDevice::setBufferSize(int size)
{
    unsigned char *tempBuffer = new unsigned char[size];
    unsigned char *delBuffer = mBuffer;
    mBuffer = tempBuffer;
    mBufferSize = size;
    if (delBuffer)
        delete [] delBuffer;
}
//---------------------------------------------------------------------------

int AudioDevice::read(void *data, int size)
{
    if (!mDma)
        return 0;
    
    if (!mDma->isEnabled())
        mDma->start();
    
    int maxsize = bufferSize() >> 1;
    int rdsize = size>maxsize? maxsize: size;
    
    if (mDma->currentPage() == 1)
        memcpy(data, buffer(), rdsize);
    else
        memcpy(data, buffer()+maxsize, rdsize);
    
    return rdsize;
}

int AudioDevice::write(void *data, int size)
{
    if (!mDma)
        return 0;
    
    int maxsize = bufferSize() >> 1;
    int wrsize = size>maxsize? maxsize: size;
    
#ifdef SPEAKER_USE_DOUBLE_BUFFER
    if (mDma->currentPage() == 1)
        memcpy(buffer(), data, wrsize);
    else
        memcpy(buffer()+maxsize, data, wrsize);
    if (!mDma->isEnabled())
        mDma->start(size / mFormat.bytesPerSample());
#else    
    mDma->stop();
    memcpy(buffer(), data, wrsize);
    mDma->start(size / mFormat.bytesPerSample());
#endif
    
    return wrsize;
}
//---------------------------------------------------------------------------

void AudioDevice::pause(bool pauseOn)
{
    if (mDma)
        mDma->setEnabled(!pauseOn);
}

void AudioDevice::stop()
{
    if (mDma)
        mDma->stop();
}
//---------------------------------------------------------------------------
