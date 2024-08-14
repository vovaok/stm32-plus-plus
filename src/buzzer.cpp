#include "buzzer.h"
#include "core/application.h"

unsigned short Buzzer::mNoteFreqs[12] = {8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544, 13290, 14080, 14917, 15804};

Buzzer::Buzzer(Gpio::Config pin) :
    mTime(0),
    mEndFlag(false),
    m_isPlaying(false),
    mPlayBuf(0L),
    mPlaySize(0),
    mPlayCnt(0)
{
    HardwareTimer::TimerNumber tim = HardwareTimer::getTimerByPin(pin);
    mChan = HardwareTimer::getChannelByPin(pin);

    if (tim == HardwareTimer::TimNone)
        THROW(Exception::InvalidPeriph);
    if (mChan == HardwareTimer::ChNone)
        THROW(Exception::InvalidPin);

    mPwm = new PwmOutput(tim, 1000);
    //mPwm->configChannel(mChan, pin);
    mPwm->configChannelToggleMode(pin);
//    mPwm->setDutyCycle(mChan, 32768);
    mPwm->start();

    stmApp()->registerTaskEvent(EVENT(&Buzzer::task));
    stmApp()->registerTickEvent(EVENT(&Buzzer::tick));
}

void Buzzer::task()
{
    if (mEndFlag)
    {
        mEndFlag = false;
        off();
        if (onBeepDone)
            onBeepDone();
        if (mPlayCnt < mPlaySize)
            beep(mPlayBuf[mPlayCnt++]);
        else if (mPlayCnt && onPlayDone)
        {
            m_isPlaying = false;
            onPlayDone();
        }
    }
}

void Buzzer::tick(int dt)
{
    if (mTime > dt)
    {
        mTime -= dt;
        if (mTime < 25)
            off();
    }
    else if (mTime)
    {
        mTime = 0;
        mEndFlag = true;
    }
}

//void Buzzer::setVolume(unsigned char volume_percent)
//{
//    mPwm->setDutyCycle(mChan, ((unsigned long)volume_percent << 15) / 100); // 50% = max volume
//}

void Buzzer::setFrequency(int f_Hz)
{
    mPwm->setFrequency(f_Hz * 4);
}

void Buzzer::setEnabled(bool enabled)
{
    mPwm->setChannelEnabled(mChan, enabled);
}

void Buzzer::beep(int duration_ms)
{
    if (mTime)
        mEndFlag = true;
    mTime = duration_ms;
    on();
}

void Buzzer::beep(unsigned char note, int duration_ms)
{
    setFrequency(getFreqByNote(note));
    beep(duration_ms);
}

void Buzzer::beep(Note note)
{
    setFrequency(getFreqByNote(note.note));
    beep(note.duration_ms);
}

int Buzzer::getFreqByNote(unsigned char note)
{
    int octave1 = (note / 12);
    note -= octave1 * 12;
    return mNoteFreqs[note] >> (10 - octave1);
}
//---------------------------------------------------------------------------

void Buzzer::play(Note *buffer, int count)
{
    mPlayBuf = buffer;
    mPlaySize = count;
    mPlayCnt = 0;
    if (mPlaySize)
    {
        m_isPlaying = true;
        beep(mPlayBuf[mPlayCnt++]);
    }
}

void Buzzer::stop()
{
    mPlayBuf = 0L;
    mPlaySize = 0;
    mPlayCnt = 0;
    m_isPlaying = false;
}
//---------------------------------------------------------------------------
