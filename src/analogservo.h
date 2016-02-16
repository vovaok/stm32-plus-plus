#ifndef _ANALOGSERVO_H
#define _ANALOGSERVO_H

#include "hardtimer.h"
#include "gpio.h"
#include <vector>

/*!  ласс серводвигател€.
    ќбъект данного класса нельз€ создать €вно.
    Ќеобходимо создать объект класса AnalogServo, использу€ его метод addServo(), добавить серводвигатель и получить указатель на него.
    “еперь можно обращатьс€ непосредственно к объекту серводвигател€, чтобы устанавливать параметры.
*/
class Servo : public Gpio
{
private:
    int mFreq;
    int mValue;
    int mMin;
    int mMax;
    int mPosition; // дл€ хранени€ заданного положени€
//    bool mEnabled;
    
    Servo(PinName pin, int frequency_Hz=50);
    
    friend class AnalogServo;
    
public:
    /*! –азрешение управлени€ серводвигателем.
        ≈сли нет разрешени€, вход серводвигател€ подт€гиваетс€ к питанию через сопротивление.
        ≈сли же разрешение есть, на вход серводвигател€ выдаЄтс€ сигнал Ў»ћ.
        \param enabled \arg true разрешить управление; \arg false запретить управление.
    */
    void setEnabled(bool enabled);
    
    /*! ”становка диапазона допустимых положений.
        ѕоложение серводвигател€ может мен€тьс€ от 0 до 255.
        ≈сли существуют механические ограничени€, то лучше задать их программно, чтобы предотвратить выход серводвигател€ из стро€.
        \param min ћинимальное положение (по умолчанию 0).
        \param max ћаксимальное положение (по умолчанию 255)
    */
    void setRange(int min, int max);
    
    /*! ”становка текущего положени€.
        Ћогическое положение задаЄтс€ в пределах от 0 до 255, при этом физическое мен€етс€ в пределах заданного минимума и максимума (см. setRange()).
        \param pos заданное положение.
    */
    void setPosition(int pos);
    
    /*! “екущее логическое положение.
        \return установленное логическое положение серводвигател€ в пределах от 0 до 255.
    */
    int position() const {return mPosition;}
    
    /*! ”становка текущего положени€ без ограничений.
        ѕозвол€ет обойти ограничени€ при задании положени€. ћожет быть полезно при калибровке серводвигател€.
        \param value «адание положени€ (в пределах от 0 до 255).
    */
    void setValue(int value);
    
    /*! “екущее положение (физическое).
        \return положение серводвигател€ в абсолютных пределах (от 0 до 255).
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