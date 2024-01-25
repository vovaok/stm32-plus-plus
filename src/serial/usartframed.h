#pragma once

#include "usart.h"

class UsartFramed : public Usart
{
public:
    UsartFramed(Gpio::Config pinTx, Gpio::Config pinRx);
    
protected:
//    virtual bool fillBuffer(const char *data, int size) override;
    virtual int writeData(const char *data, int size) override;
    virtual int readData(char *data, int size) override;
};