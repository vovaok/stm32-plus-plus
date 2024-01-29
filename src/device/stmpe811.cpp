#include "stmpe811.h"

STMPE811::STMPE811(I2c *i2c, uint8_t address) :
    m_i2c(i2c),
    m_address(address),
    m_isOpen(false),
    m_touchDetected(false),
    m_oldTouch(false),
    m_x(0), m_y(0), m_z(0)
{
    m_timer = new Timer;
    m_timer->setInterval(10);
    m_timer->setTimeoutEvent(EVENT(&STMPE811::task));
}

void STMPE811::task()
{
    read();
    
    if (!m_oldTouch && m_touchDetected)
    {
        if (onTouch)
            onTouch(m_x, m_y);
    }
    else if (m_oldTouch && !m_touchDetected)
    {
        if (onRelease)
            onRelease(m_x, m_y);
    }
    
    m_oldTouch = m_touchDetected;
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
  
    int xDiff, yDiff, x, y;

    /* Check if the Touch detect event happened */
    m_touchDetected = ((readReg(IOE_REG_TP_CTRL) & 0x80) != 0);

    if (m_touchDetected) 
    {
        x = readX();
        y = readY();
        xDiff = x > m_x? (x - m_x): (m_x - x);
        yDiff = y > m_y? (y - m_y): (m_y - y);       
        if (xDiff + yDiff > 5)
        {
            m_x = x;
            m_y = y;       
        }
    }  

    /* Update the Z Pression index */  
    m_z = readZ();  

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

uint16_t STMPE811::readX()
{
    int32_t x, xr;

    /* Read x value from DATA_X register */
    x = readWord(IOE_REG_TP_DATA_X);

    /* x value first correction */
    if (x <= 3000)
        x = 3870 - x;
    else
        x = 3800 - x;

    /* x value second correction */  
    xr = x / 15;

    /* return x position value */
    if (xr < 0)
        xr = 0;
    else if (xr >= 240)
        xr = 239;
    return (uint16_t)(xr); 
}

uint16_t STMPE811::readY()
{
    int32_t y, yr;

    /* Read y value from DATA_Y register */
    y = readWord(IOE_REG_TP_DATA_Y);

    /* y value first correction */
    y -= 360;  

    /* y value second correction */
    yr = y / 11;

    /* return y position value */
    if (yr < 0)
        yr = 0;
    else if (yr >= 320)
        yr = 319;
    return (uint16_t)(yr); 
}

uint16_t STMPE811::readZ()
{
    int16_t z;

    /* Read z value from DATA_Z register */
    z = readWord(IOE_REG_TP_DATA_Z);

    /* return z position value */
    if (z < 0)
        z = 0;
    return (uint16_t)(z); 
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