#ifndef _ESP8266_H
#define _ESP8266_H

#include "serialinterface.h"
#include "usart.h"
#include "gpio.h"

namespace Serial
{

class ESP8266 : public SerialInterface
{
private:
    Usart *mUsart;
    Gpio *mResetPin;
    
    bool mTransparentMode;
    
    void task();
  
public:
    ESP8266(Usart *usart, Gpio::PinName resetPin);
    
    void hardReset();
    void sendCmd(ByteArray ba);
    
    int read(ByteArray &ba);
    int write(const ByteArray &ba);
    
    void interruptTransparentMode();
    void reset();
};

}

#endif