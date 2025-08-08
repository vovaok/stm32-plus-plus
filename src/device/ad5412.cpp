#include "ad5412.h"

AD5412::AD5412(Spi *spi, Gpio::PinName latchPin, Gpio::PinName clearPin) :
        mSpi(spi),
        mLatchPin(new Gpio(latchPin)),       
        mEnabled(false)
    {
        // Initialize pins
        mLatchPin->setAsOutput();
         select();
        
         if (clearPin!=Gpio::noPin)
        {
            mClearPin = new Gpio(clearPin);
            mClearPin->setAsOutput();
            mClearPin->reset(); // Deassert CLEAR
        }
        
        // Configure SPI
        Spi::Config conf;
        conf.CPHA = 0;      // Data captured on rising edge
        conf.CPOL = 0;       // Clock idle low
        conf.master = 1;     // Master mode
        conf.baudrate = 3;   // Adjust based on your system clock
        mSpi->setConfig(conf);
        mSpi->setDataSize(8); // AD5412/AD5422 uses 24-bit transfers
        mSpi->open();
    }

    AD5412::~AD5412()
    {
        delete mLatchPin;
        if (mClearPin) delete mClearPin;
    }

    // Set output value (0-65535 for 16-bit, 0-4095 for 12-bit)
    void AD5412::setValue(uint16_t value)
    {
        uint32_t data = (0x01 << 16) | (value << (mIs16Bit ? 0 : 4));
        writeSPI(data);
    }

    // Configure output range and options
    void AD5412::configure(OutputRange range, bool enableOutput, bool useExternalResistor)
    {
        ControlRegister ctrl;
        ctrl.value = 0;
        ctrl.range = range;
        ctrl.outen = enableOutput ? 1 : 0;
        ctrl.rext = useExternalResistor ? 1 : 0;
        
        uint32_t data = (0x55 << 16) | ctrl.value;
        writeSPI(data);
    }

    // Enable/disable slew rate control
    void AD5412::setSlewRateControl(bool enable, SlewRateStep step, SlewRateClock clock)
    {
        ControlRegister ctrl;
        ctrl.value = 0;
        ctrl.sren = enable ? 1 : 0;
        ctrl.sr_step = step;
        ctrl.sr_clock = clock;
        
        uint32_t data = (0x55 << 16) | ctrl.value;
        writeSPI(data);
    }

    // Perform a reset (asynchronous clear)
    void AD5412::reset(bool toMidscale)
    {
        if (mClearPin)
        {
            // Hardware reset
            mClearPin->set();    // Assert CLEAR
            delayMicroseconds(20); // Minimum 20ns pulse width
            mClearPin->reset();  // Deassert CLEAR
        }
        else 
        {
            // Software reset
            uint32_t data = (0x56 << 16) | 0x01;
            writeSPI(data);
        }
    }

    // Read status register
    uint8_t AD5412::readStatus()
    {
        // First write sets up readback
        writeSPI(0x020000);
        // Second write (NOP) clocks out the data
        return writeSPI(0x000000) & 0xFF;
    }



    void AD5412::select()
    {
        mLatchPin->reset();
        delayMicroseconds(1);
    }

    void AD5412::deselect()
    {
        delayMicroseconds(1);
        mLatchPin->set();
        delayMicroseconds(1);
    }

    uint32_t AD5412::writeSPI(uint32_t data)
    {
            
        mLatchPin->reset(); 
        
        uint32_t response = mSpi->write((data>>16)&0xff);
        response = mSpi->write((data>>8)&0xff);
        response = mSpi->write((data) & 0xff);      
        
        mLatchPin->set();       
        return response;
    }

    void AD5412::delayMicroseconds(uint32_t us)
    {
        // Implement based on your system
        for (volatile uint32_t i = 0; i < us * 100; i++);
    }
