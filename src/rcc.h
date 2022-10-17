#ifndef _RCC_H
#define _RCC_H

//#include "stm32_conf.h"
#include "stm32f4xx.h"

class Rcc
{
private:
    static unsigned long mHseValue;
    static unsigned long mPllM;
    static unsigned long mPllN;
#if !defined(STM32F37X)
    static unsigned long mPllP;
    static unsigned long mPllQ;
#endif
    static unsigned long mSysClk;
    static unsigned long mAHBClk;
    static unsigned long mAPB1Clk;
    static unsigned long mAPB2Clk;
    
public:
    static void configPll(unsigned long hseValue, unsigned long sysClk);
    static unsigned long hseValue() {return Rcc::mHseValue;}
    static unsigned long sysClk() {return Rcc::mSysClk;}
    static unsigned long hClk() {return Rcc::mAHBClk;}
    static unsigned long pClk1() {return Rcc::mAPB1Clk;}
    static unsigned long pClk2() {return Rcc::mAPB2Clk;}
};

#endif