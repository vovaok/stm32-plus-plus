#pragma once

#include "bytearray.h"

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
    
    int voltage() const {return m_voltage;} // [mV]
    int current() const {return m_current;} // [mA]
    int remainingCapacity() const {return m_remCap;} // [mAh] or [mWh]
    int totalCapacity() const {return m_totCap;} // [mAh] or [mWh]
    int designedCapacity() const {return m_desCap;} // [mAh] or [mWh]
    int8_t percent() const {return m_percent;} // remaining capacity in percents
    State state() const {return m_state;}
    int temperature() const {return m_temperature;} // degrees of Celsium
    bool isPresent() const {return m_present;}
    
    uint8_t cellCount() const {return m_data.size() / 2;}
    uint16_t cellVoltage(int num_cell)
    {
        int idx = num_cell - 1;
        if (idx * 2 < m_data.size())
            return reinterpret_cast<uint16_t *>(m_data.data())[idx];
        return 0;
    }
    
protected:
    int m_voltage = 0;
    int m_current = 0;
    int m_remCap = 0;
    int m_totCap = 0;
    int m_desCap = 0;
    int8_t m_percent = 0;
    State m_state = UnknownState;
    bool m_present = false;
    int m_temperature = 0;
    
    void setCellCount(int value)
    {
        if (value > 0 && value <= 32)
            m_data.resize(value * 2);
    }
    
    void setCellVoltage(uint8_t idx, uint16_t value)
    {
        if (idx * 2 < m_data.size())
            reinterpret_cast<uint16_t *>(m_data.data())[idx] = value;
    }
    
private:
    ByteArray m_data;
};
