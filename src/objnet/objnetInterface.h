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
#ifndef QT_CORE_LIB
class ObjnetInterface
{
#else
class ObjnetInterface : public QObject
{
    Q_OBJECT
#endif
private:

protected:
//    typedef enum
//    {
//        IfUnknown = 0
//        IfCan,
//        IfUsbHid,
//        IfUart
//    } Type;
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
    
    /*! ������ ���������� ��������� ���� � ������ ��������.
        ���������� ��� �������� ����������� �������� ����������������� ���������
        \return ����� �������, ��������� ��� ������.
    */
    virtual int availableWriteCount() = 0;

    /*! Flush transmit queue.

    */
    virtual void flush() = 0;

    /*! ���������� ������� �� ���� �������.
        ��� ����� ��������� ����������� �� ���������� ID ��������� � �������,
        ����� ���� ������������� �����. ���� ��������������� ������ ���������
        � ��� ������, ��� ���� ����� ����������� � 1. \n
        \b ���������: ����� ��������� ���������, ���������� ���������������� ���� �� ���� ������.
        \param id ������������� �������.
        \param mask ����� �������, �� ��������� ��� ���� ���������� � 1, �.�. ������������� ������ ��������� ���������.
        \return ����� �������, ������� ����� ������������ ��� ��������, ��������.
    */
    virtual int addFilter(unsigned long id, unsigned long mask=0xFFFFFFFF) = 0;

    /*! �������� �������.
        \param [in] number ����� �������, ������� ����� �����.
        ���������� ���������� ��������, ���������� � ������� addFilter().
    */
    virtual void removeFilter(int number) = 0;

    int maxFrameSize() const {return mMaxFrameSize;}
};

}

#endif
