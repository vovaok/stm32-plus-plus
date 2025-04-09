#pragma once

//! Abstract base class for BMS drivers
class Battery
{
public:
    enum State
    {
        UnknownState,
        DischargingState,
        ChargingState,
        // .. something else state
    };
    
    const int &voltage() const {return m_voltage;} // [mV]
    const int &current() const {return m_current;} // [mA]
    int remainingCapacity() const {return m_remCap;} // [mAh] or [mWh]
    int totalCapacity() const {return m_totCap;} // [mAh] or [mWh]
    int designedCapacity() const {return m_desCap;} // [mAh] or [mWh]
    const uint8_t &percent() const {return m_percent;} // remaining capacity in percents
    State state() const {return m_state;}
    float temperature() const {return m_temperature;} // degrees of Celsium
    bool isPresent() const {return m_present;}
    
    virtual uint16_t cellVoltage(int num_cell) {return 0;}
    
protected:
    int m_voltage = 0;
    int m_current = 0;
    int m_remCap = 0;
    int m_totCap = 0;
    int m_desCap = 0;
    uint8_t m_percent = 0;
    State m_state = UnknownState;
    bool m_present = false;
    float m_temperature = 0;
};
