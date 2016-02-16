#include "i2smicrophone.h"

I2SMicrophone::I2SMicrophone(const AudioFormat &format, I2S *device) :
    AbstractMicrophone(format),
    mDev(device),
    mOpened(false)
{
    
}

void I2SMicrophone::open()
{
    if (mOpened)
        return;// false;
    
    mDma = mDev->dmaForRx();
    mDma->setDoubleBuffer(buffer(), buffer() + bufferSize()/2, bufferSize()/(2 * mFormat.bytesPerSample()));

    mDma->start();
    mDev->open();
    
    mOpened = true;
    //return mOpened;
}

void I2SMicrophone::close()
{
    mDev->deleteDmaForRx();
    mDev->close();
}