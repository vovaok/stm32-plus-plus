#include "analogservo.h"

Servo::Servo(PinName pin, int frequency_Hz) :
    Gpio(pin, Gpio::pullUp),
    mFreq(frequency_Hz),
    mValue(128),
    mMin(0),
    mMax(255),
    mEnabled(false)
{
}
   
void Servo::setEnabled(bool enabled)
{
    if (enabled && !mEnabled)
        setAsOutput();
    else if (!enabled && mEnabled)
        setAsInputPullUp();
    mEnabled = enabled;
}

void Servo::setRange(int min, int max)
{
    mMin = min<0? 0: min>255? 255: min;
    mMax = max<0? 0: max>255? 255: max;
    setValue(mValue); // fit to limits 
}

void Servo::setPosition(int pos)
{
    mPosition = pos<0? 0: pos>255? 255: pos;
    mValue = ((mPosition * (mMax - mMin)) >> 8) + mMin;
}

void Servo::setValue(int value)
{
    mValue = value<mMin? mMin: value>mMax? mMax: value;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

AnalogServo::AnalogServo(TimerNumber timerNumber) :
    HardwareTimer(timerNumber, 4096*50)
{
    setUpdateEvent(EVENT(&AnalogServo::timerHandler));
    HardwareTimer::setEnabled(true);
}
  
AnalogServo::~AnalogServo()
{
    
}

void AnalogServo::timerHandler()
{
    //GPIOD->BSRRL = 1<<1;
    mTime = (mTime + 1) & 0x0FFF; // 12 bit PWM
    int size = mServo.size();
    for (int i=0; i<size; i++)
    {
        Servo *servo = mServo[i];
        if (!mTime)
            servo->set();
        if (mTime == servo->mValue + 180)
            servo->reset();
//        bool pinstate = mTime < (servo->mValue + 180);
//        servo->write(pinstate);
    }
    //GPIOD->BSRRH = 1<<1;
}
//---------------------------------------------------------------------------

void AnalogServo::setEnabled(bool enabled)
{
    for (int i=0; i<mServo.size(); i++)
        mServo[i]->setEnabled(enabled);
}

Servo *AnalogServo::addServo(Gpio::PinName pin)
{
    Servo *servo = new Servo(pin);
    mServo.push_back(servo);
    return servo;
}
//---------------------------------------------------------------------------