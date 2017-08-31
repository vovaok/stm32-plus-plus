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
    } VoltageEntry;
  
    Adc *mAdc;
    float mTemperature;
    float mVbat;
    map<string, VoltageEntry> mVoltages;
    
    void onTimer();
  
public:
    PowerManager(Adc *adc);
    
    void addVoltageMeasurement(string name, Adc::Channel channel, float Rhigh, float Rlow);
    
    inline float temperature() const {return mTemperature;}
    inline float batteryVoltage() const {return mVbat;}
    float voltage(string name) {return mVoltages[name].value;}
};

#endif