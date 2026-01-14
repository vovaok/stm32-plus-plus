#include "drv8711.h"

Drv8711::Drv8711(Spi *spi, Gpio::PinName csPin, Gpio::PinName enablePin) :
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
    m_spi->setConfig(conf);
    m_spi->setDataSize(16);
    m_spi->open();    
    
    mCtrlRegister.reg = readReg(CTRL_REG);
    mTorqueRegister.reg = readReg(TORQUE_REG);
    mStatusRegister.reg = readReg(STATUS_REG);
   
}

void Drv8711::setFaultPin(Gpio::PinName pin)
{
    m_faultPin = new Gpio(pin);
//    m_faultPin->setAsInput();
}

bool Drv8711::isFault() const
{
    return m_faultPin && !m_faultPin->read();
}

void Drv8711::writeReg(uint8_t reg, uint16_t data)
{
    uint16_t tmp = 0;
    tmp |= (reg & 0x07) << 12;
    tmp |= data & 0x0fFF;
    
    m_csPin->reset();
    for (int w=0; w<100; w++);
    m_spi->write16(tmp);
    for (int w=0; w<100; w++);
    m_csPin->set();
}

uint16_t Drv8711::readReg(uint8_t reg)
{
    uint16_t tmp = 0;
    tmp |= (1 << 15);
    tmp |= (reg & 0x07) << 12;
    
    m_csPin->reset();
    for (int w=0; w<100; w++);
    tmp = m_spi->write16(tmp);
    for (int w=0; w<100; w++);
    m_csPin->set();
    
    return tmp;
}

void Drv8711::setRevers(bool rev)
{
  mCtrlRegister.CtrlReg.RDIR = rev;
  writeReg(CTRL_REG,mCtrlRegister.reg);
}

void Drv8711::setEnable(bool en)
{
  mCtrlRegister.CtrlReg.ENBL = en;
  writeReg(CTRL_REG,mCtrlRegister.reg);
}

void Drv8711::setStepMode(DRV8711ControlRegister mode)
{
  mCtrlRegister.CtrlReg.MODE = mode;
  writeReg(CTRL_REG,mCtrlRegister.reg);
}

void Drv8711::setTorq(char torq)
{
  mTorqueRegister.TorqueReg.TORQUE = torq;
  writeReg(TORQUE_REG,mTorqueRegister.reg);
}

void Drv8711::setTorqPercent(float perc)
{
  mTorqueRegister.TorqueReg.TORQUE = perc <1? perc >0? perc*255: 0 :255 ;
  writeReg(TORQUE_REG,mTorqueRegister.reg);
}

uint16_t Drv8711::status()
{
 mStatusRegister.reg = readReg(STATUS_REG);
 return mStatusRegister.reg;
}

uint8_t Drv8711::calcBlankValue(uint16_t ns)
{
    if (ns <= 1000) return 0x00; // Для 1 ?s или меньше
    if (ns >= 5120) return 0xFF; // Максимальное значение
    return (ns - 1000) / 20 + 0x32;
}


uint8_t Drv8711::calcDecayValue(uint16_t ns)
{
    return (ns / TDECAY_STEP) & 0xFF;
}
