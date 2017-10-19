#include "motorDC.h"
#define sat(x, c) (((x)>(c))? (c): ((x)<-(c))? -(c): (x))


MotorDC::MotorDC(PwmOutput *pwmIn) : slopePwm(0),mEnable(false),kpPos(0), kiPos(0), sSCur(0), kpCur(0),kiCur(0),
 mCurrent(0),mPosition(0),mSpeed(0),currentSlope(0.05),slopeValuePwm(1000),mPwm(0),sSPos(0),mEnableCurrentLoop(false),
 mCurrentLimit(12),mPwmCurrent(0),mEnableLoops(0),mEnablePositionLoop(0),inControlValue(0),
 mEnableLimitsPosition(false),isLimitF(false),isLimitB(false)
  
 
  {
    pwm =  pwmIn ;
    
    timerPos = new Timer();
    timerPos->setTimeoutEvent(EVENT(&MotorDC::positionLoop));
    timerPos->start(10); // ms 
    
    timerCurrent = new Timer();
    timerCurrent->setTimeoutEvent(EVENT(&MotorDC::currentLoop));
    timerCurrent->start(1); // ms 
    
    sSCur = (kiCur)? (((long)255*65535)/kiCur): 0;
    
  }

  
  
  void MotorDC::setPositionValue(float value)
  {
    mPosition = value;
    if(mEnableLimitsPosition)
    {
      if(value>maxPos)
        isLimitF = true;
      else if(value<(maxPos-40))
        isLimitF = false;
      
      if(value<minPos)
        isLimitB = true;
      else if(value>minPos+40)
        isLimitB = false;
    }
    else
    {
        isLimitF = false;
        isLimitB = false;
    }
  }
  
  
  void MotorDC::positionLoop()
  {
    
    if(mEnableLoops&&mEnablePositionLoop)
    {
        float err = (inControlValue - mPosition);
        erriPos += err;
        erriPos = SAT(erriPos, sSPos);
        mPwm = err * kpPos + erriPos * kiPos;

        if (!mEnableCurrentLoop)
            setSpeed(lrintf(mPwm));
        else
            mPwm = sat(mPwm, mCurrentLimit);
    
    }
   
    
    
  
  }
  
  void MotorDC::currentLoop()
  {
  
    if(mEnableLoops&&mEnableCurrentLoop)
    {
      if(!mEnablePositionLoop)
      mPwm = inControlValue;
        if ((mPwm - mPwmCurrent) > currentSlope)
        {

          mPwmCurrent += currentSlope;
        }
        else if ((mPwm - mPwmCurrent) < -currentSlope)
        {

          mPwmCurrent -= currentSlope;
        }
        else 
          mPwmCurrent = mPwm;
     
     
        float err = (mPwmCurrent - mCurrent);
        erriCur += err;
        erriCur = sat(erriCur, sSCur);
         
        float pwm = (err*kpCur+erriCur*kiCur);
             
        setSpeed(pwm);
    }
  }

 void MotorDC::setPositionLimits(float min,float max)
 {
    minPos=min;
    maxPos=max;
 }
  
  
void MotorDC::SetChanels(ChannelNumber chnum1,Gpio::Config pin1,ChannelNumber chnum2,Gpio::Config pin2)
{

  channelA  = chnum1;
  channelB  = chnum2;
  
  pwm->configChannel(chnum1, pin1, Gpio::NoConfig);
  pwm->configChannel(chnum2, pin2, Gpio::NoConfig);
  
}

void MotorDC::setEnable(bool enable)
{

  mEnable=enable;
  
  pwm->setChannelEnabled(channelA, enable, enable);
  pwm->setChannelEnabled(channelB, enable, enable);
  
  if(!enable)
  {
    slopePwm =0;
    erriCur  =0;
    erriPos  =0;
  }
}



void MotorDC::setSpeed(int speed)
{

  if(abs(speed)>65000)
    erriCur=0;
  speed = sat(speed,65000);
  //разгон торможение*******************
 
  
   if((speed-slopePwm)>slopeValuePwm)
                slopePwm+=slopeValuePwm;
            
            else if((speed-slopePwm)<-slopeValuePwm)
                slopePwm-=slopeValuePwm;
            
            else 
                slopePwm=speed;
  //*************************************
  if(slopePwm>0&&!isLimitF)
  {
   pwm->setDutyCycle(channelA,65535-slopePwm);
   pwm->setDutyCycle(channelB,65535);
  }
  
  else if (slopePwm<0&&!isLimitB)
  {
   pwm->setDutyCycle(channelA,65535);
   pwm->setDutyCycle(channelB,65535+slopePwm);
  }
  
  else
    {
   pwm->setDutyCycle(channelA,65535);
   pwm->setDutyCycle(channelB,65535);
    }
  
}