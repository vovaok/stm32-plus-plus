#include "usartframed.h"

UsartFramed::UsartFramed(Gpio::Config pinTx, Gpio::Config pinRx) :
    Usart(pinTx, pinRx)
{
    setCharacterMatchEvent('}');
    m_sequential = false;
}
    
//bool UsartFramed::fillBuffer(const char *data, int size)
//{
//    /// @todo check buffer capacity! can crash
//
//    uint8_t cs = 0;
//    char *begin = mTxBuffer.data();
//    char *dst = begin + mTxPos;
//    char *end = begin + mTxBuffer.size();
//    *dst++ = '{';
//    if (dst >= end)
//        dst = begin;
//    size++;
//    while (size--)
//    {
//        char b = *data++;
//        
//        if (size)
//            cs -= b;
//        else
//            b = cs;
//            
//        switch (b)
//        {
//        case '{':
//        case '}':
//        case '\\':
//            *dst++ = '\\';
//            b ^= 0x20;
//            if (dst >= end)
//                dst = begin;
//        }
//        *dst++ = b;
//        if (dst >= end)
//            dst = begin;
//    }
//    *dst++ = '}';
//    if (dst >= end)
//        dst = begin;
//    mTxPos = dst - mTxBuffer.data();
//    return true;
//}

int UsartFramed::writeData(const char *data, int size)
{
    uint8_t cs = 0;
    if (Usart::writeBuffer("{", 1) <= 0)
        return 0;
    
    int sz = size + 1;
    while (sz--)
    {
        char b = *data++;
        
        if (sz)
            cs -= b;
        else
            b = cs;
            
        switch (b)
        {
        case '{':
        case '}':
        case '\\':
            if (Usart::writeBuffer("\\", 1) <= 0)
                return 0;
            b ^= 0x20;
        }
        if (Usart::writeBuffer(&b, 1) <= 0)
            return 0;
    }
    if (Usart::writeData("}", 1) <= 0)
        return 0;
    return size;
}

int UsartFramed::readData(char *data, int size)
{
    int avail = bytesAvailable();
    int sz = 0;
    uint8_t cs = 0;
    const char *begin = mRxBuffer.data();
    const char *src = begin + mRxPos;
    const char *end = begin + mRxBuffer.size();
    char *dst = data;
    
    while (avail--)
    {
        if (*src == '{')
            break;
        if (++src >= end)
            src = begin;
    }
    
    mRxPos = src - begin;
    if (avail <= 0)
        return 0;
    
    if (++src >= end)
        src = begin;
    
    while (avail--)
    {
        char b = *src++;
        if (src >= end)
            src = begin;
        
        if (b == '\\')
        {
            b = *src++ ^ 0x20;
            if (src >= end)
                src = begin;
        }
        else if (b == '}')
        {
            mRxPos = src - begin;
            if (!cs)
                return sz - 1;
            break;
        }
        else if (b == '{') // fail restart
        {
            mRxPos = src - begin;
            sz = 0;
            cs = 0;
            dst = data;
            continue;
        }
            
        if (sz < size)
            *dst++ = b;
        cs += b;
        sz++;
    }
    
    return 0;
}