
#include "device/hd44780.h"

HD44780::HD44780(char cols, char rows) : mCols(cols), mRows(rows)
{

  
  
}
void HD44780::Init() 
{
    /* At least 40ms */
    delay(40);
    
    /* Set LCD width and height */
    HD44780_Opts.Rows = mRows;
    HD44780_Opts.Cols = mCols;
    
    /* Set cursor pointer to beginning for LCD */
    HD44780_Opts.currentX = 0;
    HD44780_Opts.currentY = 0;
    
    HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
    if (mRows > 1) {
        HD44780_Opts.DisplayFunction |= HD44780_2LINE;
    }
    
    /* Try to set 4bit mode */
    Cmd4bit(0x03);
    delay(30);
    
    /* Second try */
    Cmd4bit(0x03);
    delay(30);
    
    /* Third goo! */
    Cmd4bit(0x03);
    delay(10);
    
    /* Set 4-bit interface */
    Cmd4bit(0x02);
    delay(1);
    
    /* Set # lines, font size, etc. */
   Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);
    
    /* Turn the display on with no cursor or blinking default */
    HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
    DisplayOn();
    
    /* Clear lcd */
    //	HD44780::Clear();
    
    /* Default font directions */
    HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
    Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);


///* Delay */
//osDelay(10);
//uint8_t customChar[8] = {
//    0b00000,
//    0b00100,
//    0b01010,
//    0b10001,
//    0b01010,
//    0b00100,
//    0b00000,
//    0b00000
//};
//
//uint8_t customChar2[8] = {
//    0b00000,
//    0b00100,
//    0b01110,
//    0b11111,
//    0b01110,
//    0b00100,
//    0b00000,
//    0b00000
//};

//CreateChar(0, &customChar);
//CreateChar(1, &customChar2);
Clear();

}

void HD44780::Clear(void) {
    Cmd(HD44780_CLEARDISPLAY);
    delay(5);
}

void HD44780::Puts(uint8_t x, uint8_t y, char* str) {
    CursorSet(x, y);
    uint8_t cnt = 20;
    while (*str) {
        
        Data(charTable[*str]);
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
            CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
        }
        if (*str == '\n') {
            HD44780_Opts.currentY++;
            CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
        } else if (*str == '\r') {
            CursorSet(0, HD44780_Opts.currentY);
        } else {
            Data(*str);
            CursorSet(x, y);
        }
        str++;
    }
}

void HD44780::DisplayOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::DisplayOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::BlinkOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_BLINKON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::BlinkOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::CursorOn(void) {
    HD44780_Opts.DisplayControl |= HD44780_CURSORON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::CursorOff(void) {
    HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
    Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void HD44780::ScrollLeft(void) {
    Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void HD44780::ScrollRight(void) {
    Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void HD44780::CreateChar(uint8_t location, uint8_t *data) {
    uint8_t i;
    /* We have 8 locations available for custom characters */
    location &= 0x07;
    Cmd(HD44780_SETCGRAMADDR | (location << 3));
    
    for (i = 0; i < 8; i++) {
        Data(data[i]);
    }
}

void HD44780::PutCustom(uint8_t x, uint8_t y, uint8_t location) {
    CursorSet(x, y);
    Data(location);
}

/* Private functions */
void HD44780::Cmd(uint8_t cmd) {
    /* Command mode */
    HD44780_RS_LOW();
    
    /* High nibble */
    Cmd4bit(cmd >> 4);
    /* Low nibble */
    Cmd4bit(cmd & 0x0F);
}

void HD44780::Data(uint8_t data) {
    /* Data mode */
    HD44780_RS_HIGH();
    
    /* High nibble */
    Cmd4bit(data >> 4);
    /* Low nibble */
    Cmd4bit(data & 0x0F);
}

void HD44780::Cmd4bit(uint8_t cmd) {
    /* Set output port */
  
  
    
     d7Pin->write(cmd & 0x08);
     d6Pin->write(cmd & 0x04);
     d5Pin->write(cmd & 0x02);
     d4Pin->write(cmd & 0x01);
    
    
    
    HD44780_E_BLINK();
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


void HD44780::setCmdPins(Gpio::PinName rs, Gpio::PinName e )
 {
   rsPin = new Gpio(rs);
   rsPin->setAsOutput();
   
   ePin = new Gpio( e);
   ePin->setAsOutput();  
 }
   
 void HD44780::setDataPins(Gpio::PinName d4, Gpio::PinName d5, Gpio::PinName d6, Gpio::PinName d7 )
 {
   d4Pin = new Gpio(d4);
   d4Pin->setAsOutput();
   
   d5Pin = new Gpio(d5);
   d5Pin->setAsOutput();
   
   d6Pin = new Gpio(d6);
   d6Pin->setAsOutput();
   
   d7Pin = new Gpio(d7);
   d7Pin->setAsOutput();
 }


 void HD44780::InitPins(void) {
   
   
   
   
   
}
