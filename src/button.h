#ifndef _BUTTON_H
#define _BUTTON_H

#include "gpio.h"

/*! ������.
    ������ ������������ � ����� ����� �����������, � � ������� ����� ������ � ��� ����� ��������.
    ����� ������������ ������ ���������� ������� ��� �����, ��������, Gpio::PE10, ���������� �� �������� � ��������������.
    ����� ����� ��������� ���������, �������� ������� ������� � ����������.
*/
class Button : private Gpio
{
private:
    bool mState;
    bool mInverted;
    int mDebounceTime;
    int mTime;
    NotifyEvent mPressEvent;
    NotifyEvent mReleaseEvent;
    
    void task();// stmApp task event
    void tick(int period); // stmApp tick event
  
public:
    /*! ����������� ������.
        �������������� �������� ����� �� ���� � ������������� �������� �����, ���� ��� �������.
        \param pin ��� �����, � ������� ���������� ������, ��������, Gpio::PE10.
        \param pullUp ��������: 
                      \arg true �������� ����� ������������; 
                      \arg false (�� ���������) ��� ��������� �������� (���������� �������).
        \param inverted �������������� �����: 
                      \arg true (�� ���������) ������� ������ �������������� ��� 0, ���������� ��� 1;
                      \arg false ��������: ������� ������ �������������� ��� 1, ���������� ��� 0.
    */
    Button(PinName pin, bool pullUp=false, bool inverted=true);
    
    /*! ���������� �������.
        ����� ��� ���� �� �������������������. :)
    */
    ~Button();
    
    /*! ��������� ������� �������� ���������.
        ������� ��������� ���������� ��� ������� ������ � ������ ��������� �����.
        � ������� ����� ������� ��������� ����� ������������.
        \param timeMs ����� �������� ��������� � ������������� (�� ���������: 50��).
                      ��� ������ ������ ��������, ��� ������� ������ �������, �� �������� ��������� �������.
                      ���� ����� �������� ��������� ������� �������, ������ ���������� ����������� �� ��������������� �������.
    */
    void setDebounceTime(int timeMs) {mDebounceTime = timeMs;}
    
    /*! �������� ��������� ������.
        ���������� ������ ������ ��� �������� � ������ ��������������.
        \return \arg true ������; \arg false ��������.
    */
    bool state() const;
    
    /*! ��������� ����������� ������� �������.
        \param event ��������� �� �������, ������� ����������� ������������.\n
        ������ �������������: \code setPressEvent(EVENT(&MyClass::MyPressHandler)); \endcode
    */
    void setPressEvent(NotifyEvent event);
    
    /*! ��������� ����������� ������� ����������.
        \param event ��������� �� �������, ������� ����������� ������������.\n
        ������ �������������: \code setReleaseEvent(EVENT(&MyClass::MyReleaseHandler)); \endcode
    */
    void setReleaseEvent(NotifyEvent event);
};

#endif