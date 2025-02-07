
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
 void setCmdPins(Gpio::PinName rs, Gpio::PinName e );
 void setDataPins(Gpio::PinName d4, Gpio::PinName d5, Gpio::PinName d6, Gpio::PinName d7 );
 void InitPins(void);
    
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
void Puts(uint8_t x, uint8_t y, const char *str);

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

void Puts1(uint8_t x, uint8_t y, const char *str);

void CursorSet(uint8_t col, uint8_t row);
void Data(uint8_t data);

int currentColumn() const {return HD44780_Opts.currentX;}
int currentRow() const {return HD44780_Opts.currentY;}


  private:
    
    static const char charTable[256];
    
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


void Cmd(uint8_t cmd);
void Cmd4bit(uint8_t cmd);
void delay(int ms) {for (int w=ms*20000; --w;);}
void delay_us(int us) {for (int w=us*20; --w;);}

 HD44780_Options_t HD44780_Opts;

void HD44780_RS_LOW()     {rsPin->write(false); }       
void HD44780_RS_HIGH()    {rsPin->write(true);  }       
void HD44780_E_LOW()      {ePin->write(false); }                       
void HD44780_E_HIGH()     {ePin->write(true);  }                

void HD44780_E_BLINK()    {HD44780_E_HIGH(); delay_us(150); HD44780_E_LOW(); delay_us(150); }

};



#endif
