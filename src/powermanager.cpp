#include "powermanager.h"

PowerManager::PowerManager(Adc *adc) :
    mAdc(adc),
    mTemperature(0),
    mVbat(0)
{
    if (!mAdc)
        mAdc = Adc::instance(1);

    mAdc->setResolution(Adc::Res12bit);
    mAdc->addChannel(Adc::Vbat, Adc::SampleTime_56Cycles);
    mAdc->addChannel(Adc::TempSensor, Adc::SampleTime_56Cycles);
//    mAdc->addChannel(Adc::VrefInt, Adc::SampleTime_56Cycles);

    mAdc->setContinuousMode(true);
    mAdc->setMultisample(8);
    mAdc->start();
    mAdc->startConversion();

    Timer *timer = new Timer();
    timer->onTimeout = EVENT(&PowerManager::onTimer);
    timer->start(10);
}

void PowerManager::addVoltageMeasurement(string name, Gpio::Config pin, float Rhigh, float Rlow)
{
    addMeasurement(name, pin, (Rlow + Rhigh) / Rlow);
}

void PowerManager::addMeasurement(string name, Gpio::Config pin, float factor, float bias)
{
    mAdc->stop();

    Adc::Channel channel = mAdc->addChannel(pin, Adc::SampleTime_56Cycles);
    VoltageEntry entry;
    entry.channel = channel;
    entry.bias = bias;
    entry.factor = 3.3f * factor / mAdc->maxValue();
    entry.rawValue = 0;
    entry.value = 0;
    entry.Kf = 0.9;
    mVoltages[name] = entry;

    mAdc->start();
    mAdc->startConversion();
}

void PowerManager::setFilter(string name, float Kf)
{
    mVoltages[name].Kf = Kf;
}

void PowerManager::onTimer()
{
    float temp = mAdc->result(Adc::TempSensor) * 1320 / mAdc->maxValue() - 278.5f;
    if (!mTemperature)
        mTemperature = temp;
    else
        mTemperature = 0.95f*mTemperature + 0.05f*temp;

//    mVref = mAdc->result(Adc::VrefInt);

    float v = mAdc->result(Adc::Vbat) * (2.0f * 3.3f / mAdc->maxValue());
//    if (!mVbat)
        mVbat = v;
//    else
//        mVbat = 0.9*mVbat + 0.1*v;

    for (auto &pair: mVoltages)
    {
        VoltageEntry &entry = pair.second;
        float v = mAdc->result(entry.channel) * entry.factor + entry.bias;
        entry.rawValue = v;
        if (!entry.value)
            entry.value = v;
        else
            entry.value = entry.Kf * entry.value + (1.0f - entry.Kf) * v;
    }
}