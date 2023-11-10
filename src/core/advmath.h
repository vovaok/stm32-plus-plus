#ifndef _ADVMATH_H
#define _ADVMATH_H

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

#endif