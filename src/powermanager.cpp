#include "powermanager.h"

PowerManager::PowerManager(Adc *adc) :
    mAdc(adc),
    mTemperature(0),
    mVbat(0)
{
    mAdc->addChannel(Adc::Vbat, Adc::SampleTime_56Cycles);
    mAdc->addChannel(Adc::TempSensor, Adc::SampleTime_56Cycles);
  
    Timer *timer = new Timer();
    timer->setTimeoutEvent(EVENT(&PowerManager::onTimer));
    timer->start(10);
}

void PowerManager::addVoltageMeasurement(string name, Adc::Channel channel, float Rhigh, float Rlow)
{
    mAdc->addChannel(channel, Adc::SampleTime_56Cycles);
    VoltageEntry entry;
    entry.channel = channel;
    entry.factor = (3.3f * (Rlow + Rhigh) / Rlow / 4095.0f);
    entry.value = 0;
    mVoltages[name] = entry;
}

void PowerManager::onTimer()
{
    float temp = (mAdc->result(Adc::TempSensor) - 864) * 1320 / 4095.0f;
    if (!mTemperature)
        mTemperature = temp;
    else
        mTemperature = 0.95*mTemperature + 0.05*temp;
    
    float v = mAdc->result(Adc::Vbat) * (2.0f * 3.3f / 4095.0f);
    mVbat = 0.9*mVbat + 0.1*v;
    
    for (map<string, VoltageEntry>::iterator it = mVoltages.begin(); it != mVoltages.end(); ++it)
    {
        VoltageEntry &entry = it->second;
        float v = mAdc->result(entry.channel) * entry.factor;
        entry.value = 0.9*entry.value + 0.1*v;
    }
}