#include "drv8711.h"

Drv8711::Drv8711(Spi *spi, Gpio::PinName csPin, Gpio::PinName enablePin) :
    m_spi(spi)
{
    m_csPin = new Gpio(csPin);
    m_csPin->setAsOutput();
    m_csPin->reset();
    
    if(enablePin != Gpio::noPin)
    {
    m_enablePin = new Gpio(enablePin);
    m_enablePin->setAsOutput();
    m_enablePin->reset();
    for (int w=0; w<10000; w++);
    m_enablePin->set();
    for (int w=0; w<200000; w++);
    }
    
    Spi::Config conf; 
    conf.CPHA = 1;
    conf.CPOL = 1;
    conf.master = 1;
    conf.baudrate = 5;
    m_spi->setConfig(conf);
    m_spi->setDataSize(16);
    m_spi->open();   
    
    mCtrlRegister.reg = DTIME_850NS | ISGAIN_10 | EXSTALL_INTERNAL | MODE_1_32_STEP | RSTEP_NO_ACTION | RDIR_DIR_PIN | ENBL_DISABLE;;
    mTorqueRegister.reg = SMPLTH_100US | 0x40;
    mOffRegister.reg = PWMMODE_INTERNAL | 0x80;
    mBlankRegister.reg = ABT_ENABLED | 0x80;
    mDecayRegister.reg = DECMOD_SLOW_ALL | 0x10;
    mStallRegister.reg = VDIV_32 | SDCNT_8STEPS | SDTHR_DEFAULT;
    mDriveRegister.reg = IDRIVEP_100MA | IDRIVEN_200MA | TDRIVEP_500NS | TDRIVEN_500NS | OCPDEG_4US | OCPTH_500MV;
    
    
    
    writeReg(0,mCtrlRegister.reg);
    writeReg(1,mTorqueRegister.reg);
    writeReg(2,mOffRegister.reg);
    writeReg(3,mBlankRegister.reg);
    writeReg(4,mDecayRegister.reg);
    writeReg(5,mStallRegister.reg);
    writeReg(6,mDriveRegister.reg); 
    mStatusRegister.reg = readReg(STATUS_REG);
   
}
    
void Drv8711::updateCtrlReg(uint16_t data)
{
   mCtrlRegister.reg = data;    
   writeReg(CTRL_REG,  data);
}

void Drv8711::updateTorquelReg(uint16_t data)
{
   mTorqueRegister.reg = data;    
   writeReg(TORQUE_REG,  data);
}

void Drv8711::updateOffReg(uint16_t data)
{
   mOffRegister.reg = data;    
   writeReg(OFF_REG,  data);
}
    
void Drv8711::updateBlankReg(uint16_t data)
{
   mBlankRegister.reg = data;    
   writeReg(BLANK_REG,  data);
}  
    
void Drv8711::updateDecayReg(uint16_t data)
{
   mDecayRegister.reg = data;    
   writeReg(DECAY_REG,  data);
}  
  
void Drv8711::updateStallReg(uint16_t data)
{
   mStallRegister.reg = data;    
   writeReg(STALL_REG,  data);
}  
  
void Drv8711::updateDriveReg(uint16_t data)
{
   mDriveRegister.reg = data;    
   writeReg(DRIVE_REG,  data);
}     
   
void Drv8711::resetErrors()
{
     
   writeReg(STATUS_REG,  0x00);
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
    
    m_csPin->set();
    for (int w=0; w<100; w++);
    m_spi->write16(tmp);
    for (int w=0; w<100; w++);
    m_csPin->reset();
}

uint16_t Drv8711::readReg(uint8_t reg)
{
    uint16_t tmp = 0;
    tmp |= (1 << 15);
    tmp |= (reg & 0x07) << 12;
    
    m_csPin->set();
    for (int w=0; w<100; w++);
    tmp = m_spi->write16(tmp);
    for (int w=0; w<100; w++);
    m_csPin->reset();
    
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
  mCtrlRegister.CtrlReg.MODE =0;
  mCtrlRegister.reg |= mode;
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
