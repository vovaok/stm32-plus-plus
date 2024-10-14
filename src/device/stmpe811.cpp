#include "stmpe811.h"

STMPE811::STMPE811(I2c *i2c, uint8_t address) :
    m_i2c(i2c),
    m_address(address),
    m_isOpen(false)
{
    m_timer = new Timer;
    m_timer->setInterval(10);
    m_timer->onTimeout = EVENT(&STMPE811::onTimer);

    m_i2c->setBusClock(100000);
    m_i2c->open();

    open();

    calcCalibration(240, 320, 3850, 350, 300, 3900);
//    setSwappedXY(true);
}

void STMPE811::onTimer()
{
    m_timerFlag = true;
}

bool STMPE811::open()
{
    if (!checkId())
        return false;

    // Generate IO Expander Software reset
    reset();

    // IO Expander configuration
    fnctCmd(FctADC, true);

    // Touch Panel controller and ADC configuration
    tpConfig();

//    stmApp()->registerTaskEvent(EVENT(&STMPE811::task));
    m_timer->start();

    m_isOpen = true;

    return true;
}

bool STMPE811::read()
{
    if (!m_isOpen)
        return false;

    if (!m_timerFlag)
        return false;
    m_timerFlag = false;

    /* Check if the Touch detect event happened */
    m_pen = ((readReg(IOE_REG_TP_CTRL) & 0x80) != 0);

    if (m_pen)
    {
        m_rawX = readWord(IOE_REG_TP_DATA_X);
        m_rawY = readWord(IOE_REG_TP_DATA_Y);
        m_rawZ = readWord(IOE_REG_TP_DATA_Z);
    }

    /* Clear the interrupt pending bit and enable the FIFO again */
    writeReg(IOE_REG_FIFO_STA, 0x01);
    writeReg(IOE_REG_FIFO_STA, 0x00);

    return true;
}

bool STMPE811::checkId()
{
    return STMPE811_ID == readWord(0);
}

void STMPE811::reset()
{
    // Power Down the IO_Expander
    writeReg(IOE_REG_SYS_CTRL1, 0x02);
    // wait for a delay to insure registers erasing
    for (int w=200000; --w;);
    // Power On the Codec after the power off => all registers are reinitialized
    writeReg(IOE_REG_SYS_CTRL1, 0x00);
}

void STMPE811::fnctCmd(Fct fct, bool enable)
{
    uint8_t tmp = readReg(IOE_REG_SYS_CTRL2);
    if (enable)
        tmp &= ~(uint8_t)fct;
    else
        tmp |= (uint8_t)fct;
    writeReg(IOE_REG_SYS_CTRL2, tmp);
}

void STMPE811::ioAfConfig(uint8_t ioPin, bool enable)
{
    uint8_t tmp = readReg(IOE_REG_GPIO_AF);
    if (enable)
        tmp |= ioPin;
    else
        tmp &= ~ioPin;
    writeReg(IOE_REG_GPIO_AF, tmp);
}

void STMPE811::tpConfig()
{
    /* Enable touch Panel functionality */
    fnctCmd(FctTP, ENABLE);

    /* Select Sample Time, bit number and ADC Reference */
    writeReg(IOE_REG_ADC_CTRL1, 0x49);

    /* Wait for ~20 ms */
    for (int w=200000; --w;);

    /* Select the ADC clock speed: 3.25 MHz */
    writeReg(IOE_REG_ADC_CTRL2, 0x01);

    /* Select TSC pins in non default mode */
    ioAfConfig((uint8_t)TOUCH_IO_ALL, DISABLE);

    /* Select 2 nF filter capacitor */
    writeReg(IOE_REG_TP_CFG, 0x9A);

    /* Select single point reading  */
    writeReg(IOE_REG_FIFO_TH, 0x01);

    /* Write 0x01 to clear the FIFO memory content. */
    writeReg(IOE_REG_FIFO_STA, 0x01);

    /* Write 0x00 to put the FIFO back into operation mode  */
    writeReg(IOE_REG_FIFO_STA, 0x00);

    /* set the data format for Z value: 7 fractional part and 1 whole part */
    writeReg(IOE_REG_TP_FRACT_XYZ, 0x01);

    /* set the driving capability of the device for TSC pins: 50mA */
    writeReg(IOE_REG_TP_I_DRIVE, 0x01);

    /* Use no tracking index, touch-panel controller operation mode (XYZ) and
     enable the TSC */
    writeReg(IOE_REG_TP_CTRL, 0x03);

    /*  Clear all the status pending bits */
    writeReg(IOE_REG_INT_STA, 0xFF);
}

uint8_t STMPE811::readReg(uint8_t addr)
{
    uint8_t tmp = addr;
    if (m_i2c->write(m_address, &tmp, 1))
        m_i2c->read(m_address, &tmp, 1);
    return tmp;
}

void STMPE811::writeReg(uint8_t addr, uint8_t value)
{
    uint8_t buf[2] = {addr, value};
    m_i2c->write(m_address, buf, 2);
}

uint16_t STMPE811::readWord(uint8_t addr)
{
    uint8_t buf[2] = {addr, IOE_REG_CHP_ID};
    if (m_i2c->write(m_address, buf, 1))
        m_i2c->read(m_address, buf, 2);
    return (buf[0] << 8) | buf[1];
}