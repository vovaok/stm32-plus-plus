#include "advmath.h"
#include <stdlib.h>

static const uint16_t sin_16_8b_Table[256] =
{
  0x0, 0x324, 0x648, 0x96b, 0xc8c, 0xfab, 0x12c8,
  0x15e2, 0x18f9, 0x1c0c, 0x1f1a, 0x2224, 0x2528, 0x2827, 0x2b1f,
  0x2e11, 0x30fc, 0x33df, 0x36ba, 0x398d, 0x3c57, 0x3f17, 0x41ce,
  0x447b, 0x471d, 0x49b4, 0x4c40, 0x4ec0, 0x5134, 0x539b, 0x55f6,
  0x5843, 0x5a82, 0x5cb4, 0x5ed7, 0x60ec, 0x62f2, 0x64e9, 0x66d0,
  0x68a7, 0x6a6e, 0x6c24, 0x6dca, 0x6f5f, 0x70e3, 0x7255, 0x73b6,
  0x7505, 0x7642, 0x776c, 0x7885, 0x798a, 0x7a7d, 0x7b5d, 0x7c2a,
  0x7ce4, 0x7d8a, 0x7e1e, 0x7e9d, 0x7f0a, 0x7f62, 0x7fa7, 0x7fd9,
  0x7ff6, 0x7fff, 0x7ff6, 0x7fd9, 0x7fa7, 0x7f62, 0x7f0a, 0x7e9d,
  0x7e1e, 0x7d8a, 0x7ce4, 0x7c2a, 0x7b5d, 0x7a7d, 0x798a, 0x7885,
  0x776c, 0x7642, 0x7505, 0x73b6, 0x7255, 0x70e3, 0x6f5f, 0x6dca,
  0x6c24, 0x6a6e, 0x68a7, 0x66d0, 0x64e9, 0x62f2, 0x60ec, 0x5ed7,
  0x5cb4, 0x5a82, 0x5843, 0x55f6, 0x539b, 0x5134, 0x4ec0, 0x4c40,
  0x49b4, 0x471d, 0x447b, 0x41ce, 0x3f17, 0x3c57, 0x398d, 0x36ba,
  0x33df, 0x30fc, 0x2e11, 0x2b1f, 0x2827, 0x2528, 0x2224, 0x1f1a,
  0x1c0c, 0x18f9, 0x15e2, 0x12c8, 0xfab, 0xc8c, 0x96b, 0x648,
  0x324, 0x0, 0xfcdc, 0xf9b8, 0xf695, 0xf374, 0xf055, 0xed38,
  0xea1e, 0xe707, 0xe3f4, 0xe0e6, 0xdddc, 0xdad8, 0xd7d9, 0xd4e1,
  0xd1ef, 0xcf04, 0xcc21, 0xc946, 0xc673, 0xc3a9, 0xc0e9, 0xbe32,
  0xbb85, 0xb8e3, 0xb64c, 0xb3c0, 0xb140, 0xaecc, 0xac65, 0xaa0a,
  0xa7bd, 0xa57e, 0xa34c, 0xa129, 0x9f14, 0x9d0e, 0x9b17, 0x9930,
  0x9759, 0x9592, 0x93dc, 0x9236, 0x90a1, 0x8f1d, 0x8dab, 0x8c4a,
  0x8afb, 0x89be, 0x8894, 0x877b, 0x8676, 0x8583, 0x84a3, 0x83d6,
  0x831c, 0x8276, 0x81e2, 0x8163, 0x80f6, 0x809e, 0x8059, 0x8027,
  0x800a, 0x8000, 0x800a, 0x8027, 0x8059, 0x809e, 0x80f6, 0x8163,
  0x81e2, 0x8276, 0x831c, 0x83d6, 0x84a3, 0x8583, 0x8676, 0x877b,
  0x8894, 0x89be, 0x8afb, 0x8c4a, 0x8dab, 0x8f1d, 0x90a1, 0x9236,
  0x93dc, 0x9592, 0x9759, 0x9930, 0x9b17, 0x9d0e, 0x9f14, 0xa129,
  0xa34c, 0xa57e, 0xa7bd, 0xaa0a, 0xac65, 0xaecc, 0xb140, 0xb3c0,
  0xb64c, 0xb8e3, 0xbb85, 0xbe32, 0xc0e9, 0xc3a9, 0xc673, 0xc946,
  0xcc21, 0xcf04, 0xd1ef, 0xd4e1, 0xd7d9, 0xdad8, 0xdddc, 0xe0e6,
  0xe3f4, 0xe707, 0xea1e, 0xed38, 0xf055, 0xf374, 0xf695, 0xf9b8,
  0xfcdc
};

//! Fast inverse square-root.
//! See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
float invSqrt(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

// ! Saturate value x to c
float sat(float x, float c)
{
    if (x > c)
        return c;
    if (x < -c)
        return -c;
    return x;
}

int sat(int x, int c)
{
    if (x > c)
        return c;
    if (x < -c)
        return -c;
    return x;
}

float bound(float min, float val, float max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}

int bound(int min, int val, int max)
{
    if (val < min)
        return min;
    if (val > max)
        return max;
    return val;
}


//! Unwrap value x from [-2*M_PI; 2*M_PI) into [-M_PI; M_PI)
float unwrap(float x)
{
    if (x >= M_PI)
        x -= 2*M_PI;
    else if (x < -M_PI)
        x += 2*M_PI;
    return x;
}

float unwrap_deg(float x)
{
    if (x >= 180)
        x -= 360;
    else if (x < -180)
        x += 360;
    return x;
}

//! Irwin-Hall distribution
float rnd(float rms)
{
    float x = -6;
    for (int i=0; i<12; i++)
        x += rand() / (float)(RAND_MAX);
    return x * rms;
}

int rnd(int rms)
{
    int x = -6 * rms;
    for (int i=0; i<12; i++)
        x += rand() % rms; // rand() * rms / RAND_MAX;
    return x;
}

int16_t sin_i16(uint8_t x)
{
    return sin_16_8b_Table[x];
}

int16_t cos_i16(uint8_t x)
{
    return sin_16_8b_Table[(uint8_t)(x + 64)];
}
