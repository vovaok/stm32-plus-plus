#ifndef _ENCODER_H
#define _ENCODER_H

//#include <macs_task.hpp>
#include "hardwaretimer.h"
#include "precisetimer.h"
#include "timer.h"

class Encoder : private HardwareTimer//, public Task
{
private:
    int mPulses;
    short mPosition;
    float mSpeed;
    float mFilter;
//    int mRevolutions;
//    int mMaxRevolutions;
    
    PreciseTimer m_precTimer;
    bool valid;
    
//    void overflowHandler();
    
    void update();
    
protected:
//    virtual void Execute() override;
    
public:
    Encoder(Gpio::Config pinA, Gpio::Config pinB, Gpio::Config pinIdx=Gpio::NoConfig);
    
    using HardwareTimer::counter;
    using HardwareTimer::setCounter;
    
    void open();
    void close();
    
    const float &speed() const {return mSpeed;}
    
    bool direction() const {return (tim()->CR1 & TIM_CR1_DIR)? false: true;}
    void setPulsesCount(int value) {mPulses = value;}
    int pulses() const {return mPulses;}
    
//    unsigned long position() const {return mRevolutions + counter();}
//    void setPosition(unsigned long value) {mRevolutions = value / mPulses; setCounter(value - mRevolutions);}
    short position() const {return counter();}//mPosition;}
    void setPosition(short value) {mPosition = value; setCounter(value);}    
    void resetPosition() {mPosition = 0; setCounter(0);}
    
    void setFilter(float factor) {mFilter = BOUND(0, factor, 1);}
};

#endif
