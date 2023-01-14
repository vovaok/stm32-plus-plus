#include "drv8320.h"

Drv8320::Drv8320(Spi *spi, Gpio::PinName csPin, Gpio::PinName enablePin) :
    m_spi(spi)
{
    m_csPin = new Gpio(csPin);
    m_csPin->setAsOutput();
    m_csPin->set();
    
    m_enablePin = new Gpio(enablePin);
    m_enablePin->setAsOutput();
    m_enablePin->reset();
    for (int w=0; w<10000; w++);
    m_enablePin->set();
    for (int w=0; w<200000; w++);
    
    Spi::Config conf; 
    conf.CPHA = 1;
    conf.CPOL = 0;
    conf.master = 1;
    conf.baudrate = 5;
    conf.frame16bit = 1;
    m_spi->setConfig(conf);
    m_spi->open(); 
    
    __root static uint16_t jopa;
    
    jopa = readReg(FaultStatus1);
    jopa = readReg(FaultStatus2);
    
    jopa = readReg(OcpControl);
    
    writeReg(DriverControl, 0x000); // 0x100
    writeReg(GateDriveHS,   0x3FF); // 0x366
    writeReg(GateDriveLS,   0x0FF); // 500-ns peak gate-current drive time // 0x433
    writeReg(OcpControl,    0x151); // retry time=4ms, 100-ns deadtime, VDS=0.13V // 0x45F
    
    m_spi->close();
}

void Drv8320::setFaultPin(Gpio::PinName pin)
{
    m_faultPin = new Gpio(pin);
//    m_faultPin->setAsInput();
}

bool Drv8320::isFault() const
{
    return m_faultPin && !m_faultPin->read();
}

void Drv8320::writeReg(uint8_t reg, uint16_t data)
{
    uint16_t tmp = 0;
    tmp |= (reg & 0x0F) << 11;
    tmp |= data & 0x07FF;
    
    m_csPin->reset();
    for (int w=0; w<100; w++);
    m_spi->write(tmp);
    for (int w=0; w<100; w++);
    m_csPin->set();
}

uint16_t Drv8320::readReg(uint8_t reg)
{
    uint16_t tmp = 0;
    tmp |= (1 << 15);
    tmp |= (reg & 0x0F) << 11;
    
    m_csPin->reset();
    for (int w=0; w<100; w++);
    tmp = m_spi->write(tmp);
    for (int w=0; w<100; w++);
    m_csPin->set();
    
    return tmp;
}