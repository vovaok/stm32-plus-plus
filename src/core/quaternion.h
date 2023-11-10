#ifndef _QUATERNION_H
#define _QUATERNION_H

#include <math.h>
#include "advmath.h"

class Quaternion
{
public:
    Quaternion();
    Quaternion(float scalar, float x, float y, float z);
    
    float x() const;
    float y() const;
    float z() const;
    float scalar() const;
    
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setScalar(float scalar);

    float length() const;
    float lengthSquared() const;

    Quaternion normalized() const;
    void normalize();

    Quaternion conjugate() const;
    Quaternion conjugated() const;
    
    Quaternion &operator+=(const Quaternion &quaternion);
    Quaternion &operator-=(const Quaternion &quaternion);
    Quaternion &operator*=(float factor);
    Quaternion &operator*=(const Quaternion &quaternion);
    Quaternion &operator/=(float divisor);

    friend inline bool operator==(const Quaternion &q1, const Quaternion &q2);
    friend inline bool operator!=(const Quaternion &q1, const Quaternion &q2);
    friend inline const Quaternion operator+(const Quaternion &q1, const Quaternion &q2);
    friend inline const Quaternion operator-(const Quaternion &q1, const Quaternion &q2);
    friend inline const Quaternion operator*(float factor, const Quaternion &quaternion);
    friend inline const Quaternion operator*(const Quaternion &quaternion, float factor);
    friend inline const Quaternion operator*(const Quaternion &q1, const Quaternion& q2);
    friend inline const Quaternion operator-(const Quaternion &quaternion);
    friend inline const Quaternion operator/(const Quaternion &quaternion, float divisor);
    
    static Quaternion fromAxisAndAngle(float x, float y, float z, float angle);
    
    void getEulerAngles(float *pitch, float *yaw, float *roll) const;
    static Quaternion fromEulerAngles(float pitch, float yaw, float roll);
    
private:
    float wp, xp, yp, zp;
};

inline Quaternion::Quaternion() : wp(1.0f), xp(0.0f), yp(0.0f), zp(0.0f) {}

inline Quaternion::Quaternion(float scalar, float x, float y, float z) : wp(scalar), xp(x), yp(y), zp(z) {}

inline float Quaternion::x() const { return float(xp); }
inline float Quaternion::y() const { return float(yp); }
inline float Quaternion::z() const { return float(zp); }
inline float Quaternion::scalar() const { return float(wp); }

inline void Quaternion::setX(float aX) { xp = aX; }
inline void Quaternion::setY(float aY) { yp = aY; }
inline void Quaternion::setZ(float aZ) { zp = aZ; }
inline void Quaternion::setScalar(float aScalar) { wp = aScalar; }

inline Quaternion Quaternion::conjugated() const
{
    return Quaternion(wp, -xp, -yp, -zp);
}

inline Quaternion Quaternion::conjugate() const
{
    return Quaternion(wp, -xp, -yp, -zp);
}

inline Quaternion &Quaternion::operator+=(const Quaternion &q)
{
    xp += q.xp;
    yp += q.yp;
    zp += q.zp;
    wp += q.wp;
    return *this;
}

inline Quaternion &Quaternion::operator-=(const Quaternion &q)
{
    xp -= q.xp;
    yp -= q.yp;
    zp -= q.zp;
    wp -= q.wp;
    return *this;
}

inline Quaternion &Quaternion::operator*=(float factor)
{
    xp *= factor;
    yp *= factor;
    zp *= factor;
    wp *= factor;
    return *this;
}

inline const Quaternion operator*(const Quaternion &q1, const Quaternion& q2)
{
//    float ww = (q1.zp + q1.xp) * (q2.xp + q2.yp);
//    float yy = (q1.wp - q1.yp) * (q2.wp + q2.zp);
//    float zz = (q1.wp + q1.yp) * (q2.wp - q2.zp);
//    float xx = ww + yy + zz;
//    float qq = 0.5 * (xx + (q1.zp - q1.xp) * (q2.xp - q2.yp));
//
//    float w = qq - ww + (q1.zp - q1.yp) * (q2.yp - q2.zp);
//    float x = qq - xx + (q1.xp + q1.wp) * (q2.xp + q2.wp);
//    float y = qq - yy + (q1.wp - q1.xp) * (q2.yp + q2.zp);
//    float z = qq - zz + (q1.zp + q1.yp) * (q2.wp - q2.xp);
  
    float w = q1.wp * q2.wp - q1.xp * q2.xp - q1.yp * q2.yp - q1.zp * q2.zp;
    float x = q1.wp * q2.xp + q1.xp * q2.wp + q1.yp * q2.zp - q1.zp * q2.yp;
    float y = q1.wp * q2.yp - q1.xp * q2.zp + q1.yp * q2.wp + q1.zp * q2.xp;
    float z = q1.wp * q2.zp + q1.xp * q2.yp - q1.yp * q2.xp + q1.zp * q2.wp;

    return Quaternion(w, x, y, z);
}

inline Quaternion &Quaternion::operator*=(const Quaternion &q)
{
    *this = *this * q;
    return *this;
}

inline Quaternion &Quaternion::operator/=(float divisor)
{
    xp /= divisor;
    yp /= divisor;
    zp /= divisor;
    wp /= divisor;
    return *this;
}

inline bool operator==(const Quaternion &q1, const Quaternion &q2)
{
    return q1.xp == q2.xp && q1.yp == q2.yp && q1.zp == q2.zp && q1.wp == q2.wp;
}

inline bool operator!=(const Quaternion &q1, const Quaternion &q2)
{
    return q1.xp != q2.xp || q1.yp != q2.yp || q1.zp != q2.zp || q1.wp != q2.wp;
}

inline const Quaternion operator+(const Quaternion &q1, const Quaternion &q2)
{
    return Quaternion(q1.wp + q2.wp, q1.xp + q2.xp, q1.yp + q2.yp, q1.zp + q2.zp);
}

inline const Quaternion operator-(const Quaternion &q1, const Quaternion &q2)
{
    return Quaternion(q1.wp - q2.wp, q1.xp - q2.xp, q1.yp - q2.yp, q1.zp - q2.zp);
}

inline const Quaternion operator*(float factor, const Quaternion &q)
{
    return Quaternion(q.wp * factor, q.xp * factor, q.yp * factor, q.zp * factor);
}

inline const Quaternion operator*(const Quaternion &q, float factor)
{
    return Quaternion(q.wp * factor, q.xp * factor, q.yp * factor, q.zp * factor);
}

inline const Quaternion operator-(const Quaternion &q)
{
    return Quaternion(-q.wp, -q.xp, -q.yp, -q.zp);
}

inline const Quaternion operator/(const Quaternion &q, float divisor)
{
    return Quaternion(q.wp / divisor, q.xp / divisor, q.yp / divisor, q.zp / divisor);
}

#endif