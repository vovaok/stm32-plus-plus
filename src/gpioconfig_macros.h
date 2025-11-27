#pragma once

#define PINPERIPH_HELPER(num, ch) \
( \
    (((num) & 0x0F) << 4) | \
    (((num) & 0x10) >> 2) | \
    ((ch) & 0x0F) \
)

#define PINADC_HELPER(num, ch) \
( \
    (((num) & 0x7) << 4) | \
    (((ch) & 0x10) << 3) | \
    ((ch) & 0x0F) \
)

#define PINCONFIGAF(pin, af, periph_num, periph_ch) \
    PINCONFIG(pin, modeAF | outPushPull | pullNone | speed50MHz, af, \
              PINPERIPH_HELPER(periph_num, periph_ch))
        
#define PINAF(pin, num, ch)     PINCONFIGAF(pin, _CURRENT_AF, num, ch)        

#define DECL_PIN_2(periph, pin) \
    periph##_##pin = PINAF(pin, 0, 0)
#define DECL_PIN_3(periph, name, pin) \
    periph##_##name##_##pin = PINAF(pin, 0, 0)
#define DECL_PIN_4(periph, num, name, pin) \
    periph##num##_##name##_##pin = PINAF(pin, num, 0)
#define DECL_PIN_5(periph, num, name, ch, pin) \
    periph##num##_##name##ch##_##pin = PINAF(pin, num, ch)
#define DECL_PIN_6(periph, num, name, ch, suffix, pin) \
    periph##num##_##name##ch##suffix##_##pin = PINAF(pin, num, ch | 8)
        
#pragma diag_suppress=Pe1665
#define DECL_PIN_HELPER(x1, x2, x3, x4, x5, f, ...) f
#define DECL_PIN(periph, ...) DECL_PIN_HELPER(__VA_ARGS__, \
                                DECL_PIN_6(periph, __VA_ARGS__), \
                                DECL_PIN_5(periph, __VA_ARGS__), \
                                DECL_PIN_4(periph, __VA_ARGS__), \
                                DECL_PIN_3(periph, __VA_ARGS__), \
                                DECL_PIN_2(periph, __VA_ARGS__))
        
//#define DECL_TIM_CH_PIN(num, ch, pin) \
//    TIM##num##_CH##ch##_##pin = PINCONFIGAF(pin, _CURRENT_AF, num, ch)
//#define DECL_TIM_CHN_PIN(num, ch, pin) \
//    TIM##num##_CH##ch##N_##pin = PINCONFIGAF(pin, _CURRENT_AF, num, ch|8)        

#define DECL_ADC_PIN(num, ch, pin) \
    ADC##num##_IN##ch##_##pin = PINCONFIG(pin, modeAnalog, afNone, PINADC_HELPER(num, ch))