#include "buzzer.h"
#include "core/application.h"

Buzzer::Buzzer(Gpio::Config pin) :
    PwmPin(pin)
{
    setMode(HardwareTimer::PwmMode_Toggle);

    stmApp()->registerTaskEvent(EVENT(&Buzzer::task));
    stmApp()->registerTickEvent(EVENT(&Buzzer::tick));
}

void Buzzer::task()
{
    if (m_endFlag)
    {
        m_endFlag = false;
        if (onBeepDone)
            onBeepDone();
    }
}

void Buzzer::tick(int dt)
{
    if (m_time > dt)
    {
        m_time -= dt;
    }
    else if (m_time)
    {
        m_time = 0;
        stop();
        m_endFlag = true;
    }
}

//void Buzzer::setVolume(unsigned char volume_percent)
//{
//    mPwm->setDutyCycle(mChan, ((unsigned long)volume_percent << 15) / 100); // 50% = max volume
//}

void Buzzer::setFrequency(int f_Hz)
{
    device()->setFrequency(f_Hz * 2);
}

void Buzzer::beep(int duration_ms)
{
    m_time = duration_ms;
    on();
}

void Buzzer::stop()
{
    off();
    m_endFlag = true;
}