#include "encoder.h"

Encoder::Encoder(Gpio::Config pinA, Gpio::Config pinB, Gpio::Config pinIdx) :
    HardwareTimer((TimerNumber)GpioConfigGetPeriphNumber(pinA)),
    mPulses(1),
    mPosition(0),
    mSpeed(0),
    mFilter(0.75f)
//    mRevolutions(0),
//    mMaxRevolutions(pulses*32)
{
    Gpio::config(3, pinA, pinB, pinIdx);
    setAutoReloadRegister(0xFFFF);
    tim()->SMCR |= 3;//TIM_EncoderMode_TI12
    //TIM_EncoderInterfaceConfig(tim(), TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
    tim()->CCMR1 |= (2<<4) | (2<<12); 
    
//    setUpdateEvent(EVENT(&Encoder::overflowHandler));
    
//    Task::Add(this);
    
    Timer *timer = new Timer;
    timer->setTimeoutEvent(EVENT(&Encoder::update));
    timer->start(1);
    
    setEnabled(true);
    valid = false;
}

void Encoder::open()
{
    setEnabled(true);
}

void Encoder::close()
{
    setEnabled(false);
}

//void Encoder::Execute()
//{
//    setEnabled(true);
//    bool valid = false;
//    
//    while (1)
//    {
//        update();
//        Delay(5);
//    }
//}

void Encoder::update()
{  
    short newpos = counter();
    short deltaPos = newpos - mPosition;
    mPosition = newpos;
    float dt = m_precTimer.delta();
    if (!dt)
    {
        valid = false;
    }
    else if (valid)
    {
        float newspeed = (deltaPos * 60.0f / mPulses) / dt; // [rpm]
        mSpeed = mFilter * mSpeed + (1.0f - mFilter) * newspeed;
    }
    valid = true;
}

//void Encoder::overflowHandler()
//{
//    if (direction())
//        mRevolutions += mPulses;
//    else
//        mRevolutions -= mPulses;
//    if (mRevolutions >= mMaxRevolutions)
//        mRevolutions = 0;
//    else if (mRevolutions < 0)
//        mRevolutions = mMaxRevolutions - mPulses;
//}