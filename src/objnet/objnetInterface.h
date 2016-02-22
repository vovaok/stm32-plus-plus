#ifndef OBJNET_INTERFACE_H
#define OBJNET_INTERFACE_H

/*! @file objnetInterface.h
    @brief Objnet interface class and types.
    Contains type definitions for objnet and handles receive/transmit queues.\n
    \b Note: The interface does not take care of memory allocation for messages,
    higher level must do this dirty work. Don't forget to free memory after use,
    otherwise program will \b crash and may \b burn your board. MUHAHAHAHAHAHA!!
    */

#include <queue>
#include "objnetmsg.h"

namespace Objnet
{

/*! Media access interface.
    Provide access to link layer of the network
*/
#ifdef __ICCARM__
class ObjnetInterface
{
#else
class ObjnetInterface : public QObject
{
    Q_OBJECT
#endif
private:

protected:
    int mMaxFrameSize; //!< Maximal data size in frame

public:
    virtual ~ObjnetInterface() {}

    /*! Place message msg into transmit queue.
        Complete fields of msg with message ID (using LocalMsgId or GlobalMsgId),
        pointer to \b existing data and its length if needed;
        then call this function to place the message into specific queue depending on
        range (local or global).
        Note that global messages have higher priority and will be sent to the network prior to local ones.
        This function is pure virtual, you must reimplement it in your interface class.
        \param [in] msg Message to send
        \return true if message was sent with no errors
    */
    virtual bool write(CommonMessage &msg) = 0;

    /*! Retrieve next local message msg from receive queue.
        If this function returns true, it means the message msg is valid
        and data is copied to \b existing buffer (user must allocate memory).
        This function is pure virtual, you must reimplement it in your interface class.
        \param [out] msg Received message
        \return true if message is valid
    */
    virtual bool read(CommonMessage &msg) = 0;

//    /*! Retrieve next global message msg from receive queue.
//        This function differs from receive(), it retrieves only global messages
//        If this function returns true, it means the message msg is valid
//        and data is copied to \b existing buffer (user must allocate memory).
//        This function is pure virtual, you must reimplement it in your interface class.
//        \param [out] msg Received message
//        \return true if message is valid
//    */
//    virtual bool receiveGlobal(GlobalMessage &msg) = 0;

    /*! Flush transmit queue.

    */
    virtual void flush() = 0;

    /*! Добавление фильтра на приём пакетов.
        При приёме сообщение фильтруется по совпадению ID сообщения и фильтра,
        после чего накладывается маска. Биты идентификаторов должны совпадать
        в тех местах, где биты маски установлены в 1. \n
        \b Замечание: чтобы принимать сообщения, необходимо сконфигурировать хотя бы один фильтр.
        \param id идентификатор фильтра.
        \param mask маска фильтра, по умолчанию все биты выставлены в 1, т.е. идентификатор должен полностью совпадать.
        \return номер фильтра, который можно использовать для удаления, например.
    */
    virtual int addFilter(unsigned long id, unsigned long mask=0xFFFFFFFF) = 0;

    /*! Удаление фильтра.
        \param [in] number номер фильтра, который будет удалён.
        Желательно передавать значение, полученное в функции addFilter().
    */
    virtual void removeFilter(int number) = 0;

    int maxFrameSize() const {return mMaxFrameSize;}
};

}

#endif
