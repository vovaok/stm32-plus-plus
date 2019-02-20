#include "usbvcp.h"

UsbVcp::UsbVcp(UsbDevice::UsbCore usb, unsigned short vid, unsigned short pid)
{
    mDev = new UsbDevice(usb);
    mDev->setVidPid(vid, pid);
    unsigned long ver = Application::version();
    mDev->setDeviceRevisionNumber((ver >> 8) & 0xFF, ver & 0xFF);
    mDev->setManufacturer(Application::company());
    mDev->setProduct(Application::name());
    char serial[9];
    sprintf(serial, "%08X", CpuId::serial());
    mDev->setSerialNumber(serial);
    
    mCfg = new UsbConfiguration();
    mIfCtrl = new UsbVcpCtrlInterface();
    mIfData = new UsbVcpDataInterface();
    
    mIfData->setReceiveEvent(EVENT(&UsbVcp::onReceive));
    
    mDev->attachNode(mCfg);
    mCfg->attachNode(mIfCtrl);
    mCfg->attachNode(mIfData);
        
    mDev->start();
}

UsbVcp::~UsbVcp()
{
    delete mDev;
}

void UsbVcp::onReceive(const ByteArray &ba)
{
    mBuffer.append(ba);
    if (onReadyRead)
        onReadyRead();
}
//---------------------------------------------------------------------------

int UsbVcp::write(const ByteArray &ba)
{
    mIfData->sendData(ba);
    return ba.size();
}

int UsbVcp::read(ByteArray &ba)
{
    ba.append(mBuffer);
    int sz = mBuffer.size();
    mBuffer.clear();
    return sz;
}
//---------------------------------------------------------------------------