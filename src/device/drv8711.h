#ifndef _DRV8711_H
#define _DRV8711_H

#include "spi.h"

class Drv8711
{
public:
    Drv8711(Spi *spi, Gpio::PinName csPin, Gpio::PinName enablePin);
    
    void setFaultPin(Gpio::PinName pin);    
    bool isFault() const;
    
    
      enum DRV8711Registers {
    CTRL_REG    = 0x00,   // Control Register
    TORQUE_REG  = 0x01,   // Torque Register
    OFF_REG     = 0x02,   // Off Register
    BLANK_REG   = 0x03,   // Blank Register
    DECAY_REG   = 0x04,   // Decay Register
    STALL_REG   = 0x05,   // Stall Register
    DRIVE_REG   = 0x06,   // Drive Register
    STATUS_REG  = 0x07    // Status Register
};
    
    
    enum DRV8711StatusRegister {
    // Биты статуса (каждый бит представляет отдельный флаг)
    OTS_MASK     = 0x01,  // Перегрев (Overtemperature Shutdown)
    AOCP_MASK    = 0x02,  // Перегрузка по току канала A
    BOCP_MASK    = 0x04,  // Перегрузка по току канала B
    APDF_MASK    = 0x08,  // Ошибка преддрайвера канала A
    BPDF_MASK    = 0x10,  // Ошибка преддрайвера канала B
    UVLO_MASK    = 0x20,  // Пониженное напряжение питания (Undervoltage Lockout)
    STD_MASK     = 0x40,  // Обнаружен стопор (Stall Detected)
    STDLAT_MASK  = 0x80,  // Зафиксирован стопор (Latched Stall Detect)
    
    // Комбинации флагов
    ALL_FAULTS_MASK = 0xFF,  // Маска всех флагов ошибок 
   
};
    
    
    enum DRV8711DriveRegister {
    // Биты 1-0: Порог защиты от перегрузки по току (OCPTH)
    OCPTH_MASK = 0x03,
    OCPTH_250MV = 0x00,  // 250 мВ
    OCPTH_500MV = 0x01,  // 500 мВ (по умолчанию)
    OCPTH_750MV = 0x02,  // 750 мВ
    OCPTH_1000MV = 0x03, // 1000 мВ

    // Биты 3-2: Время деглитча защиты OCP (OCPDEG)
    OCPDEG_MASK = 0x0C,
    OCPDEG_1US = 0x00,   // 1 мкс
    OCPDEG_2US = 0x04,   // 2 мкс
    OCPDEG_4US = 0x08,   // 4 мкс (по умолчанию)
    OCPDEG_8US = 0x0C,   // 8 мкс

    // Биты 5-4: Время драйва низкой стороны (TDRIVEN)
    TDRIVEN_MASK = 0x30,
    TDRIVEN_250NS = 0x00,  // 250 нс
    TDRIVEN_500NS = 0x10,  // 500 нс (по умолчанию)
    TDRIVEN_1US = 0x20,    // 1 мкс
    TDRIVEN_2US = 0x30,    // 2 мкс

    // Биты 7-6: Время драйва высокой стороны (TDRIVEP)
    TDRIVEP_MASK = 0xC0,
    TDRIVEP_250NS = 0x00,  // 250 нс
    TDRIVEP_500NS = 0x40,  // 500 нс (по умолчанию)
    TDRIVEP_1US = 0x80,    // 1 мкс
    TDRIVEP_2US = 0xC0,    // 2 мкс

    // Биты 9-8: Пиковый ток драйва низкой стороны (IDRIVEN)
    IDRIVEN_MASK = 0x300,
    IDRIVEN_100MA = 0x000, // 100 мА
    IDRIVEN_200MA = 0x100, // 200 мА
    IDRIVEN_300MA = 0x200, // 300 мА (по умолчанию)
    IDRIVEN_400MA = 0x300, // 400 мА

    // Биты 11-10: Пиковый ток драйва высокой стороны (IDRIVEP)
    IDRIVEP_MASK = 0xC00,
    IDRIVEP_50MA = 0x000,  // 50 мА
    IDRIVEP_100MA = 0x400, // 100 мА
    IDRIVEP_150MA = 0x800, // 150 мА (по умолчанию)
    IDRIVEP_200MA = 0xC00  // 200 мА
};
    
    enum DRV8711StallRegister {
    // Биты 7-0: Порог обнаружения стопора (SDTHR)
    SDTHR_MASK = 0xFF,
    SDTHR_DEFAULT = 0x40, // Значение по умолчанию
    
    // Биты 9-8: Количество шагов для подтверждения стопора (SDCNT)
    SDCNT_MASK = 0x300,
    SDCNT_1STEP = 0x000,  // STALLn при первом шаге ниже порога (по умолчанию)
    SDCNT_2STEPS = 0x100, // STALLn после 2 шагов
    SDCNT_4STEPS = 0x200, // STALLn после 4 шагов
    SDCNT_8STEPS = 0x300, // STALLn после 8 шагов
    
    // Биты 11-10: Делитель Back EMF (VDIV)
    VDIV_MASK = 0xC00,
    VDIV_32 = 0x000,     // Делитель 32 (по умолчанию)
    VDIV_16 = 0x400,     // Делитель 16
    VDIV_8 = 0x800,      // Делитель 8
    VDIV_4 = 0xC00,      // Делитель 4
};
    
    enum DRV8711DecayRegister {
    // Биты 7-0: Время перехода смешанного decay (TDECAY)
    TDECAY_MASK = 0xFF,
    TDECAY_MIN = 0x00,    // 0 нс (минимальное значение)
    TDECAY_MAX = 0xFF,    // 127.5 ?s (максимальное значение)
    TDECAY_DEFAULT = 0x10, // Значение по умолчанию (8 ?s)
    TDECAY_STEP = 500,    // Шаг в 500 нс

    // Биты 10-8: Режим decay (DECMOD)
    DECMOD_MASK = 0x700,
    DECMOD_SLOW_ALL = 0x000,       // 000: Всегда медленный decay
    DECMOD_MIXED_INDEXER = 0x100,  // 001: Медленный при увеличении тока, смешанный при уменьшении (только indexer mode, по умолчанию)
    DECMOD_FAST_ALL = 0x200,       // 010: Всегда быстрый decay
    DECMOD_MIXED_ALL = 0x300,      // 011: Всегда смешанный decay
    DECMOD_AUTO_MIXED_INDEXER = 0x400, // 100: Медленный при увеличении, авто-смешанный при уменьшении (только indexer mode)
    DECMOD_AUTO_MIXED_ALL = 0x500, // 101: Всегда авто-смешанный decay
    // 110-111: Зарезервированы

    // Бит 11: Зарезервирован
    
};
    
    enum DRV8711BlankRegister {
    // Биты 7-0: Время blanking (TBLANK)
    TBLANK_MASK = 0xFF,
    TBLANK_MIN = 0x00,   // 1.00 ?s (специальное значение)
    TBLANK_1US = 0x32,   // 1.00 ?s (альтернативное значение)
    TBLANK_MAX = 0xFF,   // 5.12 ?s
    TBLANK_DEFAULT = 0x80, // Значение по умолчанию
    
    // Бит 8: Адаптивное время blanking
    ABT_MASK = 0x100,
    ABT_DISABLED = 0x000,  // Адаптивное время отключено (по умолчанию)
    ABT_ENABLED = 0x100,   // Адаптивное время включено
    
    // Биты 11-9: Зарезервированы    
    
    // Дополнительные удобные определения
    TBLANK_STEP = 20,      // Шаг в 20 нс
    TBLANK_OFFSET = 1000,  // Базовое значение 1000 нс (1 ?s)
};
    
    enum DRV8711OffRegister {
    // Биты 7-0: Время выключения (TOFF)
    TOFF_MASK = 0xFF,
    TOFF_MIN = 0x00,   // 500 нс
    TOFF_MAX = 0xFF,   // 128 мкс
    TOFF_DEFAULT = 0x30, // Значение по умолчанию
    
    // Бит 8: Режим PWM
    PWMMODE_MASK = 0x100,
    PWMMODE_INTERNAL = 0x000,  // Использовать внутренний индексатор (по умолчанию)
    PWMMODE_EXTERNAL = 0x100,  // Обход индексатора, управление через входы xINx
    
    // Биты 11-9: Зарезервированы
    // Не определяем, так как биты зарезервированы
};
    
    enum DRV8711TorqueRegister {
    // Биты 7-0: Установка полного выходного тока
    TORQUE_MASK = 0xFF,
    TORQUE_DEFAULT = 0xFF, // Максимальный ток по умолчанию
    
    // Биты 10-8: Порог выборки Back EMF
    SMPLTH_MASK = 0x700,
    SMPLTH_50US = 0x000,
    SMPLTH_100US = 0x100,  // Значение по умолчанию (001)
    SMPLTH_200US = 0x200,
    SMPLTH_300US = 0x300,
    SMPLTH_400US = 0x400,
    SMPLTH_600US = 0x500,
    SMPLTH_800US = 0x600,
    SMPLTH_1000US = 0x700,
    
    // Бит 11: Зарезервирован  
};
    
    
   enum DRV8711ControlRegister {
    // Бит 0: Включение двигателя
    ENBL_MASK = 0x01,
    ENBL_DISABLE = 0x00,
    ENBL_ENABLE = 0x01,
    
    // Бит 1: Направление вращения
    RDIR_MASK = 0x02,
    RDIR_DIR_PIN = 0x00,
    RDIR_INVERSE_DIR_PIN = 0x02,
    
    // Бит 2: Шаг (только запись)
    RSTEP_MASK = 0x04,
    RSTEP_NO_ACTION = 0x00,
    RSTEP_ADVANCE = 0x04,
    
    // Биты 6-3: Режим микрошага
    MODE_MASK = 0x78,
    MODE_FULL_STEP_71 = 0x00,
    MODE_HALF_STEP = 0x08,
    MODE_QUARTER_STEP = 0x10,
    MODE_1_8_STEP = 0x18,
    MODE_1_16_STEP = 0x20,
    MODE_1_32_STEP = 0x28,
    MODE_1_64_STEP = 0x30,
    MODE_1_128_STEP = 0x38,
    MODE_1_256_STEP = 0x40,
    
    // Бит 7: Режим обнаружения стопора
    EXSTALL_MASK = 0x80,
    EXSTALL_INTERNAL = 0x00,
    EXSTALL_EXTERNAL = 0x80,
    
    // Биты 9-8: Усиление датчика тока
    ISGAIN_MASK = 0x300,
    ISGAIN_5 = 0x000,
    ISGAIN_10 = 0x100,
    ISGAIN_20 = 0x200,
    ISGAIN_40 = 0x300,
    
    // Биты 11-10: Время dead time
    DTIME_MASK = 0xC00,
    DTIME_400NS = 0x000,
    DTIME_450NS = 0x400,
    DTIME_650NS = 0x800,
    DTIME_850NS = 0xC00
};
    
private:
    Spi *m_spi;
    Gpio *m_csPin;
    Gpio *m_enablePin;
    Gpio *m_faultPin;
    
    
union CtrlRegister {
    uint16_t reg;
    struct {
        uint16_t ENBL    : 1;  // Motor enable
        uint16_t RDIR    : 1;  // Direction control
        uint16_t RSTEP   : 1;  // Step command (write-only)
        uint16_t MODE    : 4;  // Step mode
        uint16_t EXSTALL : 1;  // External stall detect
        uint16_t ISGAIN  : 2;  // Current sense gain
        uint16_t DTIME   : 2;  // Dead time
        uint16_t         : 4;  // Reserved
    }CtrlReg;
};

union TorqueRegister
{
  uint16_t reg;
  struct 
  {
    uint16_t TORQUE  : 8;  // Torque setting
    uint16_t SMPLTH  : 3;  // Back EMF sample threshold
    uint16_t         : 5;  // Reserved
  }TorqueReg;
};

union OffRegister {
    uint16_t reg;
    struct {
        uint16_t TOFF    : 8;  // Fixed off time
        uint16_t PWMMODE : 1;  // PWM mode select
        uint16_t         : 7;  // Reserved
    }OffReg;
};

union BlankRegister {
    uint16_t reg;
    struct BlankReg{
        uint16_t TBLANK  : 8;  // Current trip blanking time
        uint16_t ABT     : 1;  // Adaptive blanking time
        uint16_t         : 7;  // Reserved
    };
};

union DecayRegister {
    uint16_t reg;
    struct DecayReg{
        uint16_t TDECAY  : 8;  // Mixed decay transition time
        uint16_t DECMOD  : 3;  // Decay mode
        uint16_t         : 5;  // Reserved
    };
};

union StallRegister {
    uint16_t reg;
    struct {
        uint16_t SDTHR   : 8;  // Stall detect threshold
        uint16_t SDCNT   : 2;  // Stall detect count
        uint16_t VDIV    : 2;  // Back EMF divider
        uint16_t         : 4;  // Reserved
    };
};

union DriveRegister {
    uint16_t reg;
    struct {
        uint16_t OCPTH   : 2;  // OCP threshold
        uint16_t OCPDEG  : 2;  // OCP deglitch time
        uint16_t TDRIVEN : 2;  // Low-side gate drive time
        uint16_t TDRIVEP : 2;  // High-side gate drive time
        uint16_t IDRIVEN : 2;  // Low-side gate peak current
        uint16_t IDRIVEP : 2;  // High-side gate peak current
        uint16_t         : 4;  // Reserved
    };
};

union StatusRegister {
    uint16_t reg;
    struct {
        uint16_t OTS     : 1;  // Overtemperature shutdown
        uint16_t AOCP    : 1;  // Channel A overcurrent
        uint16_t BOCP    : 1;  // Channel B overcurrent
        uint16_t APDF    : 1;  // Channel A predriver fault
        uint16_t BPDF    : 1;  // Channel B predriver fault
        uint16_t UVLO    : 1;  // Undervoltage lockout
        uint16_t STD     : 1;  // Stall detected
        uint16_t STDLAT  : 1;  // Latched stall detect
        uint16_t         : 8;  // Reserved
    };
};
    
  
    void writeReg(uint8_t reg, uint16_t data);
    uint16_t readReg(uint8_t reg);    
    uint8_t calcBlankValue(uint16_t ns); // Расчет значения для произвольного времени (в наносекундах)    
    uint8_t calcDecayValue(uint16_t ns); // Расчет значения для произвольного времени (в наносекундах)
    
};

#endif