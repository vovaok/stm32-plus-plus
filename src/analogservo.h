#ifndef _ANALOGSERVO_H
#define _ANALOGSERVO_H

#include "hardtimer.h"
#include "gpio.h"
#include <vector>

/*! Класс серводвигателя.
    Объект данного класса нельзя создать явно.
    Необходимо создать объект класса AnalogServo, используя его метод addServo(), добавить серводвигатель и получить указатель на него.
    Теперь можно обращаться непосредственно к объекту серводвигателя, чтобы устанавливать параметры.
*/
class Servo : public Gpio
{
private:
    int mFreq;
    int mValue;
    int mMin;
    int mMax;
    int mPosition; // для хранения заданного положения
    bool mEnabled;
    
    Servo(PinName pin, int frequency_Hz=50);
    
    friend class AnalogServo;
    
public:
    /*! Разрешение управления серводвигателем.
        Если нет разрешения, вход серводвигателя подтягивается к питанию через сопротивление.
        Если же разрешение есть, на вход серводвигателя выдаётся сигнал ШИМ.
        \param enabled \arg true разрешить управление; \arg false запретить управление.
    */
    void setEnabled(bool enabled);
    
    /*! Проверка разрешения управления серводвигателем.
        \return true, если на серводвигатель выдаётся ШИМ; false, если выход подтянут к питанию.
    */
    bool isEnabled() {return mEnabled;}
    
    /*! Установка диапазона допустимых положений.
        Положение серводвигателя может меняться от 0 до 255.
        Если существуют механические ограничения, то лучше задать их программно, чтобы предотвратить выход серводвигателя из строя.
        \param min Минимальное положение (по умолчанию 0).
        \param max Максимальное положение (по умолчанию 255)
    */
    void setRange(int min, int max);
    
    /*! Установка текущего положения.
        Логическое положение задаётся в пределах от 0 до 255, при этом физическое меняется в пределах заданного минимума и максимума (см. setRange()).
        \param pos заданное положение.
    */
    void setPosition(int pos);
    
    /*! Текущее логическое положение.
        \return установленное логическое положение серводвигателя в пределах от 0 до 255.
    */
    int position() const {return mPosition;}
    
    /*! Установка текущего положения без ограничений. Было. Теперь с ограничениями.
        Позволяет обойти ограничения при задании положения. Может быть полезно при калибровке серводвигателя.
        \param value Задание положения (в пределах от 0 до 255).
    */
    void setValue(int value);
    
    /*! Текущее положение (физическое).
        \return положение серводвигателя в абсолютных пределах (от 0 до 255).
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
