#include "usbvcp.h"

UsbVcp::UsbVcp(UsbDevice::UsbCore usb, unsigned short vid, unsigned short pid)
{
    mDev = new UsbDevice(usb);
    mDev->setVidPid(vid, pid);
    char serial[9];
    sprintf(serial, "%08X", CpuId::serial());
    mDev->setSerialNumber(serial);
    initDevice();
}

UsbVcp::UsbVcp(UsbDevice::UsbCore usb, const char *serial)
{
    mDev = new UsbDevice(usb);
    mDev->setVidPid(0x0483, 0x5740);
    mDev->setSerialNumber(serial);
    initDevice();
}

void UsbVcp::initDevice()
{
    mDev->setDeviceClass(2, 2, 0); // for win10
    unsigned long ver = Application::version();
    mDev->setDeviceRevisionNumber((ver >> 8) & 0xFF, ver & 0xFF);
    mDev->setManufacturer(Application::company());
    mDev->setProduct(Application::name());
    
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