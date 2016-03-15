#include "i2s.h"

I2S::I2S(Gpio::Config clkPin, Gpio::Config doPin) :
    mActive(false),
    mRxDma(0L), mTxDma(0L)
{
    SPI_TypeDef *clkSpi = getSpiByPin(clkPin);
    SPI_TypeDef *doSpi = getSpiByPin(doPin);
    if (!clkSpi || !doSpi)
        throw Exception::invalidPin;
    if (clkSpi != doSpi)
        throw Exception::invalidPeriph;
    
    Gpio::config(clkPin);
    Gpio::config(doPin);
    
    mSpi = clkSpi;
    if (mSpi == SPI1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    }
    else if (mSpi == SPI2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
    else if (mSpi == SPI3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    }
    
    I2S_InitTypeDef config;
    SPI_I2S_DeInit(mSpi);
    config.I2S_AudioFreq = 48000;
    config.I2S_Standard = I2S_Standard_LSB;//PCMShort;
    config.I2S_DataFormat = I2S_DataFormat_16b;
    config.I2S_CPOL = I2S_CPOL_High;
    config.I2S_Mode = I2S_Mode_MasterRx;
    config.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_Init(mSpi, &config);
    
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
    throw Exception::badSoBad;
    #warning ne rabotaet for stm32f427 or 437 or 429 or 439
#else
    unsigned long pr = config.I2S_MCLKOutput;
    if (config.I2S_DataFormat == I2S_DataFormat_16b)
    {

        // blabla
//#warning nado zapilit baudrate calculation, a poka:
        if (config.I2S_AudioFreq == 8000)
        {
            RCC_PLLI2SConfig(192, 2);
            pr |= 0x100 + 187;
        }
        else if (config.I2S_AudioFreq == 32000)
        {
            RCC_PLLI2SConfig(256, 2);
            pr |= 0x100 + 62;
        }
        else if (config.I2S_AudioFreq == 48000)
        {
            RCC_PLLI2SConfig(192, 5);
            pr |= 0x100 + 12;
        }
        else
        {
            throw Exception::badSoBad;
        }
    }
    else
    {
        if (config.I2S_AudioFreq == 8000)
        {
            RCC_PLLI2SConfig(192, 3);
            pr |= 0x100 + 62;
        }
        else if (config.I2S_AudioFreq == 32000)
        {
            RCC_PLLI2SConfig(256, 5);
            pr |= 0x100 + 12;
        }
        else if (config.I2S_AudioFreq == 48000)
        {
            RCC_PLLI2SConfig(384, 5);
            pr |= 0x100 + 12;
        }
        else
        {
            throw Exception::badSoBad;
        }
    }

    
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);
    
    mSpi->I2SPR = pr; 
#endif
}

SPI_TypeDef *I2S::getSpiByPin(Gpio::Config pinConfig)
{
    switch (pinConfig)
    {
        case Gpio::SPI1_NSS_PA4: case Gpio::SPI1_SCK_PA5: case Gpio::SPI1_MISO_PA6: case Gpio::SPI1_MOSI_PA7:
        case Gpio::SPI1_NSS_PA15: case Gpio::SPI1_SCK_PB3: case Gpio::SPI1_MISO_PB4: case Gpio::SPI1_MOSI_PB5:
        return SPI1;
        
        case Gpio::SPI2_NSS_PB9: case Gpio::SPI2_SCK_PB10: case Gpio::SPI2_NSS_PB12: case Gpio::SPI2_SCK_PB13:
        case Gpio::SPI2_MISO_PB14: case Gpio::SPI2_MOSI_PB15: case Gpio::SPI2_MISO_PC2: case Gpio::SPI2_MOSI_PC3:
        case Gpio::I2S2_MCK_PC6: case Gpio::I2S_CKIN_PC9: case Gpio::SPI2_NSS_PI0: case Gpio::SPI2_SCK_PI1:
        case Gpio::SPI2_MISO_PI2: case Gpio::SPI2_MOSI_PI3:
        return SPI2;
        
        case Gpio::SPI3_NSS_PA4: case Gpio::SPI3_NSS_PA15: case Gpio::SPI3_SCK_PB3: case Gpio::SPI3_MISO_PB4:
        case Gpio::SPI3_MOSI_PB5: case Gpio::I2S3_MCK_PC7: case Gpio::SPI3_SCK_PC10: case Gpio::SPI3_MISO_PC11:
        case Gpio::SPI3_MOSI_PC12:
        return SPI3;
        
        default: return 0L;
    }
}

void I2S::setActive(bool enable)
{
    I2S_Cmd(mSpi, enable? ENABLE: DISABLE); 
    mActive = enable;
}

Dma *I2S::dmaForRx()
{
    if (mRxDma)
        return mRxDma;
    
    if (mSpi == SPI1)
    {
        mRxDma = Dma::getStreamForPeriph(Dma::ChannelSpi1_Rx);
    }
    else if (mSpi == SPI2)
    {
        mRxDma = Dma::getStreamForPeriph(Dma::ChannelSpi2_Rx);
    }
    else if (mSpi == SPI3)
    {
        mRxDma = Dma::getStreamForPeriph(Dma::ChannelSpi3_Rx);
    }
    if (mRxDma)
    {
        SPI_I2S_DMACmd(mSpi, SPI_I2S_DMAReq_Rx, ENABLE);
        mRxDma->setSource((void*)&mSpi->DR, 2);
    }
    return mRxDma;
}

Dma *I2S::dmaForTx()
{
    if (mTxDma)
        return mTxDma;
  
    if (mSpi == SPI1)
    {
        mTxDma = Dma::getStreamForPeriph(Dma::ChannelSpi1_Tx);
    }
    else if (mSpi == SPI2)
    {
        mTxDma = Dma::getStreamForPeriph(Dma::ChannelSpi2_Tx);
    }
    else if (mSpi == SPI3)
    {
        mTxDma = Dma::getStreamForPeriph(Dma::ChannelSpi3_Tx);
    }
    if (mTxDma)
    {
        SPI_I2S_DMACmd(mSpi, SPI_I2S_DMAReq_Tx, ENABLE);
        mTxDma->setSink((void*)&mSpi->DR, 2);
    }
    return mTxDma;
}

void I2S::deleteDmaForRx()
{
    SPI_I2S_DMACmd(mSpi, SPI_I2S_DMAReq_Rx, DISABLE);
    if (mRxDma)
    {
        mRxDma->stop();
        delete mRxDma;
        mRxDma = 0L;
    }
}

void I2S::deleteDmaForTx()
{
    SPI_I2S_DMACmd(mSpi, SPI_I2S_DMAReq_Tx, DISABLE);
    if (mTxDma)
    {
        mTxDma->stop();
        delete mTxDma;
        mTxDma = 0L;
    }
}

void I2S::write(unsigned short data)
{
    mSpi->DR = data;
}

unsigned short I2S::read()
{
    return mSpi->DR;
}