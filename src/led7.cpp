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
    sprintf(s, "% 4d", value);
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
    sprintf(s, "% 4d", (int)value);
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

const uint8_t Led7::font[256] =
{
    (uint8_t)CODE_0, (uint8_t)CODE_1, (uint8_t)CODE_2, (uint8_t)CODE_3, (uint8_t)CODE_4, (uint8_t)CODE_5, (uint8_t)CODE_6, (uint8_t)CODE_7,
    (uint8_t)CODE_8, (uint8_t)CODE_9, (uint8_t)CODE_a, (uint8_t)CODE_b, (uint8_t)CODE_c, (uint8_t)CODE_d, (uint8_t)CODE_e, (uint8_t)CODE_f,
    (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00,
    (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00,

    (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x22, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x20,
    (uint8_t)~0x39, (uint8_t)~0x0F, (uint8_t)~0x63, (uint8_t)~0x00,  0x7F, (uint8_t)~0x40,  0x7f, (uint8_t)~0x00,
    (uint8_t)CODE_0, (uint8_t)CODE_1, (uint8_t)CODE_2, (uint8_t)CODE_3, (uint8_t)CODE_4, (uint8_t)CODE_5, (uint8_t)CODE_6, (uint8_t)CODE_7,
    (uint8_t)CODE_8, (uint8_t)CODE_9, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x00, (uint8_t)~0x48, (uint8_t)~0x00, (uint8_t)~0x53,

    (uint8_t)~0x00, (uint8_t)~0x77, (uint8_t)~0x7c, (uint8_t)~0x39, (uint8_t)~0x5e, (uint8_t)~0x79, (uint8_t)~0x71, (uint8_t)~0x3d,
    (uint8_t)~0x76, (uint8_t)~0x30, (uint8_t)~0x1E, (uint8_t)~0x00, (uint8_t)~0x38, (uint8_t)~0x37, (uint8_t)~0x54, (uint8_t)~0x3F,
    (uint8_t)~0x73, (uint8_t)~0x00, (uint8_t)~0x50, (uint8_t)~0x6d, (uint8_t)~0x78, (uint8_t)~0x3E, (uint8_t)~0x00, (uint8_t)~0x00,
    (uint8_t)~0x00, (uint8_t)~0x6E, (uint8_t)~0x00, (uint8_t)~0x39, (uint8_t)~0x00, (uint8_t)~0x0F, (uint8_t)~0x00, (uint8_t)~0x08,

    (uint8_t)~0x00, (uint8_t)~0x77, (uint8_t)~0x7c, (uint8_t)~0x58, (uint8_t)~0x5e, (uint8_t)~0x79, (uint8_t)~0x71, (uint8_t)~0x3d,
    (uint8_t)~0x74, (uint8_t)~0x10, (uint8_t)~0x0e, (uint8_t)~0x00, (uint8_t)~0x06, (uint8_t)~0x00, (uint8_t)~0x54, (uint8_t)~0x5c,
    (uint8_t)~0x73, (uint8_t)~0x00, (uint8_t)~0x50, (uint8_t)~0x6d, (uint8_t)~0x78, (uint8_t)~0x1c, (uint8_t)~0x00, (uint8_t)~0x00,
    (uint8_t)~0x00, (uint8_t)~0x6E, (uint8_t)~0x00, (uint8_t)~0x39, (uint8_t)~0x06, (uint8_t)~0x0F, (uint8_t)~0x40, (uint8_t)~0x00,

    (uint8_t)~0x00, (uint8_t)~0x01, (uint8_t)~0x02, (uint8_t)~0x03, (uint8_t)~0x04, (uint8_t)~0x05, (uint8_t)~0x06, (uint8_t)~0x07,
    (uint8_t)~0x08, (uint8_t)~0x09, (uint8_t)~0x0A, (uint8_t)~0x0B, (uint8_t)~0x0C, (uint8_t)~0x0D, (uint8_t)~0x0E, (uint8_t)~0x0F,
    (uint8_t)~0x10, (uint8_t)~0x11, (uint8_t)~0x12, (uint8_t)~0x13, (uint8_t)~0x14, (uint8_t)~0x15, (uint8_t)~0x16, (uint8_t)~0x17,
    (uint8_t)~0x18, (uint8_t)~0x19, (uint8_t)~0x1A, (uint8_t)~0x1B, (uint8_t)~0x1C, (uint8_t)~0x1D, (uint8_t)~0x1E, (uint8_t)~0x1F,
    (uint8_t)~0x20, (uint8_t)~0x21, (uint8_t)~0x22, (uint8_t)~0x23, (uint8_t)~0x24, (uint8_t)~0x25, (uint8_t)~0x26, (uint8_t)~0x27,
    (uint8_t)~0x28, (uint8_t)~0x29, (uint8_t)~0x2A, (uint8_t)~0x2B, (uint8_t)~0x2C, (uint8_t)~0x2D, (uint8_t)~0x2E, (uint8_t)~0x2F,
    (uint8_t)~0x30, (uint8_t)~0x31, (uint8_t)~0x32, (uint8_t)~0x33, (uint8_t)~0x34, (uint8_t)~0x35, (uint8_t)~0x36, (uint8_t)~0x37,
    (uint8_t)~0x38, (uint8_t)~0x39, (uint8_t)~0x3A, (uint8_t)~0x3B, (uint8_t)~0x3C, (uint8_t)~0x3D, (uint8_t)~0x3E, (uint8_t)~0x3F,
    (uint8_t)~0x40, (uint8_t)~0x41, (uint8_t)~0x42, (uint8_t)~0x43, (uint8_t)~0x44, (uint8_t)~0x45, (uint8_t)~0x46, (uint8_t)~0x47,
    (uint8_t)~0x48, (uint8_t)~0x49, (uint8_t)~0x4A, (uint8_t)~0x4B, (uint8_t)~0x4C, (uint8_t)~0x4D, (uint8_t)~0x4E, (uint8_t)~0x4F,
    (uint8_t)~0x50, (uint8_t)~0x51, (uint8_t)~0x52, (uint8_t)~0x53, (uint8_t)~0x54, (uint8_t)~0x55, (uint8_t)~0x56, (uint8_t)~0x57,
    (uint8_t)~0x58, (uint8_t)~0x59, (uint8_t)~0x5A, (uint8_t)~0x5B, (uint8_t)~0x5C, (uint8_t)~0x5D, (uint8_t)~0x5E, (uint8_t)~0x5F,
    (uint8_t)~0x60, (uint8_t)~0x61, (uint8_t)~0x62, (uint8_t)~0x63, (uint8_t)~0x64, (uint8_t)~0x65, (uint8_t)~0x66, (uint8_t)~0x67,
    (uint8_t)~0x68, (uint8_t)~0x69, (uint8_t)~0x6A, (uint8_t)~0x6B, (uint8_t)~0x6C, (uint8_t)~0x6D, (uint8_t)~0x6E, (uint8_t)~0x6F,
    (uint8_t)~0x70, (uint8_t)~0x71, (uint8_t)~0x72, (uint8_t)~0x73, (uint8_t)~0x74, (uint8_t)~0x75, (uint8_t)~0x76, (uint8_t)~0x77,
    (uint8_t)~0x78, (uint8_t)~0x79, (uint8_t)~0x7A, (uint8_t)~0x7B, (uint8_t)~0x7C, (uint8_t)~0x7D, (uint8_t)~0x7E, (uint8_t)~0x7F,
};

//---------------------------------------------------------------------------
