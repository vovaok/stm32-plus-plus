#include "encoder.h"

Encoder::Encoder(TimerNumber timerNumber, int pulses, Gpio::Config pinA, Gpio::Config pinB, Gpio::Config pinIdx) :
    HardwareTimer(timerNumber),
    mPulses(pulses),
    mPosition(0),
    mSpeed(0),
    mFilter(0.75f)
//    mRevolutions(0),
//    mMaxRevolutions(pulses*32)
{
    switch (timerNumber)
    {
        case 6: case 7: case 10: case 11: case 13: case 14:
            throw Exception::invalidPeriph;
    }
    
    Gpio::config(3, pinA, pinB, pinIdx);
    setAutoReloadRegister(0xFFFF);//mPulses - 1);
    TIM_EncoderInterfaceConfig(tim(), TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
    tim()->CCMR1 |= (2<<4) | (2<<12);
    
//    mTimer = new Timer();
//    mTimer->setTimeoutEvent(EVENT(&Encoder::tick));
//    mTimer->start(2); // ms
    
//    setUpdateEvent(EVENT(&Encoder::overflowHandler));
    setEnabled(true);
}

void Encoder::tick(float dt)
{
    short newpos = counter();
    short deltaPos = newpos - mPosition;
    mPosition = newpos;
    float newspeed = (deltaPos * 60.0f / mPulses) / dt; // 1 rpm (2 ms period)
    mSpeed = mFilter * mSpeed + (1.0f - mFilter) * newspeed;
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
//---------------------------------------------------------------------------

SpeedEncoder::SpeedEncoder(TimerNumber timerNumber, int pulses, Gpio::Config pinA, Gpio::Config pinB) :
    HardwareTimer(timerNumber),
    mPulses(pulses),
    mSpeedPeriod(0),
    mPosition(0)
{    
    this->pinA = new Gpio(pinA);
    this->pinB = new Gpio(pinB);
//    Gpio::config(2, pinA, pinB);
    
    setAutoReloadRegister(inputClk() >> 3); // 1/8 секунды
    setCompare2((inputClk()>>3) - 1);
    setPrescaler(0);
    
    TIM_SelectInputTrigger(tim(), TIM_TS_TI1FP1); 
    TIM_SelectSlaveMode(tim(), TIM_SlaveMode_Reset); 
    
    // enable XOR function
    tim()->CR2 |= (1<<7);
    
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x2;
    TIM_ICInit(tim(), &TIM_ICInitStructure);
    
    //tim()->CCMR1 |= (2<<4) | (2<<12);
    
    setCC2Event(EVENT(&SpeedEncoder::updateHandler));
    setCC1Event(EVENT(&SpeedEncoder::captureHandler));
}

void SpeedEncoder::updateHandler()
{
    mSpeedAR = 0;
    mSpeedAF = 0;
    mSpeedBR = 0;
    mSpeedBF = 0;
    mSpeedPeriod = 0;
}

void SpeedEncoder::captureHandler()
{
    int speedPeriod = capture1();
    int pos = mPosition;
    bool A = pinA->read();
    bool B = pinB->read();
    bool direction = oldB ^ A;
    bool directionChanged = direction ^ mDirection;
    mDirection = direction;
    if (mDirection)
    {
        pos++;
    }
    else
    {
        pos--;
        speedPeriod = -speedPeriod;
    }
    
    if (!directionChanged)
    {
        if (!oldA && A)
            mSpeedAR = mSpeedAR? (3*mSpeedAR + speedPeriod) / 4: speedPeriod;
        else if (oldA && !A)
            mSpeedAF = mSpeedAF? (3*mSpeedAF + speedPeriod) / 4: speedPeriod;
        else if (!oldB && B)
            mSpeedBR = mSpeedBR? (3*mSpeedBR + speedPeriod) / 4: speedPeriod;
        else if (oldB & !B)
            mSpeedBF = mSpeedBF? (3*mSpeedBF + speedPeriod) / 4: speedPeriod;
        if (mSpeedAR && mSpeedAF && mSpeedBR && mSpeedBF)
            mSpeedPeriod = (mSpeedAR + mSpeedAF + mSpeedBR + mSpeedBF) / 4;
    }
    else
    {
        mSpeedAR = 0;
        mSpeedAF = 0;
        mSpeedBR = 0;
        mSpeedBF = 0;
        mSpeedPeriod = 0;
    }
        
    oldA = A;
    oldB = B;
    
//    if (mSpeedPeriod)      
//        mSpeedPeriod = (3*mSpeedPeriod + speedPeriod) / 4;
//    else
//        mSpeedPeriod = speedPeriod;
    
    if (pos > mPulses)
        mPosition = pos - mPulses;
    else if (pos < 0)
        mPosition = pos + mPulses;
    else
        mPosition = pos;
}

int SpeedEncoder::speed() const
{
    if (!mSpeedPeriod)
        return 0;
    return 600 * ((int)inputClk() / mSpeedPeriod) / (mPulses>>1);
}