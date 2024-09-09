
#ifndef HD44780_H
#define HD44780_H


#include "stdio.h"
#include "gpio.h"


/* 4 bit mode */
/* Control pins, can be overwritten */
/* RS - Register select pin */

#define HD44780_RS_PORT				LCD_RS_GPIO_Port
#define HD44780_RS_PIN				LCD_RS_Pin

/* E - Enable pin */

#define HD44780_E_PORT				LCD_E_GPIO_Port
#define HD44780_E_PIN				LCD_E_Pin

/* Data pins */
/* D4 - Data 4 pin */

#define HD44780_D4_PORT				LCD_D4_GPIO_Port
#define HD44780_D4_PIN				LCD_D4_Pin

/* D5 - Data 5 pin */

#define HD44780_D5_PORT				LCD_D5_GPIO_Port
#define HD44780_D5_PIN				LCD_D5_Pin

/* D6 - Data 6 pin */

#define HD44780_D6_PORT				LCD_D6_GPIO_Port
#define HD44780_D6_PIN				LCD_D6_Pin

/* D7 - Data 7 pin */

#define HD44780_D7_PORT				LCD_D7_GPIO_Port
#define HD44780_D7_PIN				LCD_D7_Pin

class HD44780
{

  public:
    
     HD44780(char cols, char rows);
 void setCmdPins(Gpio::Config rs, Gpio::Config e );
 void setDataPins(Gpio::Config d4, Gpio::Config d5, Gpio::Config d6, Gpio::Config d7 );
    
 typedef enum
 {
      /* Commands*/
 HD44780_CLEARDISPLAY        =0x01,
 HD44780_RETURNHOME          =0x02,
 HD44780_ENTRYMODESET        =0x04,
 HD44780_DISPLAYCONTROL      =0x08,
 HD44780_CURSORSHIFT         =0x10,
 HD44780_FUNCTIONSET         =0x20,
 HD44780_SETCGRAMADDR        =0x40,
 HD44780_SETDDRAMADDR        =0x80,

/* Flags for display entry mode */
 HD44780_ENTRYRIGHT          =0x00,
 HD44780_ENTRYLEFT           =0x02,
 HD44780_ENTRYSHIFTINCREMENT =0x01,
 HD44780_ENTRYSHIFTDECREMENT =0x00,

/* Flags for display on/off control */
 HD44780_DISPLAYON           =0x04,
 HD44780_CURSORON            =0x02,
 HD44780_BLINKON             =0x01,

/* Flags for display/cursor shift */
 HD44780_DISPLAYMOVE         =0x08,
 HD44780_CURSORMOVE          =0x00,
 HD44780_MOVERIGHT           =0x04,
 HD44780_MOVELEFT            =0x00,

/* Flags for function set */
 HD44780_8BITMODE            =0x10,
 HD44780_4BITMODE            =0x00,
 HD44780_2LINE               =0x08,
 HD44780_1LINE               =0x00,
 HD44780_5x10DOTS            =0x04,
 HD44780_5x8DOTS             =0x00,
 } Flag;
    
    
  
/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: Width of lcd
 * @param  rows: Height of lcd
 * @retval None
 */
void Init();

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x: X location where string will start
 * @param  y; Y location where string will start
 * @param  *str: pointer to string to display
 * @retval None
 */
void Puts(uint8_t x, uint8_t y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void ScrollLeft(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void ScrollRight(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  x: X location where character will be shown
 * @param  y: Y location where character will be shown
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void PutCustom(uint8_t x, uint8_t y, uint8_t location);

void Puts1(uint8_t x, uint8_t y, char* str);

void CursorSet(uint8_t col, uint8_t row);
void Data(uint8_t data);


  private:
    
    char charTable[256] = {
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
    
    typedef struct {
    uint8_t DisplayControl;
    uint8_t DisplayFunction;
    uint8_t DisplayMode;
    uint8_t Rows;
    uint8_t Cols;
    uint8_t currentX;
    uint8_t currentY;
} HD44780_Options_t;
    
uint8_t mCols, mRows;
Gpio *rsPin, *ePin, *d4Pin, *d5Pin, *d6Pin, *d7Pin;   

void InitPins(void);
void Cmd(uint8_t cmd);
void Cmd4bit(uint8_t cmd);
void delay(int ms) {for (int w=ms*20000; --w;);}
void delay_us(int us) {for (int w=us*20; --w;);}

 HD44780_Options_t HD44780_Opts;

void HD44780_RS_LOW()     {rsPin->write(false); }       
void HD44780_RS_HIGH()    {rsPin->write(true);  }       
void HD44780_E_LOW()      {ePin->write(false); }                       
void HD44780_E_HIGH()     {ePin->write(true);  }                

void HD44780_E_BLINK()    {HD44780_E_HIGH(); delay_us(50); HD44780_E_LOW(); delay_us(50); }

};



#endif
