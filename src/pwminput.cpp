#include "pwminput.h"

PwmInput::PwmInput(Gpio::Config inputPin) :
    HardwareTimer((TimerNumber)GpioConfigGetPeriphNumber(inputPin))
{
    int ch = GpioConfigGetPeriphChannel(inputPin);
    if (ch != 1 && ch != 2)
        THROW(Exception::InvalidPin);

    Gpio::config(inputPin);

    // 1. Select the active input for TIMx_CCR1: write the CC1S bits to ‘01’
    // in the TIMx_CCMR1 register (TI1 selected).
    // 2. Select the active polarity for TI1FP1 (used both for capture in
    // TIMx_CCR1 and counter clear): program the CC1P and CC1NP bits to ‘00’
    // (active on rising edge).
    // 3. Select the active input for TIMx_CCR2: write the CC2S bits to ‘10’
    // in the TIMx_CCMR1 register (TI1 selected).
    // 4. Select the active polarity for TI1FP2 (used for capture in TIMx_CCR2):
    // program the CC2P and CC2NP bits to ‘11’ (active on falling edge).
    // 5. Select the valid trigger input: write the TS bits to ‘101’ in the
    // TIMx_SMCR register (TI1FP1 selected).
    // 6. Configure the slave mode controller in reset mode: write the SMS bits
    // to ‘100’ in the TIMx_SMCR register.
    // 7. Enable the captures: write the CC1E and CC2E bits to ‘1’ in the
    // TIMx_CCER register.

    setCounter(0);
    setCompare1(0);
    setCompare2(0);
    setAutoReloadRegister(-1);

    selectInputTrigger(ch==2? TsTI2FP2: TsTI1FP1);
    setSlaveMode(SmReset);

    TIM_TypeDef *t = tim();
    t->CR1 |= TIM_CR1_URS; // set update flag on the overflow only
    /// @todo add filter if needed
    if (ch == 2)
    {
        t->CCMR1 = TIM_CCMR1_CC1S_1 | TIM_CCMR1_CC2S_0;
        t->CCER = (t->CCER & ~0xFF) | TIM_CCER_CC1P;
    }
    else // if (ch == 1)
    {
        t->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
        t->CCER = (t->CCER & ~0xFF) | TIM_CCER_CC2P;
    }
    t->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
}

void PwmInput::setMinFrequency(unsigned int freq)
{
    tim()->ARR = inputClk() / (freq * (tim()->PSC + 1)) - 1;
    //! @note to change prescaler use setPrecaler directly!
//    setPrescaler((inputClk() / freq) >> 16);
}

#if __FPU_PRESENT
float PwmInput::read()
#else
int PwmInput::read()
#endif
{
    TIM_TypeDef *t = tim();
    if (t->SR & TIM_SR_UIF) // overflow
    {
        m_period = 0;
        m_flag = false;
        t->SR = 0; // clear all flags
        // tut budet 4ot tipa: if (noga == 1) duty = 100%;
    }
    else if (t->CCMR1 & TIM_CCMR1_CC1S_1) // channel 2
    {
        if (t->SR & TIM_SR_CC2IF)
        {
            if (!m_flag)
            {
                m_flag = true;
                t->SR &= ~TIM_SR_CC2IF;
                return 0;
            }
            m_dutyCycle = t->CCR1;
            m_period = t->CCR2;
        }
    }
    else
    {
        if (t->SR & TIM_SR_CC1IF)
        {
            if (!m_flag)
            {
                m_flag = true;
                t->SR &= ~TIM_SR_CC1IF;
                return 0;
            }
            m_dutyCycle = t->CCR2;
            m_period = t->CCR1;
        }
    }
    return dutyCycle();
}

#if __FPU_PRESENT
float PwmInput::dutyCycle() const
{
    if (!m_period)
        return 0;
    return (float)m_dutyCycle / (float)m_period;
}

float PwmInput::percent() const
{
    return 100 * dutyCycle();
}

float PwmInput::period_ms() const
{
    return m_period * 1000.f * (prescaler() + 1) / inputClk();
}

float PwmInput::frequency() const
{
    if (!m_period)
        return 0;
    float psc = prescaler() + 1;
    return inputClk() / (psc * m_period);
}

#else
int PwmInput::dutyCycle()
{
    if (!m_period)
        return 0;
    return (m_dutyCycle << 16) / m_period;
}

int PwmInput::percent() const
{
    return m_dutyCycle * 100 / m_period;
}

int PwmInput::period_ms() const
{
    return m_period * (prescaler() + 1) / (inputClk() / 1000);
}

int PwmInput::period_us() const
{
    return m_period * 1000 * (prescaler() + 1) / (inputClk() / 1000);
}

int PwmInput::frequency() const
{
    if (!m_period)
        return 0;
    int psc = prescaler() + 1;
    return inputClk() / (psc * m_period);
}

#endif
