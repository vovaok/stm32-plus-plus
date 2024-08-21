
#include "hd44780.h"
#include "timer.h"
struct timer timer_lcd;
/* Private HD44780 structure */


const char charTable[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x92,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA2,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB5,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
    0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,0xAC,0xE2,0xAD,0xAE,0x62,0xAF,0xB0,0xB1,
    0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
    0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7
};



/* Pin definitions */

#define HD44780_RS_LOW              HAL_GPIO_WritePin(HD44780_RS_PORT,HD44780_RS_PIN, GPIO_PIN_RESET)
#define HD44780_RS_HIGH             HAL_GPIO_WritePin(HD44780_RS_PORT,HD44780_RS_PIN, GPIO_PIN_SET)
#define HD44780_E_LOW               HAL_GPIO_WritePin(HD44780_E_PORT,HD44780_E_PIN, GPIO_PIN_RESET)
#define HD44780_E_HIGH              HAL_GPIO_WritePin(HD44780_E_PORT,HD44780_E_PIN, GPIO_PIN_SET)

#define HD44780_E_BLINK             HD44780_E_HIGH; HD44780_Delay(50); HD44780_E_LOW; HD44780_Delay(50)
#define HD44780_Delay(x)            delay_us(x)//osDelay(x)


uint8_t flag_first = 0;
void HD44780::Init() 
{
    
    HD44780::InitPins();
    
    /* At least 40ms */
    HD44780_Delay(40);
    
    /* Set LCD width and height */
    HD44780_Opts.Rows = rows;
    HD44780_Opts.Cols = cols;
    
    /* Set cursor pointer to beginning for LCD */
    HD44780_Opts.currentX = 0;
    HD44780_Opts.currentY = 0;
    
    HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
    if (rows > 1) {
        HD44780_Opts.DisplayFunction |= HD44780_2LINE;
    }
    
    /* Try to set 4bit mode */
    HD44780::Cmd4bit(0x03);
    osDelay(30);
    
    /* Second try */
    HD44780::Cmd4bit(0x03);
    osDelay(30);
    
    /* Third goo! */
    HD44780::Cmd4bit(0x03);
    osDelay(10);
    
    /* Set 4-bit interface */
    HD44780::Cmd4bit(0x02);
    osDelay(1);
    
    /* Set # lines, font size, etc. */
    HD44780::Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);
    
    /* Turn the display on with no cursor or blinking default */
    HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
    HD44780::DisplayOn();
    
    /* Clear lcd */
    //	HD44780::Clear();
    
    /* Default font directions */
    HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    HD44780::Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);
}

/* Set LCD width and height */
HD44780_Opts.Rows = rows;
HD44780_Opts.Cols = cols;

/* Set cursor pointer to beginning for LCD */
HD44780_Opts.currentX = 0;
HD44780_Opts.currentY = 0;

HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
if (rows > 1) {
    HD44780_Opts.DisplayFunction |= HD44780_2LINE;
}

/* Try to set 4bit mode */
HD44780::Cmd4bit(0x03);
osDelay(10);

/* Second try */
HD44780::Cmd4bit(0x03);
osDelay(10);

/* Third goo! */
HD44780::Cmd4bit(0x03);
osDelay(10);

/* Set 4-bit interface */
HD44780::Cmd4bit(0x02);
osDelay(1);

/* Set # lines, font size, etc. */
HD44780::Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

/* Turn the display on with no cursor or blinking default */
HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
HD44780::DisplayOn();

/* Clear lcd */
//	HD44780::Clear();

/* Default font directions */
HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
HD44780::Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

/* Delay */
osDelay(10);
uint8_t customChar[8] = {
    0b00000,
    0b00100,
    0b01010,
    0b10001,
    0b01010,
    0b00100,
    0b00000,
    0b00000
};

uint8_t customChar2[8] = {
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

HD44780::CreateChar(0, &customChar);
HD44780::CreateChar(1, &customChar2);HD44780::Clear();

}

void HD44780::Clear(void) {
    HD44780::Cmd(HD44780_CLEARDISPLAY);
    osDelay(5);
}

void HD44780::Puts(uint8_t x, uint8_t y, char* str) {
    HD44780::CursorSet(x, y);
    uint8_t cnt = 20;
    while (*str) {
        
        HD44780::Data(charTable[*str]);
        cnt--;
        HD44780_Opts.currentX++;
        
        str++;
    }
}

void HD44780::Puts1(uint8_t x, uint8_t y, char* str) {
    
    while (*str) {
        if (HD44780_Opts.currentX >= HD44780_Opts.Cols) {
            HD44780_Opts.currentX = 0;
            HD44780_Opts.currentY++;
            HD44780::CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
        }
        if (*str == '\n') {
            HD44780_Opts.currentY++;
            HD44780::CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
        } else if (*str == '\r') {
            HD44780::CursorSet(0, HD44780_Opts.currentY);
        } else {
            HD44780::Data(*str);
            HD44780::CursorSet(x, y);
        }
        str++;
    }
}

void HD44780::DisplayOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::DisplayOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::BlinkOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_BLINKON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::BlinkOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::CursorOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_CURSORON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::CursorOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
    HD44780::Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::ScrollLeft(void) {
    HD44780::Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780::ScrollRight(void) {
    HD44780::Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void HD44780::CreateChar(uint8_t location, uint8_t *data) {
    uint8_t i;
    /* We have 8 locations available for custom characters */
    location &= 0x07;
    HD44780::Cmd(HD44780_SETCGRAMADDR | (location << 3));
    
    for (i = 0; i < 8; i++) {
        HD44780::Data(data[i]);
    }
}

void HD44780::PutCustom(uint8_t x, uint8_t y, uint8_t location) {
    HD44780::CursorSet(x, y);
    HD44780::Data(location);
}

/* Private functions */
static void HD44780::Cmd(uint8_t cmd) {
    /* Command mode */
    HD44780_RS_LOW;
    
    /* High nibble */
    HD44780::Cmd4bit(cmd >> 4);
    /* Low nibble */
    HD44780::Cmd4bit(cmd & 0x0F);
}

void HD44780::Data(uint8_t data) {
    /* Data mode */
    HD44780_RS_HIGH;
    
    /* High nibble */
    HD44780::Cmd4bit(data >> 4);
    /* Low nibble */
    HD44780::Cmd4bit(data & 0x0F);
}

static void HD44780::Cmd4bit(uint8_t cmd) {
    /* Set output port */
    
    HAL_GPIO_WritePin(HD44780_D7_PORT, HD44780_D7_PIN, (cmd & 0x08));
    HAL_GPIO_WritePin(HD44780_D6_PORT, HD44780_D6_PIN, (cmd & 0x04));
    HAL_GPIO_WritePin(HD44780_D5_PORT, HD44780_D5_PIN, (cmd & 0x02));
    HAL_GPIO_WritePin(HD44780_D4_PORT, HD44780_D4_PIN, (cmd & 0x01));
    HD44780_E_BLINK;
}

void HD44780::CursorSet(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    
    /* Go to beginning */
    if (row >= HD44780_Opts.Rows) {
        row = 0;
    }
    
    /* Set current column and row */
    HD44780_Opts.currentX = col;
    HD44780_Opts.currentY = row;
    
    /* Set location address */
    HD44780::Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

static void HD44780::InitPins(void) {
    return;
}
