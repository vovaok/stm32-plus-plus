#ifndef _FDCAN_H
#define _FDCAN_H

#include "caninterface.h"
#include "gpio.h"

extern "C"
{
    void FDCAN1_IT0_IRQHandler();
    void FDCAN1_IT1_IRQHandler();
    void FDCAN2_IT0_IRQHandler();
    void FDCAN2_IT1_IRQHandler();
    void FDCAN3_IT0_IRQHandler();
    void FDCAN3_IT1_IRQHandler();
}

class FdCan : public CanInterface
{
public:
    FdCan(Gpio::Config fdcanRx, Gpio::Config fdcanTx);

    virtual bool hasFD() override {return true;}

    virtual int configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel) override;
    virtual bool removeFilter(int index) override;

    virtual int pendingMessageLength(int fifoChannel) override;
    virtual int receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel) override;
    virtual bool transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size) override;

    virtual bool setBaudrate(int value);
    bool setDataBaudrate(int value); // for CAN FD mode with BRS
    virtual bool open(Device::OpenMode mode=Device::ReadWrite) override; // set to normal mode
    virtual bool close() override; // set to init mode
    virtual bool isOpen() const override;

    virtual void setRxInterruptEnabled(int fifoChannel, bool enabled) override;

private:
    static FdCan *m_instances[3];
    FDCAN_GlobalTypeDef *m_dev = nullptr;

    friend void FDCAN1_IT0_IRQHandler();
    friend void FDCAN1_IT1_IRQHandler();
    friend void FDCAN2_IT0_IRQHandler();
    friend void FDCAN2_IT1_IRQHandler();
    friend void FDCAN3_IT0_IRQHandler();
    friend void FDCAN3_IT1_IRQHandler();

    union StdFilterElement
    {
        struct
        {
        uint32_t SFID2: 11;
        uint32_t : 5;
        uint32_t SFID1: 11;
        uint32_t SFEC: 3;
        uint32_t SFT: 2;
        };
        uint32_t word = 0;
    };

    union ExtFilterElement
    {
        struct
        {
        uint32_t EFID1: 29;
        uint32_t EFEC: 3;
        uint32_t EFID2: 29;
        uint32_t : 1;
        uint32_t EFT: 2;
        };
        uint32_t word[2] = {0, 0};
    };

    union RxFifoElement
    {
        struct
        {
        uint32_t ID: 29;
        uint32_t RTR: 1;
        uint32_t XTD: 1;
        uint32_t ESI: 1;

        uint32_t RXTS: 16;
        uint32_t DLC: 4;
        uint32_t BRS: 1;
        uint32_t FDF: 1;
        uint32_t : 2;
        uint32_t FIDX: 7;
        uint32_t ANMF: 1;

        uint32_t data[16];
        };
        uint32_t words[18];
    };

    union TxEventFIFOElement
    {
        struct
        {
        uint32_t ID: 29;
        uint32_t RTR: 1;
        uint32_t XTD: 1;
        uint32_t ESI: 1;

        uint32_t TXTS: 16;
        uint32_t DLC: 4; // Data Length Code
        uint32_t BRS: 1; // Bit Rate Switching
        uint32_t EDL: 1; // Extended Data Length
        uint32_t ET: 2; // Event Type
        uint32_t MM: 8; // Message Marker
        };
        uint32_t word[2];
    };

    union TxBufferHeader
    {
        struct
        {
        uint32_t ID: 29;
        uint32_t RTR: 1; // Remote Transmission Request
        uint32_t XTD: 1; // 1 = extended ID (29 bit)
        uint32_t ESI: 1; // Error State Indicator

        uint32_t : 16;
        uint32_t DLC: 4; // Data Length Code
        uint32_t BRS: 1; // Bit Rate Switching
        uint32_t FDF: 1; // FD format
        uint32_t : 1;
        uint32_t EFC: 1; // Event FIFO Control (1 = store events)
        uint32_t MM: 8; // message marker for the Event FIFO
        };
        uint32_t words[2] = {0, 0};
    };

    union TxBufferElement
    {
        struct
        {
        TxBufferHeader hdr;
        uint32_t data[16];
        };
        uint32_t words[18];
    };

    struct MessageRAM
    {
        StdFilterElement stdFilters[28];
        ExtFilterElement extFilters[8];
        RxFifoElement rxFifo0[3];
        RxFifoElement rxFifo1[3];
        TxEventFIFOElement txEventFifo[3];
        TxBufferElement txBuffers[3];
    };

    MessageRAM *msgRam = nullptr;

    RxFifoElement *nextRxMessage(int fifoChannel);
    static int calcDLC(int size);
    static int sizeFromDLC(uint8_t DLC);
};

#endif