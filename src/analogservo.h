#ifndef _ANALOGSERVO_H
#define _ANALOGSERVO_H

#include "hardwaretimer.h"
#include "gpio.h"
#include <vector>

/*! ����� ��������������.
    ������ ������� ������ ������ ������� ����.
    ���������� ������� ������ ������ AnalogServo, ��������� ��� ����� addServo(), �������� �������������� � �������� ��������� �� ����.
    ������ ����� ���������� ��������������� � ������� ��������������, ����� ������������� ���������.
*/
class Servo : public Gpio
{
private:
    int mFreq;
    int mValue;
    int mLoadedValue;
    int mMin;
    int mMax;
    int mPosition; // ��� �������� ��������� ���������
    unsigned short mOffset; // for internal use
    bool mEnabled;
    
    Servo(PinName pin, int frequency_Hz=50);
    
    friend class AnalogServo;
    
public:
    /*! ���������� ���������� ���������������.
        ���� ��� ����������, ���� �������������� ������������� � ������� ����� �������������.
        ���� �� ���������� ����, �� ���� �������������� ������� ������ ���.
        \param enabled \arg true ��������� ����������; \arg false ��������� ����������.
    */
    void setEnabled(bool enabled);
    
    /*! �������� ���������� ���������� ���������������.
        \return true, ���� �� �������������� ������� ���; false, ���� ����� �������� � �������.
    */
    bool isEnabled() {return mEnabled;}
    
    /*! ��������� ��������� ���������� ���������.
        ��������� �������������� ����� �������� �� 0 �� 255.
        ���� ���������� ������������ �����������, �� ����� ������ �� ����������, ����� ������������� ����� �������������� �� �����.
        \param min ����������� ��������� (�� ��������� 0).
        \param max ������������ ��������� (�� ��������� 255)
    */
    void setRange(int min, int max);
    
    /*! ��������� �������� ���������.
        ���������� ��������� ������� � �������� �� 0 �� 255, ��� ���� ���������� �������� � �������� ��������� �������� � ��������� (��. setRange()).
        \param pos �������� ���������.
    */
    void setPosition(int pos);
    
    /*! ������� ���������� ���������.
        \return ������������� ���������� ��������� �������������� � �������� �� 0 �� 255.
    */
    int position() const {return mPosition;}
    
    /*! ��������� �������� ��������� ��� �����������. ����. ������ � �������������.
        ��������� ������ ����������� ��� ������� ���������. ����� ���� ������� ��� ���������� ��������������.
        \param value ������� ��������� (� �������� �� 0 �� 255).
    */
    void setValue(int value);
    
    /*! ������� ��������� (����������).
        \return ��������� �������������� � ���������� �������� (�� 0 �� 255).
    */
    int value() const {return mValue;}
};
//---------------------------------------------------------------------------

class AnalogServo : private HardwareTimer
{
private:
    std::vector<Servo*> mServo;
    int mTime;
  
    void timerHandler();
  
public:
    AnalogServo(TimerNumber timerNumber);
    ~AnalogServo();
    
    void setEnabled(bool enabled);
    Servo *addServo(Gpio::PinName pin);
};

#endif
