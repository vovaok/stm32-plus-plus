#ifndef _EEPROM24C_H
#define _EEPROM24C_H

#include "i2c.h"
#include "core/device.h"
#include "core/application.h"

//! Класс для работы с внешней памятью EEPROM типа AT24Cxxx.
//! Данный класс является шаблонным, для создания объекта
//! лучше пользоваться предлагаемыми реализациями (см. ниже)
//! Работа с EEPROM аналогична работе с другими устройствами,
//! унаследованными от класса Device:
//! после создания объекта необходимо открыть устройство функцией open()
//! затем, используя функции read() и write(), можно читать и писать данные.
//! Чтобы указать адрес чтения или записи, следует воспользоваться функцией seek().
//! Также, для удобства, реализованы функции прямого чтения и записи
//! по заданному адресу: readFrom() и writeTo(), соответственно.
//! Пример:
//! @code
//!     // создаём объект I2C
//!     I2c *i2c = new I2c(Gpio::I2C1_SDA_PB7, Gpio::I2C1_SCL_PB8);
//!     i2c->setBusClock(400000);
//!     
//!     // создаём объект для работы с 16-кбитным EEPROM
//!     Eeprom24c16 *ee = new Eeprom24c16(i2c);
//!     ee->open();     // открываем устройство
//!     ee->seek(42);   // устанавливаем адрес в памяти EEPROM
//!     ee->write("Hello world"); // пишем туда
//!     ee->seek(48);
//!     ByteArray ba = ee->read(5); // будeт считано "world"
//!     ee->writeTo(45, "p,", 2); // записываем 2 байта по заданному адресу
//!     char buf[12]; // создаём буфер для чтения
//!     ee->readFrom(42, buf, sizeof(buf)); // будет считано "Help, world"
//! @endcode
template<int kbit, typename AddrType>
class Eeprom24c : public Device
{
public:
    //! Конструктор класса.
    //! Необходимый аргумент @param i2c - объект класса I2C, через который будет
    //! происходить обмен данными с микросхемой EEPROM.
    //! Необязательный аргумент @param chipAddress - адрес микросхемы на шине I2C,
    //! его необходимо указывать, если физический адрес
    //! отличается от стандартного 0xA0
    explicit Eeprom24c(I2c *i2c, uint8_t chipAddress=0xA0) :
        m_i2c(i2c),
        m_ptr(0),
        m_end(kbit * 128),
//        m_chunkSize(chunkSize),
        m_chipAddress(chipAddress)
    {
        m_sequential = false;
        m_halfDuplex = false;
    }
    
    //! Открывает устройство
    virtual bool open(OpenMode mode=ReadWrite)
    {
        m_i2c->open();
        return Device::open(mode);
    }
    
    //! Закрывает устройство
    virtual void close()
    {
        m_i2c->close();
        Device::close();
    }
    
    //! Возвращает текущий адрес указателя данных
    virtual uint32_t pos() const {return m_ptr;}
    
    //! Устанавливает текущий адрес указателя данных
    virtual bool seek(uint32_t pos)
    {
        if (pos >= m_end)
            return false;
        m_ptr = pos;
        return true;
    }
    
    //! Возвращает число байт, начиная с текущего адреса до конца памяти
    virtual int bytesAvailable() const
    {
        return m_end - m_ptr;
    }
    
    //! Функция прямого чтения по заданному адресу
    //! @param address - адрес в памяти EEPROM;
    //! @param data - указатель на буфер, в который будет произведено чтение
    //! (выделение памяти под буфер - ответственность разработчика);
    //! @param size - размер считываемых данных в байтах.
    //! @return Возвращает размер реально считанных данных.
    int readFrom(uint32_t address, char *data, int size)
    {
        seek(address);
        return Device::read(data, size);
    }
    
    //! Функция прямой записи по заданному адресу
    //! @param address - адрес в памяти EEPROM;
    //! @param data - указатель на буфер, который нужно записать;
    //! @param size - размер записываемых данных в байтах.
    //! @return Возвращает размер реально записанных данных.
    int writeTo(uint32_t address, const char *data, int size)
    {
        seek(address);
        return Device::write(data, size);
    }
    
protected:
    // Реализация функции чтения данных
    // В отличие от записи, читать можно всё подряд, необязательно по блокам
    virtual int readData(char *data, int size)
    {
        // Если читать не надо, то и не читаем
        if (!size)
            return false;
    
        char *dst = data;
        uint8_t page = m_ptr >> m_addrSize; // рассчитываем текущую страницу
        AddrType wordAddress = m_ptr;// & m_addrMask; // рассчитываем адрес внутри этой страницы;
        
        bool r;
        // сначала пишем в микросхему адрес, с которого хотим считать
        uint8_t deviceAddress = m_chipAddress | (page << 1);
        r = m_i2c->writeRegAddr(deviceAddress, wordAddress, sizeof(AddrType));
        if (!r) // если запись адреса прошла неудачно, возвращаем ошибку
            return -1;
        
        // читаем (size) байт из EEPROM
        r = m_i2c->read(deviceAddress, reinterpret_cast<uint8_t*>(dst), size);
        if (r)
            m_ptr += size; // НО ЭТО НЕ ТОЧНО! если во время чтения будет ошибка, то не факт, не факт
        
        if (!r)        // если во время чтения что-то пошло не так
            return -1; // возвращаем ошибку
        return size; // а если всё ок, возвращаем размер считанных данных
    }
    
    // Реализация функции записи данных
    // Согласно даташиту, писать нужно по блокам
    virtual int writeData(const char *data, int size)
    {
        int sz;
        uint8_t page;
        AddrType addr;
        const char *src = data;
        
        // сразу проверяем, есть ли смысл что-то делать дальше
        if (!size)
            return 0;
        
        do
        {
            page = m_ptr >> m_addrSize; // рассчитываем текущую страницу
            addr = m_ptr;// & m_addrMask; // рассчитываем адрес внутри этой страницы
            // рассчитываем размер блока...
            uint32_t maxsz = m_chunkSize - (addr & (m_chunkSize - 1));
            if (size < maxsz)
                sz = size; 
            else
                sz = maxsz;
            
            // пишем блок...
            // при этом стоит подождать, пока данные реально запишутся.
            int retries = 10;
            while (retries--)
            {
                if (writeChunk(page, addr, src, sz))
                    break;
                stmApp()->delay(1);
            }
            // если не записалось за 10 мс, значит действительно всё плохо...
            if (!retries)
                break;
            m_ptr += sz; // текущий адрес увеличивается на размер записанных данных
            src += sz;   // также двигаем буфер
            size -= sz;  // и уменьшаем оставшийся размер
        } while (size);  // и так до тех пор, пока совсем ничего не останется... 
        
        return src - data; // возвращаем размер записанных данных (если он вообще кому-то нужен)
    }
    
private:
    I2c *m_i2c;
    uint32_t m_ptr; // текущий адрес в EEPROM
    const uint32_t m_end; // размер EEPROM
     // максимальный размер блока при записи
    static constexpr uint32_t m_chunkSize = kbit<4?8:kbit<16?16:kbit<128?32:kbit<512?64:kbit<1024?128:256;
    static constexpr int m_addrSize = 8 * sizeof(AddrType); // размер адреса (8 или 16 бит)
//    static constexpr AddrType m_addrMask = (1 << m_addrSize) - 1; // маска адреса (0xFF или 0xFFFF)
    uint8_t m_chipAddress; // адрес устройства на шине I2C
    bool m_busy = false;
    
    // Функция записи блока в EEPROM, реализует собственно запись, согласно даташиту:
    // START -> DEVICE_ADDRESS(R/W=0) -> WORD_ADDRESS -> DATA(n) -> DATA(n+1) ... -> DATA(n+x) -> STOP
    // При этом номер страницы задаётся в младших битах адреса устройства.
    bool writeChunk(uint8_t page, AddrType wordAddress, const char *data, int size)
    {
        bool r;
        uint8_t deviceAddress = m_chipAddress | (page << 1);
        r = m_i2c->writeRegAddr(deviceAddress, wordAddress, sizeof(AddrType));
        if (r)
            r = m_i2c->write(deviceAddress, reinterpret_cast<const uint8_t *>(data), size);
        return r;
    }    
};

// Реализация шаблонов для различных вариантов микросхем
typedef Eeprom24c<1, uint8_t>     Eeprom24c01;
typedef Eeprom24c<2, uint8_t>     Eeprom24c02;
typedef Eeprom24c<4, uint8_t>     Eeprom24c04;
typedef Eeprom24c<8, uint8_t>     Eeprom24c08;
typedef Eeprom24c<16, uint8_t>    Eeprom24c16;
typedef Eeprom24c<32, uint16_t>   Eeprom24c32;
typedef Eeprom24c<64, uint16_t>   Eeprom24c64;
typedef Eeprom24c<128, uint16_t>  Eeprom24c128;
typedef Eeprom24c<256, uint16_t>  Eeprom24c256;
typedef Eeprom24c<512, uint16_t>  Eeprom24c512;
typedef Eeprom24c<1024, uint16_t> Eeprom24c1024;

#endif