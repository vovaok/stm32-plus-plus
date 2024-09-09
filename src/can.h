#ifndef _CAN_H
#define _CAN_H

#include "caninterface.h"
#include "gpio.h"
#include "core/core.h"

///*! ��� ����������� ����� ���������. */
//typedef Closure<void(int,CanRxMsg &)> CanReceiveEvent;

extern "C"
{
void CAN1_RX0_IRQHandler();
void CAN1_RX1_IRQHandler();
void CAN1_TX_IRQHandler();
void CAN2_RX0_IRQHandler();
void CAN2_RX1_IRQHandler();
void CAN2_TX_IRQHandler();
}

/*! CAN (Controller Area Network).
    �������������� ��������� CAN, ������������ ������ � ��� ������������
    � �������� �� ���� � �������� ���������.
*/
class Can : public CanInterface
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

    /*! CAN constructor by pins only
        Creates CAN peripheral interface using the specified pins
        \param pinRx        Predefined config for Rx pin
        \param pinRx        Predefined config for Tx pin
        \param baudrate     Desired baudrate, 1M by default
    */
    Can(Gpio::Config pinRx, Gpio::Config pinTx, int baudrate=1000000);

    /*! CAN destructor.

    */
    ~Can();

    virtual bool hasFD() override {return false;}
    
    /*! Add mask mode filter to bank.
        Message is filtered when receiving by matching of message ID and filter ID applying the mask.
        Bits of ID's must match where the mask bits are 1. \n
        \b Note: At least one filter should be configured before message reception.
        \param id Filter ID.
        \param mask Filter id matching mask, by default all bits must match.
        \param fifoNumber Number of associated FIFO, may be 0 or 1.
        \return The index of the filter that can be used e.g. for removing.
    */
    virtual int configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel) override;

    /*! �������� �������.
        \param [in] index ����� �������, ������� ����� �����.
        ���������� ���������� ��������, ���������� � ������� addFilter().
    */
    virtual bool removeFilter(int index) override;

    virtual int pendingMessageLength(int fifoChannel) override;
    virtual int receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel) override;
    virtual bool transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size) override;
    
    virtual bool open(Device::OpenMode mode=Device::ReadWrite) override; // set to normal mode
    virtual bool close() override; // set to init mode
    
//    /*! Send a message.
//        \param [in] msg The message to be transmitted. You must complete ExtId, DLC and Data (optional) fields before passing it to the function.
//        \return true if the message successfully placed into TX FIFO, otherwise returns false.
//    */
//    bool send(CanTxMsg &msg);
//
//    /*! Receive a message.
//        \param [in] fifoNumber Number of RX FIFO to retrieve message, can be 0 or 1.
//        \param [out] msg The received message if successful.
//        \return true if the message successfully retrieved from specified RX FIFO, if FIFO is empty return false.
//    */
//    bool receive(unsigned char fifoNumber, CanRxMsg &msg);
//
//    /*! ������� ��� ����������� (TX mailboxes) (��� ��� �����).
//        ��� �������������� ��������� ����������.
//    */
//    void flush();
//
//    /*! ������� ���������� ������� ����� (RX FIFO).
//        ������� �������� ��������� �� ��������� RX FIFO.
//        \param fifoNumber ����� RX FIFO ��� �������, ����� ���� 0 ��� 1.
//    */
//    void free(unsigned char fifoNumber);

//    /*! ��������� ��������� ������ ��������� CAN.
//        \return ��� ������.
//    */
//    ErrorCode lastError() const {return static_cast<ErrorCode>((((uint8_t)m_can->ESR) & (uint8_t)CAN_ESR_LEC) >> 4);}
//
//    /*! ���������� ������ ��������.
//        \return �������� �������� ������ �������� (TEC - transmit error counter).
//    */
//    int tecValue() const {return (uint8_t)((m_can->ESR & CAN_ESR_TEC) >> 16);}
//
//    /*! ���������� ������ �����.
//        In case of an error during reception, this counter is incremented by 1 or by 8 depending on the error condition
//        as defined by the CAN standard. After every successful reception, the counter is decremented by 1 or reset to 120
//        if its value was higher than 128. When the counter value exceeds 127, the CAN controller enters the error passive state.
//        \return �������� �������� ������ ����� (REC - receive error counter).
//    */
//    int recValue() const {return (uint8_t)((m_can->ESR & CAN_ESR_REC)>> 24);}

//    /*! ������������� ���������� ����� ���������.
//        � ���������� ���������� ����� RX FIFO � �������� ���������.
//        \param [in] event ��������� �� ����� ���� \c void \c myHandler(int,CanRxMsg&). ��������������� ����� ��������� �������: \c setReceiveEvent(EVENT(&MyClass::myHandler));
//    */
//    void setReceiveEvent(CanReceiveEvent event);
//    /*! ���������� ���������� ����� ��������� */
//    CanReceiveEvent receiveEvent() {return mReceiveEvent;}

//    /*! ������������� ���������� ������������ �����������.
//        \param [in] event ��������� �� ����� ���� \c void \c myHandler(). ��������������� ����� ��������� �������: \c setTransmitReadyEvent(EVENT(&MyClass::myHandler));
//    */
//    void setTransmitReadyEvent(NotifyEvent event);
//    /*! ���������� ���������� ������������ �����������. */
//    NotifyEvent transmitReadyEvent() {return mTransmitReadyEvent;}

//    /*! ���������� ����� ����� �������� �������. */
//    unsigned long packetsReceived() const   {return mPacketsReceived;}
//    /*! ���������� ����� ����� ������� ��������� �������. */
//    unsigned long packetsSent() const       {return mPacketsSent;}
//    /*! ���������� ����� ����� �� ��������� ��-�� ������ �������. */
//    unsigned long packetsSendFailed() const {return mPacketsSendFailed;}

//    /*! ������ � ���������� ������ �� ������ ���������.
//        \param [in] canNumber ����� ��������� CAN, ���������� ��������: 1 ��� 2.
//        \return ��������� �� ��������� ������, ���� �� ������. � ��������� ������ ���������� 0.
//    */
//    static Can* instance(int canNumber) {return canNumber>0 && canNumber<=2? mInstances[canNumber-1]: 0L;}

    virtual void setRxInterruptEnabled(int fifoChannel, bool enabled) override;
    
private:
    static Can *m_instances[2];
    CAN_TypeDef *m_can;
    int m_firstFilterIdx = 0;
//    CanReceiveEvent mReceiveEvent;
//    NotifyEvent mTransmitReadyEvent;

//    uint32_t mPacketsReceived;
//    uint32_t mPacketsSent;
//    uint32_t mPacketsSendFailed;
    
    bool isRxMessagePending(int fifoChannel);
//    void setRxInterruptEnabled(int fifoChannel, bool enabled);
//    void setTxInterruptEnabled(bool enabled);
    
    friend void CAN1_RX0_IRQHandler();
    friend void CAN1_RX1_IRQHandler();
    friend void CAN1_TX_IRQHandler();
    friend void CAN2_RX0_IRQHandler();
    friend void CAN2_RX1_IRQHandler();
    friend void CAN2_TX_IRQHandler();
};

#endif
