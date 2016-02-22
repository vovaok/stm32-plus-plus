#ifndef _BUTTON_H
#define _BUTTON_H

#include "gpio.h"

/*! Кнопка.
    Кнопка подключается к любой ножке контроллера, и с помощью этого класса с ней легко работать.
    Чтобы использовать кнопку достаточно указать имя ножки, например, Gpio::PE10, необходима ли подтяжка и инвертирование.
    Затем можно считывать состояние, получать событие нажатия и отпускания.
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
    /*! Конструктор кнопки.
        Инициализирует заданную ножку на вход и устанавливает подтяжку вверх, если это указано.
        \param pin Имя ножки, к которой подключена кнопка, например, Gpio::PE10.
        \param pullUp Подтяжка: 
                      \arg true подтяжка вверх активирована; 
                      \arg false (по умолчанию) без внутреней подтяжки (необходима внешняя).
        \param inverted Инвертирование входа: 
                      \arg true (по умолчанию) нажатая кнопка воспринимается как 0, отпущенная как 1;
                      \arg false наоборот: нажатая кнопка воспринимается как 1, отпущенная как 0.
    */
    Button(PinName pin, bool pullUp=false, bool inverted=true);
    
    /*! Деструктор удаляет.
        Ножка при этом не разынициализируется. :)
    */
    ~Button();
    
    /*! Установка времени дребезга контактов.
        Дребезг контактов появляется при нажатии кнопки и длится некоторое время.
        В течение этого времени состояние входа игнорируется.
        \param timeMs Время дребезга контактов в миллисекундах (по умолчанию: 50мс).
                      Чем меньше данное значение, тем реакция кнопки быстрее, но возможно повторное нажатие.
                      Если время дребезга контактов указать большим, кнопка перестанет реагировать на кратковременное нажатие.
    */
    void setDebounceTime(int timeMs) {mDebounceTime = timeMs;}
    
    /*! Проверка состояния кнопки.
        Определяет нажата кнопка или отпущена с учётом инвертирования.
        \return \arg true нажата; \arg false отпущена.
    */
    bool state() const;
    
    /*! Установка обработчика события нажатия.
        \param event Указатель на функцию, которая назначается обработчиком.\n
        Пример использования: \code setPressEvent(EVENT(&MyClass::MyPressHandler)); \endcode
    */
    void setPressEvent(NotifyEvent event);
    
    /*! Установка обработчика события отпускания.
        \param event Указатель на функцию, которая назначается обработчиком.\n
        Пример использования: \code setReleaseEvent(EVENT(&MyClass::MyReleaseHandler)); \endcode
    */
    void setReleaseEvent(NotifyEvent event);
};

#endif