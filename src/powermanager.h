#ifndef _POWER_MANAGER_H
#define _POWER_MANAGER_H

#include "adc.h"
#include "core/timer.h"
#include <string>
#include <map>

using namespace std;

class PowerManager
{
private:
    typedef struct
    {
        Adc::Channel channel;
        float bias;
        float factor;
        float rawValue; // unfiltered
        float value;
        float Kf;
    } VoltageEntry;

    Adc *mAdc;
    float mTemperature;
    float mVbat;
//    float mVref;
    map<string, VoltageEntry> mVoltages;

    Timer *timer;
    void onTimer();

public:
    PowerManager(Adc *adc=nullptr);
    void setUpdateInterval(int value_ms);
    int updateInterval() const {return timer->interval();}
    void addVoltageMeasurement(string name, Gpio::Config pin, float Rhigh, float Rlow);
    void addMeasurement(string name, Gpio::Config pin, float factor = 1.f, float bias = 0);
    void setFilter(string name, float Kf);

    inline const float &temperature() const {return mTemperature;}
//    inline const float &referenceVoltage() const {return mVref;}
    inline const float &batteryVoltage() const {return mVbat;}
    const float &voltage(string name) {return mVoltages[name].value;}
    const float &rawVoltage(string name) {return mVoltages[name].rawValue;}
};

#endif