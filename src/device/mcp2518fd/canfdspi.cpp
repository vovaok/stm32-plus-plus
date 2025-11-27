#include "canfdspi.h"
#include "drv_canfdspi_api.h"

// Transmit Channels
#define APP_TX_FIFO CAN_FIFO_CH31
// Receive Channels
//#define APP_RX_FIFO CAN_FIFO_CH30

CAN_BITTIME_SETUP selectedBitTime = CAN_1000K_5M; // CAN_500K_2M


CanFdSpi *CanFdSpi::m_handle[CANFDSPI_INSTANCES_MAX] {0};

CanFdSpi *CanFdSpi::instance(uint8_t id)
{
    if (id < CANFDSPI_INSTANCES_MAX)
        return m_handle[id];
    return nullptr;
}

CanFdSpi::CanFdSpi(uint8_t chipId, Spi *spi, Gpio::PinName chipSelect) :
    CanInterface(CAN_FIFO_TOTAL_CHANNELS - 1), // 31 fifos
    m_chipId(chipId),
    m_spi(spi),
    m_cs(chipSelect, Gpio::Output)
{
    if (chipId < CANFDSPI_INSTANCES_MAX)
        m_handle[chipId] = this;
    
    if (!spi->isOpen())
    {
        spi->setMasterMode();
        spi->setBaudrate(20000000);
        spi->setCPOL_CPHA(0, 0);
        spi->setDataSize(8);
        spi->setUseDmaTx(true);
        spi->setUseDmaRx(true);
        spi->onBytesWritten = EVENT(&CanFdSpi::onTransferComplete);
        spi->open();
    }
    
    m_buffer.resize(SPI_DEFAULT_BUFFER_LENGTH + 2);
    
    init();
    acquireFifoChannel(); // consume FIFO_CHANNEL_0
}

int CanFdSpi::pendingMessageLength(int fifoChannel)
{
    DRV_CANFDSPI_ReceiveChannelEventGet(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, &rxFlags);
    if (rxFlags & CAN_RX_FIFO_NOT_EMPTY_EVENT)
    {
        /// @todo retrieve more accurate result
        return 64;
    }
    return -1;
}

int CanFdSpi::receiveMessage(uint32_t *id, uint8_t *data, uint8_t maxsize, int fifoChannel)
{
    CAN_RX_MSGOBJ rxObj;
    if (receive(rxObj, data, maxsize, fifoChannel))
    {
        if (rxObj.bF.ctrl.IDE)
            *id = rxObj.bF.id.EID | (rxObj.bF.id.SID << 18);
        else
            *id = rxObj.bF.id.SID;
        return DRV_CANFDSPI_DlcToDataBytes((CAN_DLC)rxObj.bF.ctrl.DLC);
    }
    return -1;
}

bool CanFdSpi::transmitMessage(Flags flags, uint32_t id, const uint8_t *data, uint8_t size)
{
    if (flags & FD)
    {
        if (size > 64)
            return false;
    }
    else if (size > 8)
        return false;
    
    CAN_TX_MSGOBJ txObj;
    txObj.word[0] = 0;
    txObj.word[1] = 0;
//    txObj.word[2] = 0;
    
    if (flags & ExtId)
    {
        txObj.bF.id.EID = id;
        txObj.bF.id.SID = id >> 18;
        txObj.bF.ctrl.IDE = 1;
    }
    else
    {
        txObj.bF.id.SID = id;
    }
    
    if (flags & FD)
        txObj.bF.ctrl.FDF = 1;
    if (flags & BRS)
        txObj.bF.ctrl.BRS = 1;
    
    txObj.bF.ctrl.DLC = dlcFromSize(size);
        
    return send(txObj, data, size);
}

//void CanFdSpi::open()
//{
//    // Select Normal Mode
//    DRV_CANFDSPI_OperationModeSelect(m_chipId, CAN_NORMAL_MODE);
//}

uint8_t CanFdSpi::readByte(uint16_t addr)
{
    uint8_t b;
    m_cs.reset();
    m_spi->write(0x30 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    b = m_spi->read();
    m_cs.set();
    return b;
}

void CanFdSpi::writeByte(uint16_t addr, uint8_t byte)
{
    m_cs.reset();
    m_spi->write(0x20 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    m_spi->write(byte);
    m_cs.set();
}

uint16_t CanFdSpi::readHalfWord(uint16_t addr)
{
    union
    {
        uint16_t value;
        uint8_t b[2];
    };
    m_cs.reset();
    m_spi->write(0x30 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    b[0] = m_spi->read();
    b[1] = m_spi->read();
    m_cs.set();
    return value;
}

void CanFdSpi::writeHalfWord(uint16_t addr, uint16_t value)
{
    const uint8_t *src = reinterpret_cast<const uint8_t *>(&value);
    m_cs.reset();
    m_spi->write(0x20 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    m_spi->write(src[0]);
    m_spi->write(src[1]);
    m_cs.set();
}

uint32_t CanFdSpi::readReg(uint16_t addr)
{
    union
    {
        uint32_t value;
        uint8_t b[4];
    };
    m_cs.reset();
    m_spi->write(0x30 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    b[0] = m_spi->read();
    b[1] = m_spi->read();
    b[2] = m_spi->read();
    b[3] = m_spi->read();
    m_cs.set();
    return value;
}

void CanFdSpi::writeReg(uint16_t addr, uint32_t value)
{
    const uint8_t *src = reinterpret_cast<const uint8_t *>(&value);
    m_cs.reset();
    m_spi->write(0x20 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    m_spi->write(src[0]);
    m_spi->write(src[1]);
    m_spi->write(src[2]);
    m_spi->write(src[3]);
    m_cs.set();
}

void CanFdSpi::readRAM(uint16_t addr, uint8_t *buffer, int size)
{
//    addr &= ~3; // align addr
//    size &= ~3; // align size
    
    m_cs.reset();
    m_spi->write(0x30 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    m_spi->transferDma(nullptr, buffer, size);
//    m_cs.set();
}

void CanFdSpi::writeRAM(uint16_t addr, const uint8_t *buffer, int size)
{
//    addr &= ~3; // align addr
//    size &= ~3; // align size
    
    m_cs.reset();
    m_spi->write(0x20 | ((addr >> 8) & 0x0F));
    m_spi->write(addr & 0xFF);
    m_spi->transferDma(buffer, nullptr, size);
//    m_cs.set();
}

void CanFdSpi::writeRawDataAsync(const uint8_t *data, int size)
{
    m_cs.reset();
    m_spi->writeAsync(data, size);
}

void CanFdSpi::readRawDataAsync(uint8_t *data, int size)
{
    m_cs.reset();
    m_spi->transferDma(data, data, size);
}

void CanFdSpi::init()
{
    // Reset device
    DRV_CANFDSPI_Reset(m_chipId);

    // Enable ECC and initialize RAM
    DRV_CANFDSPI_EccEnable(m_chipId);

    DRV_CANFDSPI_RamInit(m_chipId, 0xff);

    // Configure device
    DRV_CANFDSPI_ConfigureObjectReset(&config);
    config.IsoCrcEnable = 1;
    config.StoreInTEF = 0;
    DRV_CANFDSPI_Configure(m_chipId, &config);
    
    configureTxFifo(APP_TX_FIFO, 7, 64);

    // Setup Bit Time
    DRV_CANFDSPI_BitTimeConfigure(m_chipId, selectedBitTime, CAN_SSP_MODE_AUTO, CAN_SYSCLK_40M);

    // Setup Transmit and Receive Interrupts
    DRV_CANFDSPI_GpioModeConfigure(m_chipId, GPIO_MODE_INT, GPIO_MODE_INT);
//    DRV_CANFDSPI_TransmitChannelEventEnable(m_chipId, APP_TX_FIFO, CAN_TX_FIFO_NOT_FULL_EVENT);
//    DRV_CANFDSPI_ReceiveChannelEventEnable(m_chipId, APP_RX_FIFO, CAN_RX_FIFO_NOT_EMPTY_EVENT);
    DRV_CANFDSPI_ModuleEventEnable(m_chipId, (CAN_MODULE_EVENT)(CAN_TX_EVENT | CAN_RX_EVENT));
}

bool CanFdSpi::open(Device::OpenMode mode)
{
    // if already opened...
    if (DRV_CANFDSPI_OperationModeGet(m_chipId) != CAN_CONFIGURATION_MODE)
        return false;        
        
    // choose Silent mode if no write allowed
    if (mode & Device::WriteOnly)
        DRV_CANFDSPI_OperationModeSelect(m_chipId, CAN_NORMAL_MODE);
    else
        DRV_CANFDSPI_OperationModeSelect(m_chipId, CAN_LISTEN_ONLY_MODE);

    return true;
}

bool CanFdSpi::close()
{
    DRV_CANFDSPI_OperationModeSelect(m_chipId, CAN_CONFIGURATION_MODE);
    return true;
}

bool CanFdSpi::isOpen() const
{
    return DRV_CANFDSPI_OperationModeGet(m_chipId) != CAN_CONFIGURATION_MODE;
}

bool CanFdSpi::configureTxFifo(int fifoChannel, int depth, int maxsize)
{
    if (fifoChannel == 0 || fifoChannel >= CAN_FIFO_TOTAL_CHANNELS)
        return false;
    
    int8_t r = 0;
    
    // Setup TX FIFO
    DRV_CANFDSPI_TransmitChannelConfigureObjectReset(&txConfig);
    txConfig.FifoSize = depth;
    txConfig.PayLoadSize = payloadSize(maxsize);
    txConfig.TxPriority = 1;
    r = DRV_CANFDSPI_TransmitChannelConfigure(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, &txConfig);
    
    if (r)
        return false;
    
    // enable notfull event of the channel
    DRV_CANFDSPI_TransmitChannelEventEnable(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, CAN_TX_FIFO_NOT_FULL_EVENT);
    
    return true;
}

bool CanFdSpi::configureRxFifo(int fifoChannel, int depth, int maxsize)
{
    if (fifoChannel == 0 || fifoChannel >= CAN_FIFO_TOTAL_CHANNELS)
        return false;
    
    int8_t r = 0;
    
    // Setup RX FIFO
    DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(&rxConfig);
    rxConfig.FifoSize = depth - 1;
    rxConfig.PayLoadSize = payloadSize(maxsize);
    r = DRV_CANFDSPI_ReceiveChannelConfigure(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, &rxConfig);
    if (r)
        return false;
    
    // enable receive event of the channel
    DRV_CANFDSPI_ReceiveChannelEventEnable(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, CAN_RX_FIFO_NOT_EMPTY_EVENT);
    return true;
}

int CanFdSpi::configureFilter(Flags flags, uint32_t id, uint32_t mask, int fifoChannel)
{
    if (m_filterIdx >= CAN_FILTER_TOTAL)
        return -1;
    if (fifoChannel == 0 || fifoChannel >= CAN_FIFO_TOTAL_CHANNELS)
        return -1;
    
    CAN_FILTER filterIdx = static_cast<CAN_FILTER>(m_filterIdx);
    REG_CiFLTOBJ fObj;
    REG_CiMASK mObj;
    
    // Setup RX Filter
    fObj.word = 0;
    if (flags & ExtId)
    {
        fObj.bF.EID = id;
        fObj.bF.SID = id >> 18;
        fObj.bF.EXIDE = 1;
    }
    else
    {
        fObj.bF.SID = id;
    }
    DRV_CANFDSPI_FilterObjectConfigure(m_chipId, filterIdx, &fObj.bF);

    // Setup RX Mask
    mObj.word = 0;
    if (flags & ExtId)
    {
        mObj.bF.MEID = mask;
        mObj.bF.MSID = mask >> 18;
        mObj.bF.MIDE = 1; // Only allow extended or standard IDs
    }
    else
    {
        mObj.bF.MSID = mask;
    }
    DRV_CANFDSPI_FilterMaskConfigure(m_chipId, filterIdx, &mObj.bF);

    // Link FIFO and Filter
    DRV_CANFDSPI_FilterToFifoLink(m_chipId, filterIdx, (CAN_FIFO_CHANNEL)fifoChannel, true);
    
    /// @todo todo!! replace magic number 8 of FIFO depth with something more useful
    int maxsize = flags & FD? 64: 8;
    int depth = maxsize == 8? 32: 8;
    configureRxFifo(fifoChannel, depth, maxsize);
    
    m_filterIdx++;
    return m_filterIdx - 1;
}

bool CanFdSpi::removeFilter(int index)
{
    /// @todo Implement filter removing
    return false;
}

bool CanFdSpi::send(CAN_TX_MSGOBJ &txObj, const uint8_t *data, uint8_t size)
{
    DRV_CANFDSPI_TransmitChannelEventGet(m_chipId, APP_TX_FIFO, &txFlags);
    if (txFlags & CAN_TX_FIFO_NOT_FULL_EVENT) // it's free
    {
        // Load message and transmit
//        uint8_t size = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC)txObj.bF.ctrl.DLC);
        DRV_CANFDSPI_TransmitChannelLoad(m_chipId, APP_TX_FIFO, &txObj,
                                         const_cast<uint8_t*>(data), size, true);
        return true;
    }
    return false;
}

bool CanFdSpi::receive(CAN_RX_MSGOBJ &rxObj, uint8_t *data, uint8_t maxsize, int fifoChannel)
{
//    uint32_t *buf = reinterpret_cast<uint32_t *>(m_rxBuf);
//    uint32_t *dst = reinterpret_cast<uint32_t *>(data);
//    readRAM(0x400 | (fifoChannel << 4), m_rxBuf, 16);
//    rxObj.word[0] = buf[0];
//    rxObj.word[1] = buf[1];
//    dst[0] = buf[2];
//    dst[1] = buf[3];
//    
//    writeByte(0x51 + 12 * fifoChannel, 0x01);
//    
//    return true;
    
    DRV_CANFDSPI_ReceiveChannelEventGet(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, &rxFlags);
    if (rxFlags & CAN_RX_FIFO_NOT_EMPTY_EVENT)
    {
        DRV_CANFDSPI_ReceiveMessageGet(m_chipId, (CAN_FIFO_CHANNEL)fifoChannel, &rxObj, data, maxsize);
        return true;
    }
    return false;
}

CAN_FIFO_PLSIZE CanFdSpi::payloadSize(uint8_t size)
{
    if (size <= 8)
        return CAN_PLSIZE_8;
    else if (size <= 12)
        return CAN_PLSIZE_12;
    else if (size <= 16)
        return CAN_PLSIZE_16;
    else if (size <= 20)
        return CAN_PLSIZE_20;
    else if (size <= 24)
        return CAN_PLSIZE_24;
    else if (size <= 32)
        return CAN_PLSIZE_32;
    else if (size <= 48)
        return CAN_PLSIZE_48;
    return CAN_PLSIZE_64;
}

//void CanFdSpi::transfer(const uint8_t *txBuf, uint8_t *rxBuf, uint16_t size)
//{
//    m_cs.reset();
//    m_spi->transfer(txBuf, rxBuf, size);
//    m_cs.set();
//}

void CanFdSpi::onTransferComplete()
{
    m_cs.set();
    transferCompleteEvent();
}

//! SPI Read/Write Transfer
int8_t DRV_SPI_TransferData(uint8_t spiSlaveDeviceIndex, uint8_t *SpiTxData, uint8_t *SpiRxData, uint16_t XferSize)
{
    CanFdSpi *dev = CanFdSpi::instance(spiSlaveDeviceIndex);
    if (dev)
    {
        dev->m_cs.reset();
        dev->m_spi->transfer(SpiTxData, SpiRxData, XferSize);
        dev->m_cs.set();
        return 0;
    }
    return -1;
}

int8_t DRV_CANFDSPI_WriteByteArray(uint8_t chipId, uint16_t addr, const uint8_t *data, uint16_t size)
{
    CanFdSpi *dev = CanFdSpi::instance(chipId);
    if (dev)
    {
        uint8_t *buf = reinterpret_cast<uint8_t*>(dev->m_buffer.data());
        uint8_t *dst = buf;
        *dst++ = (cINSTRUCTION_WRITE << 4) + ((addr >> 8) & 0xF);
        *dst++ = (uint8_t) (addr & 0xFF);
        int sz = size + 2;
        while (size--)
            *dst++ = *data++;
        
        dev->m_cs.reset();
        dev->m_spi->transfer(buf, sz);
        dev->m_cs.set();
        return 0;
    }
    return -1;
}

int8_t DRV_CANFDSPI_ReadByteArray(uint8_t chipId, uint16_t addr, uint8_t *data, uint16_t size)
{
    CanFdSpi *dev = CanFdSpi::instance(chipId);
    if (dev)
    {
        uint8_t *buf = reinterpret_cast<uint8_t*>(dev->m_buffer.data());
        uint8_t *dst = buf;
        *dst++ = (cINSTRUCTION_READ << 4) + ((addr >> 8) & 0xF);
        *dst++ = (uint8_t) (addr & 0xFF);
        int sz = size + 2;
//        while (size--)
//            *dst++ = 0x00;
        
        dev->m_cs.reset();
        dev->m_spi->transfer(buf, sz);
        dev->m_cs.set();
        
        buf++;
        buf++;
        while (size--)
            *data++ = *buf++;
        return 0;
    }
    return -1;
}
