#ifndef _MOTORDC_H
#define _MOTORDC_H

#include "pwmout.h"
#include "core/timer.h"


class MotorDC
{
private:
    HardwareTimer::ChannelNumber channelA,channelB;
    PwmOutput *pwm;
    int slopePwm;
    bool mEnable;
    float mCurrent;
    float mPosition;
    float mSpeed;
    float inControlValue;
    float kpPos, kiPos, sSPos,sSCur, kpCur,kiCur;
    float currentSlope;
    int slopeValuePwm;
    float mPwm;
    float erriPos, erriCur;
    float mCurrentLimit;
    float mPwmCurrent;
    float minPos,maxPos;
    int mPwmLimit;
    bool mEnableLimitsPosition;
    
    bool isLimitF, isLimitB;
    
    bool mEnableCurrentLoop, mEnableLoops,mEnablePositionLoop;
    
    Timer *timerPos, *timerCurrent;
  
public:
  
  
    MotorDC(PwmOutput *pwmInq);
    void setSpeed(int speed);
    void SetChanels(Gpio::Config pin1, Gpio::Config pin2);
    void setEnable(bool enable);
    bool isEnable() {return mEnable;}
    void setCurrentValue(float current) {mCurrent  = current;}
    void setPositionValue(float value);
    void setEnableLoops(bool enable) {mEnableLoops = enable;}
    void setEnableCurrentLoop(bool enable) {mEnableCurrentLoop = enable;}
    void setEnablePositionLoop(bool enable) {mEnablePositionLoop = enable;}
    void setCurrentSlopeValue(float value) {currentSlope = value;}
    void setSlopePwmValue (int value) {slopeValuePwm = value;}
    void setCurrentLimitValue(float value) {mCurrentLimit = value;}
    void setControlValue(float value) {inControlValue = value;}
    void setPwmLimit(int value) {mPwmLimit = value;}
    
    void setPositionLimits(float min,float max);
    void setPositionMinLimit(float min) {minPos = min;}
    void setPositionMaxLimit(float max) {maxPos = max;}
    void setEnablePositionLimits(bool enable) {mEnableLimitsPosition = enable; if (!enable) {isLimitF = false; isLimitB = false;}}
    
    //kp///ki
    void setKpPos(float value) {kpPos = value;}
    void setKiPos(float value) {kiPos = value;}
    
    void setKpCur(float value) {kpCur = value;}
    void setKiCur(float value) {kiCur = value; sSCur = (kiCur)? (((long)255*65535)/kiCur): 0; }
    
    
    
    void positionLoop();
    void currentLoop();
  
};







#endif