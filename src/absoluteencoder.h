#ifndef _ABSOLUTEENCODER_H
#define _ABSOLUTEENCODER_H

#include <stdint.h>

template <typename T>
class AbsoluteEncoderT
{
public:
    const T &value() const {return m_value;}
    T read()
    {
        m_value = readValue() << (sizeof(T)*8 - m_bits);
        m_valid = true;
        return m_value;
    }

    int bits() const {return m_bits;}
    T pulses() const {return (1 << m_bits);}
    T mask() const {return (1 << m_bits) - 1;}

    bool isValid() const {return m_valid;}

protected:
    int m_bits;

    AbsoluteEncoderT(int bits) :
        m_bits(bits),
        m_value(0)
    {
    }

    virtual T readValue() = 0;

private:
    T m_value;
    bool m_valid = false;
};

typedef AbsoluteEncoderT<uint16_t> AbsoluteEncoder16;
typedef AbsoluteEncoderT<uint32_t> AbsoluteEncoder;

#endif