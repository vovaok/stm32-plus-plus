#ifndef _RCC_H
#define _RCC_H

#include "stm32_conf.h"
#include "core/coreexception.h"

class Rcc
{
private:
    static unsigned long mHseValue;
    static unsigned long mPllM;
    static unsigned long mPllN;
    static unsigned long mPllP;
    static unsigned long mPllQ;
    static unsigned long mSysClk;
    static unsigned long mAHBClk;
    static unsigned long mAPB1Clk;
    static unsigned long mAPB2Clk;
    
public:
    static void configPll(unsigned long hseValue, unsigned long sysClk);
    inline static unsigned long hseValue() {return Rcc::mHseValue;}
    inline static unsigned long sysClk() {return Rcc::mSysClk;}
    inline static unsigned long hClk() {return Rcc::mAHBClk;}
    inline static unsigned long pClk1() {return Rcc::mAPB1Clk;}
    inline static unsigned long pClk2() {return Rcc::mAPB2Clk;}
};

#endif