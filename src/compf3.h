#ifndef _COMP_H
#define _COMP_H

#include "core/core.h"
#include "gpio.h"



//---------------------------------------------------------------------------


//---------------------------------------------------------------------------



class Comp
{
public:
#pragma pack(push,1)
    union Config
    {
        struct
        {
            uint32_t csr;          
        };

        struct
        {
            uint8_t COMPEN: 1;
            uint8_t reserve_1: 1;
            uint8_t COMPMODE: 2;
            uint8_t COMPINMSEL: 3;   
            uint8_t COMPINPSEL: 1;        
            uint8_t reserve_2: 1;  
            uint8_t COMPWINMODE: 1;  
            uint8_t COMPOUTSEL: 4;
            uint8_t reserve_3: 1;
            uint8_t COMPPOL: 1;            
            uint8_t COMPHYST: 2;
            uint8_t COMP_BLANKING: 2;
            uint8_t reserve_4: 1;
            uint8_t COMPINMSEL_H: 1;     //Hight bit  for 4-6 bits
            uint8_t reserve_5: 7; 
            uint8_t COMPOUT: 1;
            uint8_t COMPLOCK: 1;           

        };
        Config() : csr(0x0000) {}
    };
#pragma pack(pop)
    
    typedef enum
    {
        NoSelection              = 0b0000, //!< No selection
        Tim1Br1                  = 0b0001, //!< (BRK_ACTH) Timer 1 break inpu
        Tim1Br2                  = 0b0010, //!< (BRK2) Timer 1 break input 2
        Tim8Br1                  = 0b0011, //!< (BRK_ACTH) Timer 8 break input 
        Tim8Br2                  = 0b0100, //!< (BRK2) Timer 8 break input 2 
        Tim1Br2_Tim8Br2          = 0b0101, //!< Timer 1 break input2 + Timer 8 break input 2
        Tim1OcRef                = 0b0110, //!< Timer 1 OCrefclear input
        Tim1Capture1             = 0b0111, //!< Timer 1 input capture 1
        Tim2Capture4             = 0b1000, //!< Timer 2 input capture 4
        Tim2OcRef                = 0b1001, //!< Timer 2 OCrefclear input
        Tim3Capture1             = 0b1010, //!< Timer 3 input capture 1
        Tim3OcRef                = 0b1011, //!< Timer 3 OCrefclear inpu
        Tim20Br1                 = 0b1100, //!< Timer 20 Break Input selected
        Tim20Br2                 = 0b1101, //!< Timer 20 Break2 Input selected
        Tim1Br2_Tim8Br2_Tim8Br2  = 0b1110, //!< Timer 1 Break2 or Timer 8 Break2 or Timer 20 Break2
        Tim20OcRef               = 0b1111  //!< Timer 20 OCrefClear Input selected
            
    } OutSelCode;
    
    
    
    typedef enum
    {
    Vref1_4   = 0b0000, //!<1/4 of Vrefint
    Vref1_2   = 0b0001, //!<1/2 of Vrefint
    Vref3_4   = 0b0010, //!<3/4 of Vrefint
    Vref      = 0b0011, //!<Vrefint
    Dac1_ch1  = 0b0100, //!<PA4 or DAC1_CH1 output if enabled        
    Dac1_ch2  = 0b0101, //!<PA5 or DAC1_CH2 output if enabled
    PA2_p     = 0b0110, //!<PA2
    Dac2_ch1  = 0b1000  //!<DAC2_CH1 outpu 
    }
    InMSelCode;

private:
    COMP_TypeDef *mDev;
    Config mConfig;
    void updateConfig();

public:
    explicit Comp(int no);
    void setEnable(bool en);
    void setInvertedOut(bool invert);
    void setInMSel(InMSelCode code);
    void setOutSel(OutSelCode code);

    void setConfig(Config cfg);
 
};

#endif