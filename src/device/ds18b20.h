#pragma once

#include "usart.h"
#include "core/timer.h"

class DS18B20
{
public:
    DS18B20(Usart *usart);
    
    void setUpdateInterval(int value_ms);
    
    bool isPresent() const {return m_presence;}
    int value() const {return m_value;}
    float temperature() const {return m_temp;}
    
    NotifyEvent onReadyRead;
    
private:
    Usart *m_usart = nullptr;
    Timer *m_timer = nullptr;
    int16_t m_value = 0;
    float m_temp = 0;
    bool m_presence = false;
    
    enum FSM {Idle, InitConvert, Convert, WaitConvert, InitRead, Read};
    FSM m_fsm = Idle;
    
    void update();
    void onTransferComplete();
    
    void initTransfer();
    void checkPresence();
    void write(const ByteArray &ba);
    void readResult(char *buf); // assume buf length is 16
};
