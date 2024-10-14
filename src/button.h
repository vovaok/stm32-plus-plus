#ifndef _BUTTON_H
#define _BUTTON_H

#include "gpio.h"
#include "core/core.h"
#include <functional>

/*! Кнопка.
    Кнопка подключается к любой ножке контроллера, и с помощью этого класса с ней легко работать.
    Чтобы использовать кнопку достаточно указать имя ножки, например, Gpio::PE10, необходима ли подтяжка и инвертирование.
    Затем можно считывать состояние, получать событие нажатия и отпускания.
*/
class Button : private Gpio
{
private:
    bool mFilter;
    bool mState;
    bool mInverted;
    int mAutoRepeatTime;
    int mDebounceTime;
    int mHoldTime;
    int mTime;
    int m_taskid, m_tickid;

    void task();// stmApp task event
    void tick(int period); // stmApp tick event

public:
    /*! Конструктор кнопки.
        Инициализирует заданную ножку на вход и устанавливает подтяжку вверх, если это указано.
        \param pin Имя ножки, к которой подключена кнопка, например, Gpio::PE10.
        \param pullUp Подтяжка:
                      \arg true (по умолчанию) подтяжка вверх активирована;
                      \arg false без внутреней подтяжки (необходима внешняя).
        \param inverted Инвертирование входа:
                      \arg true (по умолчанию) нажатая кнопка воспринимается как 0, отпущенная как 1;
                      \arg false наоборот: нажатая кнопка воспринимается как 1, отпущенная как 0.
    */
    Button(PinName pin, bool pullUp=true, bool inverted=true);
    
    Button(GPIO_TypeDef *gpio, int pin, bool inverted=true);

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

    void setAutoRepeat(int timeMs = 200) {mAutoRepeatTime = timeMs;}

    /*! Проверка состояния кнопки.
        Определяет нажата кнопка или отпущена с учётом инвертирования.
        \return \arg true нажата; \arg false отпущена.
    */
    bool state() const;

    /*! Проверка состояния удержания кнопки.
        Определяет, удерживается ли кнопка более заданного времени (300 мс).
        \return \arg true нажата; \arg false отпущена.
    */
    bool isHolding() const;

    /*! Время удержания кнопки.
        \return Время в миллисекундах удержания кнопки.
    */
    int holdTime() const;

    /*! Обработчик события нажатия.
        \param Указатель на функцию Closure<void(void)>, которая назначается обработчиком.\n
        Пример использования: \code myButton->onPress = EVENT(&MyClass::MyPressHandler); \endcode
    */
    std::function<void(void)> onPress;
//    NotifyEvent onPress;

    /*! Обработчик события отпускания.
        \param Указатель на функцию Closure<void(void)>, которая назначается обработчиком.\n
        Пример использования: \code myButton->onPress = EVENT(&MyClass::MyReleaseHandler); \endcode
    */
    std::function<void(void)> onRelease;
//    NotifyEvent onRelease;


    /*! Установка обработчика события клика.
        Кликом считается кратковременное нажатие длительностью не более 300 мс.
        При этом событие возникает при отпускании кнопки.
        \param Указатель на функцию Closure<void(void)>, которая назначается обработчиком.\n
        Пример использования: \code myButton->onPress = EVENT(&MyClass::MyReleaseHandler); \endcode
    */
    std::function<void(void)> onClick;
//    NotifyEvent onClick;

    using Gpio::read;
};

#endif
