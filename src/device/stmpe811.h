#ifndef _STMPE811_H
#define _STMPE811_H

#include "i2c.h"
#include "core/timer.h"
#include "gfx/touchscreen.h"

class STMPE811 : public TouchScreen
{
public:
    STMPE811(I2c *i2c, uint8_t address);

    bool open();
    bool read();

    bool isOpen() const {return m_isOpen;}

    Closure<void(int, int)> onTouch;
    Closure<void(int, int)> onRelease;

    bool isTouchDetected() const {return m_touchDetected;}
    uint16_t x() const {return m_x;}
    uint16_t y() const {return m_y;}
    uint16_t z() const {return m_z;}

private:
    I2c *m_i2c;
    Timer *m_timer;
    uint8_t m_address;
    bool m_isOpen;
    bool m_touchDetected, m_oldTouch;
    uint16_t m_x, m_y, m_z;

    void task();

    enum Register
    {
        IOE_REG_CHP_ID	        = 0x00,
        IOE_REG_ID_VER	        = 0x02,
        IOE_REG_SYS_CTRL1	    = 0x03,
        IOE_REG_SYS_CTRL2	    = 0x04,
        IOE_REG_SPI_CFG	        = 0x08,
        IOE_REG_INT_CTRL    	= 0x09,
        IOE_REG_INT_EN	        = 0x0A,
        IOE_REG_INT_STA     	= 0x0B,
        IOE_REG_GPIO_INT_EN	    = 0x0C,
        IOE_REG_GPIO_INT_STA	= 0x0D,
        IOE_REG_GPIO_SET_PIN	= 0x10,
        IOE_REG_GPIO_CLR_PIN	= 0x11,
        IOE_REG_GPIO_MP_STA	    = 0x12,
        IOE_REG_GPIO_DIR	    = 0x13,
        IOE_REG_GPIO_ED	        = 0x14,
        IOE_REG_GPIO_RE	        = 0x15,
        IOE_REG_GPIO_FE	        = 0x16,
        IOE_REG_GPIO_AF	        = 0x17,
        IOE_REG_ADC_INT_EN	    = 0x0E,
        IOE_REG_ADC_INT_STA	    = 0x0F,
        IOE_REG_ADC_CTRL1	    = 0x20,
        IOE_REG_ADC_CTRL2	    = 0x21,
        IOE_REG_ADC_CAPT	    = 0x22,
        IOE_REG_ADC_DATA_CH0	= 0x30,
        IOE_REG_ADC_DATA_CH1	= 0x32,
        IOE_REG_ADC_DATA_CH2	= 0x34,
        IOE_REG_ADC_DATA_CH3	= 0x36,
        IOE_REG_ADC_DATA_CH4	= 0x38,
        IOE_REG_ADC_DATA_CH5	= 0x3A,
        IOE_REG_ADC_DATA_CH6	= 0x3B,
        IOE_REG_ADC_DATA_CH7	= 0x3C,
        IOE_REG_TP_CTRL	        = 0x40,
        IOE_REG_TP_CFG	        = 0x41,
        IOE_REG_WDM_TR_X	    = 0x42,
        IOE_REG_WDM_TR_Y	    = 0x44,
        IOE_REG_WDM_BL_X	    = 0x46,
        IOE_REG_WDM_BL_Y	    = 0x48,
        IOE_REG_FIFO_TH	        = 0x4A,
        IOE_REG_FIFO_STA	    = 0x4B,
        IOE_REG_FIFO_SIZE	    = 0x4C,
        IOE_REG_TP_DATA_X	    = 0x4D,
        IOE_REG_TP_DATA_Y	    = 0x4F,
        IOE_REG_TP_DATA_Z	    = 0x51,
        IOE_REG_TP_DATA_XYZ	    = 0x52,
        IOE_REG_TP_FRACT_XYZ	= 0x56,
        IOE_REG_TP_DATA	        = 0x57,
        IOE_REG_TP_I_DRIVE	    = 0x58,
        IOE_REG_TP_SHIELD	    = 0x59
    };

    enum Fct
    {
        FctADC  = 0x01,
        FctTP   = 0x02,
        FctIO   = 0x04
    };

    enum IoPin
    {
        IO_Pin_0    = 0x01,
        IO_Pin_1    = 0x02,
        IO_Pin_2    = 0x04,
        IO_Pin_3    = 0x08,
        IO_Pin_4    = 0x10,
        IO_Pin_5    = 0x20,
        IO_Pin_6    = 0x40,
        IO_Pin_7    = 0x80,
        IO_Pin_ALL  = 0xFF,
        TOUCH_YD    = IO_Pin_1,
        TOUCH_XD    = IO_Pin_2,
        TOUCH_YU    = IO_Pin_3,
        TOUCH_XU    = IO_Pin_4,
        TOUCH_IO_ALL = (IO_Pin_1 | IO_Pin_2 | IO_Pin_3 | IO_Pin_4)
    };

    static const uint16_t STMPE811_ID = 0x0811;
    bool checkId();
    void reset();
    void fnctCmd(Fct fct, bool enable);
    void ioAfConfig(uint8_t ioPin, bool enable);

    void tpConfig();

    uint16_t readX();
    uint16_t readY();
    uint16_t readZ();

    uint8_t readReg(uint8_t addr);
    void writeReg(uint8_t addr, uint8_t value);
    uint16_t readWord(uint8_t addr);
};

#endif