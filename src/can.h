#ifndef _CAN_H
#define _CAN_H

#include "stm32_conf.h"
#include "gpio.h"
#include "core/core.h"

/*! ��� ����������� ����� ���������. */
typedef Closure<void(int,CanRxMsg &)> CanReceiveEvent;

/*! CAN (Controller Area Network).
    �������������� ��������� CAN, ������������ ������ � ��� ������������
    � �������� �� ���� � �������� ���������.
*/
class Can
{
public:
    /*! ���� ������ ��������� CAN.*/
    typedef enum
    {
        errNone         = 0x0, //!< ��� ������
        errStuff        = 0x1, //!< ������ �����-�� �����
        errForm         = 0x2, //!< �������� ������ ���������
        errAck          = 0x3, //!< ��� ������, ��� ��������� ��������� �����
        errBitRecessive = 0x4, //!< ��
        errBitDominant  = 0x5, //!< ��
        errCrc          = 0x6, //!< ������ ����������� �����
        errSoftware     = 0x7, //!< ��������������� ������������� ������
    } ErrorCode;
  
private:
    static Can *mInstances[2];
    CAN_TypeDef *mCan;
    int mStartFilter;
    unsigned long mFilterUsed;
    CanReceiveEvent mReceiveEvent;
    NotifyEvent mTransmitReadyEvent;
    
    unsigned long mPacketsReceived;
    unsigned long mPacketsSent;
    unsigned long mPacketsSendFailed;
  
public:  
    /*! CAN constructor.
        Creates CAN peripheral interface to CANx with specified baudrate and
        maps it to default or given pins.
        \param canNumber    Number of CAN interface, can be 1 or 2. Passing invalid number will cause initialization fail.
        \param baudrate     Desired baudrate in bits per second. Standard values are 125000 (125 kbps), 250000 (250 kbps), 500000 (500 kbps), 1000000 (1 Mbps) but you can use your own baudrate.
        \param pinRx        Predefined config for Rx pin, if not passed, default pin will be used: PD0 for CAN1, PB5 for CAN2
        \param pinRx        Predefined config for Tx pin, if not passed, default pin will be used: PD1 for CAN1, PB6 for CAN2
    */
    Can(int canNumber, int baudrate, Gpio::Config pinRx=Gpio::NoConfig, Gpio::Config pinTx=Gpio::NoConfig);
    
    /*! CAN destructor.
    
    */
    ~Can();
    
    /*! Add mask mode filter to bank.
        Message is filtered when receiving by matching of message ID and filter ID applying the mask. Bits of ID's must match where the mask bits are 1. \n
        \b Note: At least one filter should be configured before message reception.
        \param id Filter ID.
        \param mask Filter id matching mask, by default all bits must match.
        \param fifoNumber Number of associated FIFO, may be 0 or 1.
        \return ����� �������, ������� ����� ������������ ��� ��������, ��������.
    */
    int addFilter(unsigned long id, unsigned long mask=0xFFFFFFFF, int fifoNumber=0);
    
    /*! �������� �������.
        \param [in] number ����� �������, ������� ����� �����.
        ���������� ���������� ��������, ���������� � ������� addFilter().
    */
    void removeFilter(int number);
    
    /*! Send a message.
        \param [in] msg The message to be transmitted. You must complete ExtId, DLC and Data (optional) fields before passing it to the function.
        \return true if the message successfully placed into TX FIFO, otherwise returns false.
    */
    bool send(CanTxMsg &msg);
    
    /*! Receive a message.
        \param [in] fifoNumber Number of RX FIFO to retrieve message, can be 0 or 1.
        \param [out] msg The received message if successful.
        \return true if the message successfully retrieved from specified RX FIFO, if FIFO is empty return false.
    */
    bool receive(unsigned char fifoNumber, CanRxMsg &msg);
    
    /*! ������� ��� ����������� (TX mailboxes) (��� ��� �����).
        ��� �������������� ��������� ����������.
    */
    void flush();
    
    /*! ������� ���������� ������� ����� (RX FIFO).
        ������� �������� ��������� �� ��������� RX FIFO.
        \param fifoNumber ����� RX FIFO ��� �������, ����� ���� 0 ��� 1.
    */
    void free(unsigned char fifoNumber);
    
    /*! ��������� ��������� ������ ��������� CAN.
        \return ��� ������.
    */
    ErrorCode lastError() const {return static_cast<ErrorCode>(CAN_GetLastErrorCode(mCan) >> 4);}
    
    /*! ���������� ������ ��������.
        \return �������� �������� ������ �������� (TEC - transmit error counter).
    */
    int tecValue() const {return CAN_GetLSBTransmitErrorCounter(mCan);}
    
    /*! ���������� ������ �����.
        In case of an error during reception, this counter is incremented by 1 or by 8 depending on the error condition
        as defined by the CAN standard. After every successful reception, the counter is decremented by 1 or reset to 120
        if its value was higher than 128. When the counter value exceeds 127, the CAN controller enters the error passive state.
        \return �������� �������� ������ ����� (REC - receive error counter).
    */
    int recValue() const {return CAN_GetReceiveErrorCounter(mCan);}
    
    /*! ������������� ���������� ����� ���������.
        � ���������� ���������� ����� RX FIFO � �������� ���������.
        \param [in] event ��������� �� ����� ���� \c void \c myHandler(int,CanRxMsg&). ��������������� ����� ��������� �������: \c setReceiveEvent(EVENT(&MyClass::myHandler));
    */
    void setReceiveEvent(CanReceiveEvent event);
    /*! ���������� ���������� ����� ��������� */
    CanReceiveEvent receiveEvent() {return mReceiveEvent;}
    
    /*! ������������� ���������� ������������ �����������.
        \param [in] event ��������� �� ����� ���� \c void \c myHandler(). ��������������� ����� ��������� �������: \c setTransmitReadyEvent(EVENT(&MyClass::myHandler));
    */
    void setTransmitReadyEvent(NotifyEvent event);
    /*! ���������� ���������� ������������ �����������. */
    NotifyEvent transmitReadyEvent() {return mTransmitReadyEvent;}
    
    /*! ���������� ����� ����� �������� �������. */
    unsigned long packetsReceived() const   {return mPacketsReceived;}
    /*! ���������� ����� ����� ������� ��������� �������. */
    unsigned long packetsSent() const       {return mPacketsSent;}
    /*! ���������� ����� ����� �� ��������� ��-�� ������ �������. */
    unsigned long packetsSendFailed() const {return mPacketsSendFailed;}
    
    /*! ������ � ���������� ������ �� ������ ���������.
        \param [in] canNumber ����� ��������� CAN, ���������� ��������: 1 ��� 2.
        \return ��������� �� ��������� ������, ���� �� ������. � ��������� ������ ���������� 0.
    */
    static Can* instance(int canNumber) {return canNumber>0 && canNumber<=2? mInstances[canNumber-1]: 0L;}
};

#endif