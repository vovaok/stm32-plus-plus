#include "picontroller.h"

PiController::PiController(const float &valueBinding, SaturationBehavior onSat) :
    mValuePtr(&valueBinding), mTargetValue(0),
    mLoopIn(0), mLoopOut(0),
    erri(0), erriLimit(0),
    mOnSat(onSat),
    kp(0), ki(0),
    limit(0), slope(0),
    enabled(0)
{
}

float PiController::loop(float target)
{
    mLoopIn = target;
    float value = *mValuePtr;
    
    if (enabled)
    {
        if ((mLoopIn - mTargetValue) > slope)
            mTargetValue += slope;
        else if ((mLoopIn - mTargetValue) < -slope)
            mTargetValue -= slope;
        else 
            mTargetValue = mLoopIn;
      
        float err = (mTargetValue - value);
        erri += err;
        erriLimit = ki? limit / ki: 0;
        erri = erri>erriLimit? erriLimit: erri<-erriLimit? -erriLimit: erri;
        
        float u = err * kp + erri * ki;
        
        if (u > limit || u < -limit)
        {
            switch (mOnSat)
            {
              case ResetIntegratorOnSat:
                erri = 0;
                break;
              case LimitIntegratorOnSat:
                erri -= err;
                break;
              default: ;
            }
        }

        mLoopOut = u>limit? limit: u<-limit? -limit: u;
    }
    else
    {
        erri = 0;
        mTargetValue = 0;
        mLoopOut = mLoopIn;
    }
    
    return mLoopOut;
}