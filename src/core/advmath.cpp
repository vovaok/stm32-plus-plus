#include "advmath.h"

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

