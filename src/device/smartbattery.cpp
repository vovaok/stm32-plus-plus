#include "smartbattery.h"
#include "core/application.h"

SmartBattery::SmartBattery(I2c *i2c)
{
    i2c->setBusClock(100000);
    i2c->setSMBusHostMode();
    i2c->open();
    m_dev = i2c->createDevice(0x16); // 0x16 = SMBus SBS class
}

uint16_t SmartBattery::readWord(SBS cmd)
{
    uint16_t v = 0;
    m_valid = m_dev->readReg((uint8_t)cmd, reinterpret_cast<uint8_t *>(&v), 2);
    return v;
}

uint16_t SmartBattery::readSys(SystemData cmd)
{
    writeWord(SBS::ManufacturerAccess, cmd);
    return readWord(SBS::ManufacturerAccess);
}

ByteArray SmartBattery::readBlock(SBS cmd)
{
    uint8_t buf[32] = {0};
    m_valid = m_dev->readReg((uint8_t)cmd, buf, 1);
    if (m_valid)
        m_valid = m_dev->readReg((uint8_t)cmd, buf, buf[0] + 1);
    if (buf[0])
        return ByteArray(buf + 1, buf[0]);
    return ByteArray();
}

void SmartBattery::writeWord(SBS cmd, uint16_t word)
{
    m_valid = m_dev->writeReg((uint8_t)cmd, reinterpret_cast<const uint8_t *>(&word), 2);
}

void SmartBattery::enterKey(uint32_t key)
{
    writeWord(SBS::ManufacturerAccess, key >> 16);
    writeWord(SBS::ManufacturerAccess, key & 0xFFFF);
}

void SmartBattery::sendControl(SystemControl cmd)
{
    writeWord(SBS::ManufacturerAccess, cmd);
}

ByteArray SmartBattery::readSubclass(uint8_t id, uint8_t page)
{
    if (page < 0x78 || page > 0x7f)
        return ByteArray();
    
    writeWord(SBS::DataFlashSubClassID, id);
    stmApp()->delay(100);
    return readBlock(static_cast<SBS>(page));
}

void SmartBattery::writeSubclass(uint8_t id, uint8_t page, ByteArray ba)
{
    writeWord(SBS::DataFlashSubClassID, id);
    stmApp()->delay(100);
    ba.prepend((char)ba.size());
    m_dev->writeReg(page, reinterpret_cast<const uint8_t *>(ba.data()), ba.size());
    stmApp()->delay(100);
}
