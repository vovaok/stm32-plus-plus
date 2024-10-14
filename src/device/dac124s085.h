#ifndef _DAC124S085_H
#define _DAC124S085_H

#include "spi.h"



union Message
{

unsigned short cmdDac;
struct
{
  unsigned short value: 12;  // value 0-4095
  
  unsigned char cmd: 2;      /* 00 write, not upade
                              01 write and update
                              10 write and update all ch
                              11 all outputs 0    */  

  unsigned char DAC_ch: 2;   // dacA 00 dacB 01 dacC 10 dacD 11



};

Message() : cmdDac(0){ }
};


typedef enum
{
  BEAM_X =0,
  BEAM_Y,
  BEAM_Z
}DAC_ch;

typedef enum
{
  WRITE =0,
  WRITE_UPDATE,
  WRITE_UPDATE_ALL,
  ALL_ZERO
}DAC_cmd;




class Dac124s085
{
public:
 
  
private:
    Spi *mSpi;
    Gpio *mCsPin;
    Gpio *mResetPin;
    Gpio *mDrdyPin;   
    bool mEnabled;
    
  
    void select();
    void deselect();  
    Message mMessage;
    
  
public:
    Dac124s085(Spi *spi, Gpio::PinName csPin);
    void setResetPin(Gpio::PinName pin);
    void writeRegistr(Message cmd);
    void writeRegistr(char ch,char cmd, unsigned short value);
 
};

#endif