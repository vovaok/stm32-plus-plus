#ifndef _ABSOLUTEENCODER_H
#define _ABSOLUTEENCODER_H

#include <stdint.h>

class AbsoluteEncoder
{
public:
    const uint32_t &value() const {return m_value;}
    uint32_t read()
    {
        m_value = readValue() << (32 - m_bits);
        return m_value;
    }
    
    int bits() const {return m_bits;}
    uint32_t pulses() const {return (1 << m_bits);}
    uint32_t mask() const {return (1 << m_bits) - 1;}
  
protected:
    int m_bits;
    
    AbsoluteEncoder(int bits) :
        m_bits(bits),
        m_value(0)
    {
    }
    
    virtual uint32_t readValue() = 0;
    
private:
    uint32_t m_value;
};

#endif