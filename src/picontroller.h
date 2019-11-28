#ifndef _PICONTROLLER_H
#define _PICONTROLLER_H

class PiController
{
public:
    enum SaturationBehavior {NoActionOnSat, ResetIntegratorOnSat, LimitIntegratorOnSat};
    
private:
    const float *mValuePtr;
    float mTargetValue;
    float mLoopIn;
    float mLoopOut;
    float erri;
    float erriLimit;
    SaturationBehavior mOnSat;
    
public:
    float kp;
    float ki;
    float limit; // output saturation
    float slope; // input rate limit
    bool enabled;
  
public:
    PiController(const float &valueBinding, SaturationBehavior onSat = NoActionOnSat);
    
    float loop(float target);
};

#endif