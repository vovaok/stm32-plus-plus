#ifndef _ADVMATH_H
#define _ADVMATH_H

#include <stdint.h>

#ifndef M_PI
#define M_PI    3.1415926f
#define M_PI_2  (M_PI / 2)
#endif

extern float invSqrt(float x);
extern float sat(float x, float c);
extern int sat(int x, int c);
extern float bound(float min, float val, float max);
extern int bound(int min, int val, int max);
extern float unwrap(float x);
extern float unwrap_deg(float x);
extern float rnd(float rms); // Irwin-Hall distribution
extern int rnd(int rms); // Irwin-Hall distribution

extern int16_t sin_i16(uint8_t x);
extern int16_t cos_i16(uint8_t x);

#endif