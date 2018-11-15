#include "radiointerface.h"

RadioInterface::RadioInterface(CC1200 *device) :
    cc1200(device)
{
    stmApp()->registerTaskEvent(EVENT(&RadioInterface::task));
}
//---------------------------------------------------------------------------

void RadioInterface::task()
{
    if (isOpen())
    {  
        int status = cc1200->getStatus() & 0x70;
        if (ledTx)
        {
            if (status == CC1200::TX || status == CC1200::FSTXON)
                ledTx->on();
            else
                ledTx->off();
        }
            
        if (status == CC1200::IDLE)
            cc1200->startRx();
        
        int sz = cc1200->getRxSize();
        if (sz)
        {
            if (ledRx)
                ledRx->on();
            unsigned char buf[128];
            cc1200->read(buf, sz);
            rxBuffer = string((const char*)buf + 1, buf[0]);
            mRssi = buf[sz-2];
            mLqi = buf[sz-1] & 0x7F;
        }
        else
        {
            if (ledRx)
                ledRx->off();
        }
    }
    else
    {
        if (ledTx)
            ledTx->off();
        if (ledRx)
            ledRx->off();
    }
}
//---------------------------------------------------------------------------

bool RadioInterface::open(OpenMode mode)
{
    
    SerialInterface::open(mode);
    return true;
}

void RadioInterface::close()
{
    
    SerialInterface::close();
}
//---------------------------------------------------------------------------

int RadioInterface::read(ByteArray &ba)
{
    int avail = cc1200->readReg(CC1200_NUM_RXBYTES);
    if (avail)
    {
        ba.resize(avail);
        cc1200->read(ba.data(), ba.size());
    }
    return avail;
}

int RadioInterface::write(const ByteArray &ba)
{
    int avail = 126 - cc1200->readReg(CC1200_NUM_TXBYTES);
    if (avail < ba.size())
        return 0;
    if (ledTx)
        ledTx->on();
    cc1200->send(ba.data(), ba.size());
}
//---------------------------------------------------------------------------
