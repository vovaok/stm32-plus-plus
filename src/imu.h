#ifndef _IMU_H
#define _IMU_H

#include "core/quaternion.h"

class Imu
{  
public:
	Imu();
    
    bool isAccValid() const {return m_accValid;}
    bool isGyroValid() const {return m_gyroValid;}
    bool isMagValid() const {return m_magValid;}
    bool isValid() const {return m_valid;}
    
    virtual void update(float dt) = 0;
    
    const Quaternion &quat() {return m_quat;}
    
    const float *acc() const {return m_acc;}
    float accX() const {return m_acc[0];}
    float accY() const {return m_acc[1];}
    float accZ() const {return m_acc[2];}
    
    const float *gyro() const {return m_gyro;}
    float gyroX() const {return m_gyro[0];}
    float gyroY() const {return m_gyro[1];}
    float gyroZ() const {return m_gyro[2];}
    
    const float *mag() const {return m_mag;}
    float magX() const {return m_mag[0];}
    float magY() const {return m_mag[1];}
    float magZ() const {return m_mag[2];}
    
protected:
    float m_acc[3] = {0};
    float m_gyro[3] = {0};
    float m_mag[3] = {0};
    bool m_accValid = false;
    bool m_gyroValid = false;
    bool m_magValid = false;
    bool m_valid = false;
    
    void calcQuat(float dt);
    
private:    
    Quaternion m_quat;
    float q0=1.0f, q1=0.0f, q2=0.0f, q3=0.0f;
    
    const float twoKp = 2.0f * 0.5f;											// 2 * proportional gain (Kp)
    const float twoKi = 2.0f * 0.0f;											// 2 * integral gain (Ki)
    float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f;	// integral error terms scaled by Ki
    
    void MahonyAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float dt);
};

#endif