#include "led7.h"

Led7::Led7(int digitCount, Gpio::PinName dataFirstPin, Gpio::PinName controlFirstPin) :
    mDigitCount(digitCount),
    mShiftCnt(0)
{
    mBuffer.resize(digitCount);

    Gpio::PortName dataPort = static_cast<Gpio::PortName>(dataFirstPin & 0xF0);
    mDataStartPin = dataFirstPin & 0xF;
    mDataPort = new Gpio(dataPort, 0xFF << mDataStartPin);
    mDataPort->setAsOutputOpenDrain();
    mDataPort->write(1);
    
    Gpio::PortName controlPort = static_cast<Gpio::PortName>(controlFirstPin & 0xF0);
    mControlStartPin = controlFirstPin & 0xF;
    unsigned short cmask = ((1 << digitCount) - 1) << mControlStartPin;
    mControlPort = new Gpio(controlPort, cmask);
    mControlPort->setAsOutput();
    mControlPort->write(1);
  
    mTimer = new Timer();
    mTimer->setTimeoutEvent(EVENT(&Led7::onTimer));
    mTimer->start(1); // ms 
}

Led7::~Led7()
{
}
//---------------------------------------------------------------------------

void Led7::onTimer()
{
    mControlPort->writePort((~(1<<mShiftCnt)) << mControlStartPin);
    mDataPort->writePort(mBuffer[mShiftCnt] << mDataStartPin);
    
    mShiftCnt++;
    if (mShiftCnt >= mDigitCount)
        mShiftCnt = 0;
}
//---------------------------------------------------------------------------

void Led7::setBuffer(const char *buffer, char dot)
{
    const char *ptr = buffer;
    for(int i=0; i<mDigitCount; i++)
    {
        char b = font[*ptr++];
        if (i && b == 0x7F && mBuffer[i-1] != 0x7F) // dot
        {
            mBuffer[--i] &= ~0x80;
            continue;
        }
        if (dot & (1<<i))
            b &= ~0x80;
        mBuffer[i] = b;
    }
}

void Led7::setInt(int value, char dot)
{

  if(value<0)
    value = -value;
  
  
  
  char tempBuffer[4];
  
  tempBuffer[0] = value/1000;
  tempBuffer[1] = (value%1000)/100;
  tempBuffer[2] = (value%100)/10;
  tempBuffer[3] = value%10;
  
  setBuffer(tempBuffer,dot);
}

void Led7::write(int value)
{
    int maxval = 1;
    for (int i=0; i<mDigitCount; i++)
        maxval *= 10;
    if (value >= maxval)
        value = maxval-1;
    
    char s[16];
    sprintf(s, "%d", value);
    setBuffer(s);
}

void Led7::write(unsigned short value)
{
    char s[5];
    int sz = sprintf(s, "%04x", value);
    setBuffer(s);
}

void Led7::write(unsigned char value)
{
    char s[5];
    sprintf(s, "%d", (int)value);
    setBuffer(s);
}

void Led7::write(float value)
{
    char s[16];
    sprintf(s, "%.3f", value);
    setBuffer(s);
}

void Led7::write(const char *s)
{
    setBuffer(s);
}
//---------------------------------------------------------------------------

#define CODE_0	0xC0
#define CODE_1	0xF9
#define CODE_2	0xA4
#define CODE_3	0xB0
#define CODE_4	0x99
#define CODE_5	0x92
#define CODE_6	0x82
#define CODE_7	0xf8
#define CODE_8	0x80
#define CODE_9	0x90
#define CODE_a	0x88
#define CODE_b	0x83
#define CODE_c	0xc6
#define CODE_d	0xa1
#define CODE_e	0x86
#define CODE_f	0x8e

const unsigned char Led7::font[256] = 
{
    CODE_0, CODE_1, CODE_2, CODE_3, CODE_4, CODE_5, CODE_6, CODE_7, 
    CODE_8, CODE_9, CODE_a, CODE_b, CODE_c, CODE_d, CODE_e, CODE_f,
    ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, 
    ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, ~0x00, 
    
    ~0x00, ~0x00, ~0x22, ~0x00, ~0x00, ~0x00, ~0x00, ~0x20,
    ~0x39, ~0x0F, ~0x63, ~0x00,  0x7F, ~0x40,  0x7f, ~0x00, 
    CODE_0, CODE_1, CODE_2, CODE_3, CODE_4, CODE_5, CODE_6, CODE_7,
    CODE_8, CODE_9, ~0x00, ~0x00, ~0x00, ~0x48, ~0x00, ~0x53,
    
    ~0x00, ~0x77, ~0x7c, ~0x39, ~0x5e, ~0x79, ~0x71, ~0x3d,
    ~0x76, ~0x30, ~0x1E, ~0x00, ~0x38, ~0x37, ~0x54, ~0x3F,
    ~0x73, ~0x00, ~0x50, ~0x6d, ~0x78, ~0x3E, ~0x00, ~0x00,
    ~0x00, ~0x6E, ~0x00, ~0x39, ~0x00, ~0x0F, ~0x00, ~0x08,
    
    ~0x00, ~0x77, ~0x7c, ~0x58, ~0x5e, ~0x79, ~0x71, ~0x3d,
    ~0x74, ~0x10, ~0x0e, ~0x00, ~0x06, ~0x00, ~0x54, ~0x5c,
    ~0x73, ~0x00, ~0x50, ~0x6d, ~0x78, ~0x1c, ~0x00, ~0x00,
    ~0x00, ~0x6E, ~0x00, ~0x39, ~0x06, ~0x0F, ~0x40, ~0x00,
    
    ~0x00, ~0x01, ~0x02, ~0x03, ~0x04, ~0x05, ~0x06, ~0x07,
    ~0x08, ~0x09, ~0x0A, ~0x0B, ~0x0C, ~0x0D, ~0x0E, ~0x0F,
    ~0x10, ~0x11, ~0x12, ~0x13, ~0x14, ~0x15, ~0x16, ~0x17,
    ~0x18, ~0x19, ~0x1A, ~0x1B, ~0x1C, ~0x1D, ~0x1E, ~0x1F,
    ~0x20, ~0x21, ~0x22, ~0x23, ~0x24, ~0x25, ~0x26, ~0x27,
    ~0x28, ~0x29, ~0x2A, ~0x2B, ~0x2C, ~0x2D, ~0x2E, ~0x2F,
    ~0x30, ~0x31, ~0x32, ~0x33, ~0x34, ~0x35, ~0x36, ~0x37,
    ~0x38, ~0x39, ~0x3A, ~0x3B, ~0x3C, ~0x3D, ~0x3E, ~0x3F,
    ~0x40, ~0x41, ~0x42, ~0x43, ~0x44, ~0x45, ~0x46, ~0x47,
    ~0x48, ~0x49, ~0x4A, ~0x4B, ~0x4C, ~0x4D, ~0x4E, ~0x4F,
    ~0x50, ~0x51, ~0x52, ~0x53, ~0x54, ~0x55, ~0x56, ~0x57,
    ~0x58, ~0x59, ~0x5A, ~0x5B, ~0x5C, ~0x5D, ~0x5E, ~0x5F,
    ~0x60, ~0x61, ~0x62, ~0x63, ~0x64, ~0x65, ~0x66, ~0x67,
    ~0x68, ~0x69, ~0x6A, ~0x6B, ~0x6C, ~0x6D, ~0x6E, ~0x6F,
    ~0x70, ~0x71, ~0x72, ~0x73, ~0x74, ~0x75, ~0x76, ~0x77,
    ~0x78, ~0x79, ~0x7A, ~0x7B, ~0x7C, ~0x7D, ~0x7E, ~0x7F,
};
//---------------------------------------------------------------------------