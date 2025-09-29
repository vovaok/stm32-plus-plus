#include "bmx055.h"

// ------- Accelerometer -------
//	АДРЕС И ID ЧИПА:						//
#define	BMA_ADDRESS				(0x19<<1)   //	Адрес датчика на шине I2C
#define	BMA_ID					0xFA		//	ID датчика
// ------- Gyroscope -------
//	АДРЕС И ID ЧИПА:						//
#define	BMG_ADDRESS				(0x69<<1)		//	Адрес датчика на шине I2C
#define	BMG_ID					0x0F		//	ID датчика
// ------- Magnetometer -------
//	АДРЕС И ID ЧИПА:						//
#define	BMM_ADDRESS				(0x13<<1)		//	Адрес датчика на шине I2C
#define	BMM_ID					0x32		//	ID датчика

BMX055::BMX055(I2c *i2c) :
    m_dev(i2c)
{
    m_dev->open();
//    for (int w=0; w<10000; w++);
    readReg(REG_BMA_CHIPID); // dummy read
    
    uint8_t accid = readReg(REG_BMA_CHIPID);
    if (accid == BMA_ID)
    {
        writeReg(REG_BMA_RESET, 0xB6); // reset
        for (int w=0; w<100000; w++);
        writeReg(REG_BMA_LPW, 0x00);
        writeReg(REG_BMA_RANGE, 0x03); // 0x03 = ±2g, 0x05 = ±4g, 0x08 = ±8g, 0x0C = ±16g
        writeReg(REG_BMA_BW, 0x0E); // 0x0C = 125Hz, 0x0D = 250 Hz, 0x0E = 500Hz, 0x0F = 1000Hz
        writeReg(REG_BMA_D_HBW, 0x00);
        //setFastOffset();
        m_accValid = true;
    }
    
    uint8_t gyroid = readReg(REG_BMG_CHIPID);
    if (gyroid == BMG_ID)
    {
        writeReg(REG_BMG_RESET, 0xB6); // reset
        for (int w=0; w<100000; w++);
        writeReg(REG_BMG_LPM1, 0x00);
        writeReg(REG_BMG_RANGE, 0x00); // 2000 DPS
        writeReg(REG_BMG_BW, 0x00); // 523 Hz 
        writeReg(REG_BMG_D_HBW, 0x00);
//        setFastOffset();
        writeReg(REG_BMG_AOFS_FOFS, 0xFF); // perform fast offset calibration
        while (readReg(REG_BMG_AOFS_FOFS) & 0x08 == 0);
        m_gyroValid = true;
    }
}

void BMX055::update(float dt)
{
    bool r = true;
    r &= readAcc();
    r &= readGyro();
    if (r)
    {
        calcQuat(dt);
    }
    else
    {
        m_valid = false;
    }
}

uint8_t BMX055::readReg(BMAReg reg)
{
    return readReg(BMA_ADDRESS, reg);
}

bool BMX055::writeReg(BMAReg reg, uint8_t value)
{
    return writeReg(BMA_ADDRESS, reg, value);
}

uint8_t BMX055::readReg(BMGReg reg)
{
    return readReg(BMG_ADDRESS, reg);
}

bool BMX055::writeReg(BMGReg reg, uint8_t value)
{
    return writeReg(BMG_ADDRESS, reg, value);
}

uint8_t BMX055::readReg(BMMReg reg)
{
    return readReg(BMM_ADDRESS, reg);
}

bool BMX055::writeReg(BMMReg reg, uint8_t value)
{
    return writeReg(BMM_ADDRESS, reg, value);
}

uint8_t BMX055::readReg(uint8_t addr, uint8_t reg)
{
    uint8_t buf = 0;
    m_dev->write(addr, (uint8_t*)&reg, 1);
    m_dev->read(addr, &buf, 1);
    return buf;
}

bool BMX055::writeReg(uint8_t addr, uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return m_dev->write(addr, buf, 2);
}

bool BMX055::readAcc()
{
    bool r = false;
    uint8_t buf[6] = {REG_BMA_X_LSB, 0, 0, 0, 0, 0};
    r = m_dev->write(BMA_ADDRESS, buf, 1);
    if (!r)
        return false;
    r = m_dev->read(BMA_ADDRESS, buf, 6);
    for (int i=0; i<3; i++)
        m_acc[i] = ((int16_t *)buf)[i] * (2.f / 32768.f);
    return r;
    // buf[6] <- temperature
}

bool BMX055::readGyro()
{
    bool r = false;
    uint8_t buf[6] = {REG_BMG_X_LSB, 0, 0, 0, 0, 0};
    r = m_dev->write(BMG_ADDRESS, buf, 1);
    if (!r)
        return false;
    r = m_dev->read(BMG_ADDRESS, buf, 6);
    for (int i=0; i<3; i++)
        m_gyro[i] = ((int16_t *)buf)[i]  * ((1998.f * M_PI / 180.f) / 32768.f);
    return r;
}

//bool BMX055::readMag()
//{
//    
//}

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