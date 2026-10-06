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
        float zeroOffset; // Volts
        float factor; // [out]/Volts
        float value;
        float rawValue;
        float Kf;
    } VoltageEntry;

    Adc *mAdc;
    float mTemperature;
    float mVbat;
//    float mVref;
    map<string, VoltageEntry> mVoltages;
    
    VoltageEntry *mVbus = nullptr; // bus voltage
    VoltageEntry *mIbus = nullptr; // bus current
    
    int64_t m_consumptionCounter = 0;
//    int32_t m_consumption_frac = 0;
//    int32_t m_consumption_mWh = 0;

    Timer *timer;
    void onTimer();

public:
    PowerManager(Adc *adc=nullptr);
    void setUpdateInterval(int value_ms);
    inline int updateInterval() const {return timer->interval();}
    
    //! Configure the bus voltage measurement
    //! This creates measurement channel with name "Vbus"
    //! @arg pin - pin config of ADC channel
    //! @arg Rhigh - value of the upper resistor [Ohms]
    //! @arg Rlow - value of the lower resistor [Ohms]
    void addVbus(Gpio::Config pin, float Rhigh, float Rlow);
    
    //! Configure the bus current measurement
    //! This creates measurement channel with name "Ibus"
    //! @arg pin - pin config of ADC channel
    //! @arg sensitivity - the current sensor sensitivity [mV/A]
    //! @arg zeroOffset - voltage level of zero current [V]
    void addIbus(Gpio::Config pin, float sensitivity, float zeroOffset);
    
    //! Configure custom voltage measurement
    //! @arg name - arbitrary name of the signal
    //! @arg pin - pin config of ADC channel
    //! @arg Rhigh - value of the upper resistor [Ohms]
    //! @arg Rlow - value of the lower resistor [Ohms], must be non-zero
    void addVoltageMeasurement(string name, Gpio::Config pin, float Rhigh, float Rlow);
    
    //! Configure custom measurement
    //! @arg name - arbitrary name of the signal
    //! @arg pin - pin config of ADC channel
    //! @arg factor - gain factor of the signal [unit/V]
    //! @arg zeroOffset - voltage level of signal zero offset [V]
    void addMeasurement(string name, Gpio::Config pin, float factor = 1.f, float zeroOffset = 0);
    
    //! Override default filter settings
    //! @arg name - name of the measurement channel
    //! @arg Kf - filter coefficient, must be in the interval (0; 1].
    //! 1 means no filtering; the lower Kf, the stronger the filtering. 0.1 by default
    void setFilter(string name, float Kf);
    
    //! Momentary bus voltage [V]
    float Vbus() const;
    
    //! Momentary bus current [A]
    float Ibus() const;
    
    //! Momentary consumed power [W]
    float power() const;
    
    //! Overall energy consumption [Wh]
    float consumption() const;
    
    //! Initialize consumption counter to a given value [Wh]
    //! Intended for restore last counter value after reboot
    //! Saving and restoring the value is the external job.
    void initConsumption(float value);
    
    //! Obtain temperature from internal sensor
    inline const float &temperature() const {return mTemperature;}
    
//    inline const float &referenceVoltage() const {return mVref;}
    
    //! Obtain the CMOS battrery voltage from internal circuit
    inline const float &batteryVoltage() const {return mVbat;}
    
    //! Get the value of a certain measurement channel
    const float &value(string name) {return mVoltages[name].value;}
    
    //! Get the unfiltered value of a certain measurement channel
    const float &rawValue(string name) {return mVoltages[name].rawValue;}
};

#endif