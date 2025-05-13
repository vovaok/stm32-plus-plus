#include "ds18b20.h"

DS18B20::DS18B20(Usart *usart) :
    m_usart(usart)
{
    m_usart->setConfig(Usart::Mode8N1);
    m_usart->setUseDmaTx(true);
    m_usart->setUseDmaRx(true);
    m_usart->onBytesWritten = EVENT(&DS18B20::onTransferComplete);
    m_usart->open();
    
    m_timer = new Timer();
    m_timer->onTimeout = EVENT(&DS18B20::update);
    m_timer->start(500);
}

void DS18B20::setUpdateInterval(int value_ms)
{
    m_timer->setInterval(value_ms);
}

void DS18B20::update()
{
    if (m_fsm == Idle)
    {
        m_fsm = InitConvert;
        initTransfer();
    }
    else if (m_fsm == WaitConvert) // stuck at conversion
    {
        m_fsm = Idle;
        m_presence = false;
    }
}

void DS18B20::onTransferComplete()
{
    char buf[16];
    
    switch (m_fsm)
    {
    case InitConvert:
        checkPresence();
        // Skip ROM, Convert T
        write({0xCC, 0x44});
        m_fsm = Convert;
        break;
        
    case InitRead:
        checkPresence();
        // Skip ROM, Read Scratchpad, issue 16 read slots
        write({0xCC, 0xBE, 0xFF, 0xFF});
        m_fsm = Read;
        break;
        
    case Convert:
        m_usart->read(buf, 16); // dummy read
        buf[0] = 0xFF;
        m_usart->write(buf, 1); // issue read slot
        m_fsm = WaitConvert;
        break;
        
    case WaitConvert:
        m_usart->read(buf, 1);
        if (buf[0] & 1) // conversion completed
        {
            m_fsm = InitRead;
            initTransfer();
        }
        break;
        
    case Read:
        m_usart->read(buf, 16); // dummy read
        m_usart->read(buf, 16); // read result;
        readResult(buf);
        m_fsm = Idle;
        break;
        
    default:
        m_usart->readAll(); // dummy read trash if it is 
        break;
    }
}

void DS18B20::initTransfer()
{    
    // Задаем значение сброса (reset pulse) в соответствии с протоколом DS18B20
    char tmp = 0xF0;
    
    //! @todo Сделать норм получение тактовой частоты уарта в РЦЦ

    // Устанавливаем скорость передачи данных UART (удвоенная скорость для работы с DS18B20)
	m_usart->setBaudrate(9600 * 2);              
    // Отправка сигнала сброса
    m_usart->write(&tmp, 1); 
}

void DS18B20::checkPresence()
{
    char tmp = 0xF0;
    m_usart->read(&tmp, 1);
    m_presence = (tmp != 0xF0);
    
    //! @todo Убрать * 2 после допиливания РЦЦ
    m_usart->setBaudrate(115200 * 2);
}

void DS18B20::write(const ByteArray &ba)
{
    ByteArray buf;
    for (int i=0; i<ba.size(); i++)
    {
        uint8_t b = ba[i];
        for (int j=0; j<8; j++)
        {
            if (b & (1 << j))
                buf.append(0xFF);
            else
                buf.append('\0');
        }
    }
    
    m_usart->write(buf);
}

void DS18B20::readResult(char *buf)
{
    int16_t v = 0;
    for (int i=0; i<16; i++)
    {
        if (buf[i] & 1)
            v |= (1 << i);
    }
    m_value = v;
    m_temp = m_value / 16.f;
}
