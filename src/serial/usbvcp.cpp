#include "usbvcp.h"

UsbVcp::UsbVcp(UsbDevice::UsbCore usb, unsigned short vid, unsigned short pid)
{
    mDev = new UsbDevice(usb);
    mDev->setVidPid(vid, pid);
    char serial[9];
    sprintf(serial, "%08X", CpuId::serial());
    mDev->setSerialNumber(serial);
    
    mDev->setDeviceClass(2, 2, 0); // for win10
    unsigned long ver = Application::version();
    mDev->setDeviceRevisionNumber((ver >> 8) & 0xFF, ver & 0xFF);
    mDev->setManufacturer(Application::company());
    mDev->setProduct(Application::name());
        
    initDevice();
}

UsbVcp::UsbVcp(UsbDevice::UsbCore usb, const char *serial)
{
    mDev = new UsbDevice(usb);
    mDev->setVidPid(0x0483, 0x5740);
    mDev->setSerialNumber(serial);
    
    mDev->setDeviceClass(2, 2, 0); // for win10
    unsigned long ver = Application::version();
    mDev->setDeviceRevisionNumber((ver >> 8) & 0xFF, ver & 0xFF);
    mDev->setManufacturer(Application::company());
    mDev->setProduct(Application::name());
    
//    mCfg = new UsbConfiguration();
//    mDev->attachNode(mCfg);
    
    initDevice();
}

UsbVcp::UsbVcp(UsbConfiguration *usbconf) :
    mDev(0L),
    mCfg(usbconf)
{
    initDevice();
}

void UsbVcp::initDevice()
{  
//    mDev->setDeviceClass(2, 2, 0); // for win10
//    unsigned long ver = Application::version();
//    mDev->setDeviceRevisionNumber((ver >> 8) & 0xFF, ver & 0xFF);
//    mDev->setManufacturer(Application::company());
//    mDev->setProduct(Application::name());
    
    mBufSize = 4095;
    mBuffer.resize(mBufSize);
    mBufHead = mBufTail = 0;
  
    UsbNode *iad;
  
    if (mDev)
    {
        mCfg = new UsbConfiguration();
        mDev->attachNode(mCfg);
        iad = mCfg;
    }
    else
    {
        iad = new UsbInterfaceCollection(2, 2, 0, "ONB");
        mCfg->attachNode(iad);
    }
  
    mIfCtrl = new UsbVcpCtrlInterface(iad);
    mIfData = new UsbVcpDataInterface(iad);
    mIfData->setReceiveEvent(EVENT(&UsbVcp::onReceive));
    
    if (mDev)
        mDev->start();
}

UsbVcp::~UsbVcp()
{
    delete mDev;
}

void UsbVcp::onReceive(const ByteArray &ba)
{
//    mBuffer.append(ba);
    for (int i=0; i<ba.size(); i++)
    {
        mBuffer[mBufHead++] = ba[i];
        if (mBufHead >= mBufSize)
            mBufHead = 0;
        if (mBufHead == mBufTail)
        {
            setErrorString("[USB VCP] RX buffer overflow!\n");
            return;
        }
    }
//    if (onReadyRead)
//        onReadyRead();
}
//---------------------------------------------------------------------------

int UsbVcp::writeData(const char *data, int size)
{
    ByteArray ba(data, size);
    mIfData->sendData(ba);
    return ba.size();
}

int UsbVcp::readData(char *data, int size)
{
    int sz = (mBufHead - mBufTail + mBufSize) % mBufSize;
    if (sz > size)
        sz = size;
    for (int i=0; i<sz; i++)
    {
        *data++ = mBuffer[mBufTail++];
        if (mBufTail >= mBufSize)
            mBufTail = 0;
    }
    return sz;
}

int UsbVcp::bytesAvailable() const
{
    return (mBufHead - mBufTail + mBufSize) % mBufSize;
}
//---------------------------------------------------------------------------