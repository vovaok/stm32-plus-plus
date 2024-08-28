#include "dac124s085.h"


Dac124s085::Dac124s085(Spi *spi, Gpio::PinName csPin) :
    mSpi(spi),
    mResetPin(0L),
    mDrdyPin(0L),
    mEnabled(false)
{
 
  
    mCsPin = new Gpio(csPin);
    mCsPin->setAsOutput();
  
    deselect();
    Spi::Config conf; 
    conf.CPHA = 1;
    conf.CPOL = 0;
    conf.master = 1;
    conf.baudrate = 5;
    mSpi->setConfig(conf);
    mSpi->setDataSize(16);
    mSpi->open();     
   
}


void Dac124s085::writeRegistr(Message cmd)
{
    select();
    mSpi->write(cmd.cmdDac);
    deselect();
}

void Dac124s085::writeRegistr(char ch,char cmd, unsigned short value)
{
  
  mMessage.DAC_ch = ch;
  mMessage.cmd = cmd;
  mMessage.value = value;
  select();
  mSpi->write(mMessage.cmdDac);
  deselect();
  
}


void Dac124s085::select()
{
    mCsPin->reset();
    for (int i=0; i<100; i++);
}

void Dac124s085::deselect()
{
    for (int i=0; i<100; i++);
    mCsPin->set();
    for (int i=0; i<100; i++);
}











