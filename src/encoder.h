#ifndef _ENCODER_H
#define _ENCODER_H

#include "hardtimer.h"
#include "timer.h"

class Encoder : private HardwareTimer
{
private:
    int mPulses;
    short mPosition;
    float mSpeed;
    float mFilter;
//    int mRevolutions;
//    int mMaxRevolutions;
    
    Timer *mTimer;
    
//    void overflowHandler();
    
public:
    Encoder(TimerNumber timerNumber, int pulses, Gpio::Config pinA, Gpio::Config pinB, Gpio::Config pinIdx=Gpio::NoConfig);
    
    using HardwareTimer::counter;
    using HardwareTimer::setCounter;
    using HardwareTimer::setEnabled;
    
    void tick(float dt);
    
    const float &speed() const {return mSpeed;}
    
    bool direction() const {return (tim()->CR1 & TIM_CR1_DIR)? false: true;}
    int pulses() const {return mPulses;}
    
//    unsigned long position() const {return mRevolutions + counter();}
//    void setPosition(unsigned long value) {mRevolutions = value / mPulses; setCounter(value - mRevolutions);}
    short position() const {return mPosition;}
    void setPosition(short value) {mPosition = value; setCounter(value);}    
    void resetPosition() {/*mRevolutions = 0;*/ setCounter(0);}
    
    void setFilter(float factor) {mFilter = BOUND(0, factor, 1);}
};

class SpeedEncoder : private HardwareTimer
{
private:
    int mPulses;
    int mSpeedPeriod;
    int mSpeedAR, mSpeedAF, mSpeedBR, mSpeedBF;
    int mPosition;
    
    Gpio *pinA, *pinB;
    bool oldA, oldB;
    bool mDirection;
    
    void updateHandler();
    void captureHandler();
    
public:
    SpeedEncoder(TimerNumber timerNumber, int pulses, Gpio::Config pinA, Gpio::Config pinB);
    
    using HardwareTimer::setEnabled;
    
    int speed() const;
    unsigned long counter() const {return mPosition;}
    void setCounter(unsigned long value) {mPosition = value;}
    bool direction() const {return mDirection;} // true if positive
};

#endif
