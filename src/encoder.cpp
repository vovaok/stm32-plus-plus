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
    setAutoReloadRegister((uint32_t)-1);
    if (pinB == Gpio::NoConfig)
    {
        int ch = GpioConfigGetPeriphChannel(pinA);
        if (ch == 1)
            tim()->SMCR |= 2; // Encoder mode 2 - Counter counts up/down on TI1FP1 edge depending on TI2FP2 level.
        else if (ch == 2)
            tim()->SMCR |= 1; // Encoder mode 1 - Counter counts up/down on TI2FP2 edge depending on TI1FP1 level.
        else
            THROW(Exception::InvalidPin);
    }
    else
    {
        tim()->SMCR |= 3; // Encoder mode 3 - Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input.
        //TIM_EncoderInterfaceConfig(tim(), TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
    }
    tim()->CCMR1 |= (2<<4) | (2<<12); // filter

//    setUpdateEvent(EVENT(&Encoder::overflowHandler));

//    Task::Add(this);

    m_updateTimer = new Timer;
    m_updateTimer->setTimeoutEvent(EVENT(&Encoder::update));

    setEnabled(true);
    valid = false;
}

void Encoder::open()
{
    setEnabled(true);
    m_updateTimer->start(1);
}

void Encoder::close()
{
    setEnabled(false);
    m_updateTimer->stop();
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

void Encoder::setUpdateInterval(int value_ms)
{
    if (value_ms)
        m_updateTimer->start(value_ms);
    else
        m_updateTimer->stop();
}

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