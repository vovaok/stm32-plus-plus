#ifndef _CANFDSPI_H
#define	_CANFDSPI_H

#define SPI_DEFAULT_BUFFER_LENGTH   128
#define CANFDSPI_INSTANCES_MAX      7

#include "caninterface.h"
#include "spi.h"
#include "drv_canfdspi_register.h"

//! SPI Read/Write Transfer
int8_t DRV_SPI_TransferData(uint8_t spiSlaveDeviceIndex, uint8_t *SpiTxData, uint8_t *SpiRxData, uint16_t XferSize);

int8_t DRV_CANFDSPI_WriteByteArray(uint8_t chipId, uint16_t addr, const uint8_t *data, uint16_t size);
int8_t DRV_CANFDSPI_ReadByteArray(uint8_t chipId, uint16_t addr, uint8_t *data, uint16_t size);

class CanFdSpi : public CanInterface
{
public:
    CanFdSpi(uint8_t chipId, Spi *spi, Gpio::PinName chipSelect);
    static CanFdSpi *instance(uint8_t id);
    
    virtual bool hasFD() override {return true;}
    
    virtual int configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel) override;
    virtual bool removeFilter(int index) override;
    
    virtual int pendingMessageLength(int fifoChannel) override;
    virtual int receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel) override;
    virtual bool transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size) override;    
    
    virtual bool open(Device::OpenMode mode=Device::ReadWrite) override; // set to normal mode
    virtual bool close() override; // set to init mode
    virtual bool isOpen() const override;
    
    bool configureRxFifo(int fifoChannel, int depth, int maxsize);
    bool configureTxFifo(int fifoChannel, int depth, int maxsize);
    
protected:
public:
    virtual void transferCompleteEvent() {};
    
//    bool configureRxFifo(int fifoChannel, int depth, int maxsize);
//    bool configureTxFifo(int fifoChannel, int depth, int maxsize);
    bool send(CAN_TX_MSGOBJ &txObj, const uint8_t *data, uint8_t size);
    bool receive(CAN_RX_MSGOBJ &rxObj, uint8_t *data, uint8_t maxsize, int fifoChannel);
    
    uint8_t readByte(uint16_t addr);
    void writeByte(uint16_t addr, uint8_t byte);
    uint16_t readHalfWord(uint16_t addr);
    void writeHalfWord(uint16_t addr, uint16_t value);
    uint32_t readReg(uint16_t addr);
    void writeReg(uint16_t addr, uint32_t value);
    void readRAM(uint16_t addr, uint8_t *buffer, int size);
    void writeRAM(uint16_t addr, const uint8_t *buffer, int size);
    
    void writeRawDataAsync(const uint8_t *data, int size);
    void readRawDataAsync(uint8_t *data, int size);
    
    static CAN_FIFO_PLSIZE payloadSize(uint8_t size);
    
    uint8_t m_chipId;
    Spi *m_spi;
    Gpio m_cs;
    
private:
    static CanFdSpi *m_handle[CANFDSPI_INSTANCES_MAX];
//    uint8_t m_chipId;
//    Spi *m_spi;
//    Gpio m_cs;
    
    ByteArray m_buffer;
    
    CAN_CONFIG config;
    CAN_OPERATION_MODE opMode;

    // Transmit objects
    CAN_TX_FIFO_CONFIG txConfig;
    CAN_TX_FIFO_EVENT txFlags;

    // Receive objects
    CAN_RX_FIFO_CONFIG rxConfig;
    CAN_RX_FIFO_EVENT rxFlags;
    
    int m_filterIdx = 0;

    void init();
    
    friend int8_t DRV_SPI_TransferData(uint8_t spiSlaveDeviceIndex, uint8_t *SpiTxData, uint8_t *SpiRxData, uint16_t XferSize);
    friend int8_t DRV_CANFDSPI_WriteByteArray(uint8_t chipId, uint16_t addr, const uint8_t *data, uint16_t size);
    friend int8_t DRV_CANFDSPI_ReadByteArray(uint8_t chipId, uint16_t addr, uint8_t *data, uint16_t size);
//    void transfer(const uint8_t *txBuf, uint8_t *rxBuf, uint16_t size);
    void onTransferComplete();
};

#endif	/* _CANFDSPI_H */
