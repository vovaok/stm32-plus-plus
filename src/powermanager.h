#ifndef _POWER_MANAGER_H
#define _POWER_MANAGER_H

#include "adc.h"
#include "timer.h"
#include <string>
#include <map>

using namespace std;

class PowerManager
{
private:
    typedef struct
    {
        Adc::Channel channel;
        float factor;
        float value;
        float Kf;
    } VoltageEntry;
  
    Adc *mAdc;
    float mTemperature;
    float mVbat;
    map<string, VoltageEntry> mVoltages;
    
    void onTimer();
  
public:
    PowerManager(Adc *adc);
    
    void addVoltageMeasurement(string name, Adc::Channel channel, float Rhigh, float Rlow);
    void setFilter(string name, float Kf);
    
    inline const float &temperature() const {return mTemperature;}
    inline const float &batteryVoltage() const {return mVbat;}
    const float &voltage(string name) {return mVoltages[name].value;}
};

#endif