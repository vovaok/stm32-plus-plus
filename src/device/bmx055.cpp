#include "bmx055.h"

BMX055::BMX055(I2c *i2c) :
    m_dev(i2c),
    m_valid(false)
{
    for (int i=0; i<3; i++)
        m_acc[i] = 0;
    m_dev->open();
    
    uint8_t devid = readReg(REG_BMA_CHIPID);
    if (devid == BMA_ID)
    {
        writeReg(REG_BMA_RESET, 0xB6); // reset
        for (int w=0; w<100000; w++);
        writeReg(REG_BMA_LPW, 0x00);
        writeReg(REG_BMA_RANGE, 0x03); // 0x03 = ±2g, 0x05 = ±4g, 0x08 = ±8g, 0x0C = ±16g
        writeReg(REG_BMA_BW, 0x0E); // 0x0C = 125Hz, 0x0D = 250 Hz, 0x0E = 500Hz, 0x0F = 1000Hz
        writeReg(REG_BMA_D_HBW, 0x00);
        setFastOffset();
        m_valid = true;
    }
}

uint8_t BMX055::readReg(uint8_t reg)
{
    uint8_t buf = 0;
    m_dev->write(BMA_ADDRESS, (uint8_t*)&reg, 1);
    m_dev->read(BMA_ADDRESS, &buf, 1);
    return buf;
}

bool BMX055::writeReg(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return m_dev->write(BMA_ADDRESS, buf, 2);
}

bool BMX055::readAcc()
{
    bool r = false;
    uint8_t buf[1] = {REG_BMA_X_LSB};
    r = m_dev->write(BMA_ADDRESS, buf, 1);
    if (!r)
        return false;
    r = m_dev->read(BMA_ADDRESS, (uint8_t*)m_acc, 6);
    for (int i=0; i<3; i++)
        m_value[i] = m_acc[i] / 32768.f;
    return r;
}

void BMX055::setFastOffset()
{			
    uint8_t r;
    //	Сбрасываем все значения регистров компенсаций смещения в 0 (установкой флага offset_reset)
    writeReg(REG_BMA_OFC_CTRL, 0x80);
    //	Устанавливаем значения компенсации смещения в X=0g, Y=0g, Z=+1g (offset_target_z=«01», offset_target_y=«00», offset_target_x=«00», cut_off=«0»)
    writeReg(REG_BMA_OFC_SETTING, (0x01 << 3)); 
    
    writeReg(REG_BMA_OFC_CTRL, 0x20); //	Вычисляем смещение для оси X (cal_trigger=«01» быстрое вычисление компенсации смешения)
    do
        r = readReg(REG_BMA_OFC_CTRL);
    while (!(r & 0x10)); //	Ждём завершения быстрой компенсации смещения (ждём пока установится флаг cal_rdy)
    
    writeReg(REG_BMA_OFC_CTRL, 0x40); //	Вычисляем смещение для оси Y (cal_trigger=«10» быстрое вычисление компенсации смешения)
    do
        r = readReg(REG_BMA_OFC_CTRL);
    while (!(r & 0x10)); //	Ждём завершения быстрой компенсации смещения (ждём пока установится флаг cal_rdy)
    
    writeReg(REG_BMA_OFC_CTRL, 0x60); //	Вычисляем смещение для оси Z (cal_trigger=«11» быстрое вычисление компенсации смешения)
    do
        r = readReg(REG_BMA_OFC_CTRL);
    while (!(r & 0x10)); //	Ждём завершения быстрой компенсации смещения (ждём пока установится флаг cal_rdy)
}				