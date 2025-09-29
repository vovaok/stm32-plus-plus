#include "quaternion.h"
#include <cmath>

float Quaternion::length() const
{
    return sqrtf(xp * xp + yp * yp + zp * zp + wp * wp);
}

float Quaternion::lengthSquared() const
{
    return xp * xp + yp * yp + zp * zp + wp * wp;
}

Quaternion Quaternion::normalized() const
{
//    // Need some extra precision if the length is very small.
//    double len = double(xp) * double(xp) +
//                 double(yp) * double(yp) +
//                 double(zp) * double(zp) +
//                 double(wp) * double(wp);
    float len = sqrtf(xp * xp + yp * yp + zp * zp + wp * wp);
    if (len)
        return *this / sqrtf(len);
    else
        return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
}

void Quaternion::normalize()
{
//    // Need some extra precision if the length is very small.
//    double len = double(xp) * double(xp) +
//                 double(yp) * double(yp) +
//                 double(zp) * double(zp) +
//                 double(wp) * double(wp);
  
    float len = sqrtf(xp * xp + yp * yp + zp * zp + wp * wp);
    if (len == 0)
        return;

    len = sqrtf(len);

    xp /= len;
    yp /= len;
    zp /= len;
    wp /= len;
}

Quaternion Quaternion::fromAxisAndAngle(float x, float y, float z, float angle)
{
    float length = sqrtf(x * x + y * y + z * z);
    float a = 0.5f * angle * M_PI / 180.0f;
    float s = length? (sinf(a) / length): 0;
    float c = cosf(a);
    return Quaternion(c, x * s, y * s, z * s).normalized();
}

void Quaternion::getEulerAngles(float *pitch, float *yaw, float *roll) const
{
    // Algorithm adapted from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q37

    const float len = length();
    const bool rescale = len;
    const float xps = rescale ? xp / len : xp;
    const float yps = rescale ? yp / len : yp;
    const float zps = rescale ? zp / len : zp;
    const float wps = rescale ? wp / len : wp;

    const float xx = xps * xps;
    const float xy = xps * yps;
    const float xz = xps * zps;
    const float xw = xps * wps;
    const float yy = yps * yps;
    const float yz = yps * zps;
    const float yw = yps * wps;
    const float zz = zps * zps;
    const float zw = zps * wps;

    const float sinp = -2.0f * (yz - xw);
    if (abs(sinp) >= 1.0f)
        *pitch = copysign(M_PI_2, sinp);
    else
        *pitch = asinf(sinp);
    if (*pitch < M_PI_2) {
        if (*pitch > -M_PI_2) {
            *yaw = atan2f(2.0f * (xz + yw), 1.0f - 2.0f * (xx + yy));
            *roll = atan2f(2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz));
        } else {
            // not a unique solution
            *roll = 0.0f;
            *yaw = -atan2f(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
        }
    } else {
        // not a unique solution
        *roll = 0.0f;
        *yaw = atan2f(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
    }

    *pitch = *pitch * 180.0f / M_PI;
    *yaw = *yaw * 180.0f / M_PI;
    *roll = *roll * 180.0f / M_PI;
}

/*!
    Creates a quaternion that corresponds to a rotation of
    \a roll degrees around the z axis, \a pitch degrees around the x axis,
    and \a yaw degrees around the y axis (in that order).
*/
Quaternion Quaternion::fromEulerAngles(float pitch, float yaw, float roll)
{
    // Algorithm from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q60
    pitch *= M_PI / 180 / 2;
    yaw *= M_PI / 180 / 2;
    roll *= M_PI / 180 / 2;
    const float c1 = cosf(yaw);
    const float s1 = sinf(yaw);
    const float c2 = cosf(roll);
    const float s2 = sinf(roll);
    const float c3 = cosf(pitch);
    const float s3 = sinf(pitch);
    const float c1c2 = c1 * c2;
    const float s1s2 = s1 * s2;
    const float w = c1c2 * c3 + s1s2 * s3;
    const float x = c1c2 * s3 + s1s2 * c3;
    const float y = s1 * c2 * c3 - c1 * s2 * s3;
    const float z = c1 * s2 * c3 - s1 * c2 * s3;
    return Quaternion(w, x, y, z);
}
