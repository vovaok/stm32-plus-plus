#include "esp8266.h"

ByteArray testBuf;

using namespace Serial;

ESP8266::ESP8266(Usart *usart, Gpio::PinName resetPin) :
    mUsart(usart),
    mTransparentMode(false)
{
    mResetPin = new Gpio(resetPin);
    mResetPin->write(0);
  
    mUsart->setBaudrate(78400);
    mUsart->setBufferSize(256);
    mUsart->setUseDma(true);
    mUsart->open(ReadWrite);
    
    mResetPin->setAsInputPullUp();
}

void ESP8266::task()
{
    if (mUsart->canReadLine())
    {
        ByteArray line;
        mUsart->readLine(line);
        
        // parse line
        testBuf.append(line);
    }
}

void ESP8266::sendCmd(ByteArray ba)
{
    ba.append(0x0d);
    ba.append(0x0a);
    mUsart->write(ba);
}
//---------------------------------------------------------------------------

void ESP8266::hardReset()
{
    mResetPin->setAsOutput();
    mResetPin->write(0);
    for (int i=0; i<1000; i++);
    mResetPin->setAsInputPullUp();
}
//---------------------------------------------------------------------------

int ESP8266::read(ByteArray &ba)
{
    if (mTransparentMode)
        return mUsart->read(ba);
    return -1;
}

int ESP8266::write(const ByteArray &ba)
{
    if (mTransparentMode)
        return mUsart->write(ba);
    return -1;
}
//---------------------------------------------------------------------------

void ESP8266::interruptTransparentMode()
{
    mUsart->write("+++");
    for (int i=0; i<10000; i++);
}

void ESP8266::reset()
{
    sendCmd("AT+RST");
}
//---------------------------------------------------------------------------