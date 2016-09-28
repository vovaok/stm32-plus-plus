#ifndef _AS5040MASTER_H
#define _AS5040MASTER_H

#include "spi.h"
#include <vector>

class As5040Master
{
public:
#pragma pack(push,1)
    typedef union
    {
        unsigned char word;
        struct
        {
            unsigned char parity: 1;
            unsigned char magDec: 1;
            unsigned char magInc: 1;
            unsigned char LIN: 1;
            unsigned char COF: 1;
            unsigned char OCF: 1;
        };
    } Flags;
#pragma pack(pop)
  
private:
    typedef struct
    {
        Gpio *cs;
        unsigned short raw;
        unsigned short zero;
        float value;
        Flags flags;
    } Channel;
  
private:
    Spi *mSpi;
    std::vector<Channel> mChannels;
    int mCurChannel;
    
    void onRead(unsigned short rawValue);
    
public:
    As5040Master(Spi *spi);
    
    unsigned char addChannel(Gpio::PinName csPin, float zeroDeg = 0);
    void start();
    
    void setZero(unsigned char channel);
    void setZero(unsigned char channel, float zeroDeg);
    float zeroDeg(unsigned char channel) const;
    
    bool isValid(unsigned char channel);
    float valueDeg(unsigned char channel);
    float valueRad(unsigned char channel);
    Flags flags(unsigned char channel);
    
    bool isReady() const {return mCurChannel >= mChannels.size();}
    NotifyEvent onFinish;
};

#endif