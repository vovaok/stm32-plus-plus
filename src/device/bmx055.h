#ifndef _BMX055_H
#define _BMX055_H

#include "i2c.h"
#include "imu.h"

class BMX055 : public Imu
{
public:
    BMX055(I2c *i2c);
    
    virtual void update(float dt) override;
                
    bool readAcc();
    bool readGyro();
//    bool readMag();
    
private:
    I2c *m_dev;
    
    enum BMAReg;
    enum BMGReg;
    enum BMMReg;
    
    uint8_t readReg(BMAReg reg);
    bool writeReg(BMAReg reg, uint8_t value);
    uint8_t readReg(BMGReg reg);
    bool writeReg(BMGReg reg, uint8_t value);
    uint8_t readReg(BMMReg reg);
    bool writeReg(BMMReg reg, uint8_t value);
    
    uint8_t readReg(uint8_t addr, uint8_t reg);
    bool writeReg(uint8_t addr, uint8_t reg, uint8_t value);
    
    void setFastOffset();    
};


enum BMX055::BMAReg : uint8_t
{
    REG_BMA_CHIPID = 0x00, // ID чипа
	REG_BMA_X_LSB = 0x02, // Данные углового ускорения по оси X		
	REG_BMA_X_MSB = 0x03, // Данные углового ускорения по оси X		
	REG_BMA_Y_LSB = 0x04, // Данные углового ускорения по оси Y		
	REG_BMA_Y_MSB = 0x05, // Данные углового ускорения по оси Y		
	REG_BMA_Z_LSB = 0x06, // Данные углового ускорения по оси Z		
	REG_BMA_Z_MSB = 0x07, // Данные углового ускорения по оси Z		
	REG_BMA_TEMP = 0x08, // Данные температуры чипа	в °C			
	REG_BMA_INT_ST_0 = 0x09, // Флаги состояния прерываний				
	REG_BMA_INT_ST_1 = 0x0A, // Флаги состояния прерываний				
	REG_BMA_INT_ST_2 = 0x0B, // Флаги состояния прерываний				
	REG_BMA_INT_ST_3 = 0x0C, // Флаги состояния прерываний				
	REG_BMA_FIFO_ST = 0x0E, // Статус буфера FIFO						
	REG_BMA_RANGE = 0x0F, // Диапазон измеряемого углового ускорения	
	REG_BMA_BW = 0x10, // Полоса пропускания (Bandwidths)			
	REG_BMA_LPW = 0x11, // Выбор основных режимов питания			
	REG_BMA_LOW_POWER = 0x12, // Конфигур. низкого энергопотребл.		
	REG_BMA_D_HBW = 0x13, // Выбор типа данных и их затенение		
	REG_BMA_RESET = 0x14, // Программная перезагрузка				
	REG_BMA_INT_EN_0 = 0x16, // Флаги разрешения прерываний				
	REG_BMA_INT_EN_1 = 0x17, // Флаги разрешения прерываний				
	REG_BMA_INT_EN_2 = 0x18, // Флаги разрешения прерываний				
	REG_BMA_INT_MAP_0 = 0x19, // Флаги направляющие прерывания на INT1	
	REG_BMA_INT_MAP_1 = 0x1A, // Флаги направл-е прерывания на INT1,INT2	
	REG_BMA_INT_MAP_2 = 0x1B, // Флаги направляющие прерывания на INT2	
	REG_BMA_INT_SRC = 0x1E, // Тип данных используемых для прерываний	
	REG_BMA_INT_OUT_CTRL = 0x20, // Выбор поведения выводов INT1 и INT2		
	REG_BMA_INT_RST_LATCH = 0x21, // Выбор режима и сброс прерываний			
	REG_BMA_INT_0 = 0x22, // Значения для прерывания низк. ускорения	
	REG_BMA_INT_1 = 0x23, // Значения для прерывания низк. ускорения	
	REG_BMA_INT_2 = 0x24, // Значения для низк. и высок. ускорения	
	REG_BMA_INT_3 = 0x25, // Значения для прерывания высок. ускорения
	REG_BMA_INT_4 = 0x26, // Значения для прерывания высок. ускорения
	REG_BMA_INT_5 = 0x27, // Значения для прерываний	наклона и ...	
	REG_BMA_INT_6 = 0x28, // Значения для прерывания наклона			
	REG_BMA_INT_7 = 0x29, // Значения для прерывания медл. движения	
	REG_BMA_INT_8 = 0x2A, // Значения для одиночного/двойного касания
	REG_BMA_INT_9 = 0x2B, // Значения для одиночного/двойного касания
	REG_BMA_INT_A = 0x2C, // Значения для прерывания ориентации		
	REG_BMA_INT_B = 0x2D, // Значения для прерывания ориентации		
	REG_BMA_INT_C = 0x2E, // Значения для прерывания в одной плоск.	
	REG_BMA_INT_D = 0x2F, // Значения для прерывания в одной плоск.	
	REG_BMA_FIFO_CONFIG_0 = 0x30, // Уровень watermark для буфера fifo		
	REG_BMA_PMU_SELF_TEST = 0x32, // Настройка и запуск самотестирования		
	REG_BMA_TRIM_NVM_CTRL = 0x33, // Настройка энергонезависимой памяти NVM	
	REG_BMA_BGW_SPI3_WDT = 0x34, // Настройки для цифровых интерфейсов		
	REG_BMA_OFC_CTRL = 0x36, // Управлен. вычислением компенс. смещения	
	REG_BMA_OFC_SETTING = 0x37, // Настройки вычисления компенс. смещения	
	REG_BMA_OFC_OFFSET_X = 0x38, // Значение компенсации смещения по оси X	
	REG_BMA_OFC_OFFSET_Y = 0x39, // Значение компенсации смещения по оси Y	
	REG_BMA_OFC_OFFSET_Z = 0x3A, // Значение компенсации смещения по оси Z	
	REG_BMA_TRIM_GPO = 0x3B, // Регистр памяти NVM общего назначения	
	REG_BMA_TRIM_GP1 = 0x3C, // Регистр памяти NVM общего назначения	
	REG_BMA_FIFO_CONFIG_1 = 0x3E, // Настройки конфигурации буфера FIFO		
	REG_BMA_FIFO_DATA = 0x3F, // Регистр чтения данных из буфера FIFO	
};

enum BMX055::BMGReg : uint8_t
{
	REG_BMG_CHIPID = 0x00, // ID чипа									
	REG_BMG_X_LSB = 0x02, // Данные угловой скорости по оси X		
	REG_BMG_X_MSB = 0x03, // Данные угловой скорости по оси X		
	REG_BMG_Y_LSB = 0x04, // Данные угловой скорости по оси Y		
	REG_BMG_Y_MSB = 0x05, // Данные угловой скорости по оси Y		
	REG_BMG_Z_LSB = 0x06, // Данные угловой скорости по оси Z		
	REG_BMG_Z_MSB = 0x07, // Данные угловой скорости по оси Z		
	REG_BMG_INT_ST_0 = 0x09, // Флаги состояния прерываний				
	REG_BMG_INT_ST_1 = 0x0A, // Флаги состояния прерываний				
	REG_BMG_INT_ST_2 = 0x0B, // Флаги состояния прерываний				
	REG_BMG_INT_ST_3 = 0x0C, // Флаги состояния прерываний				
	REG_BMG_FIFO_ST = 0x0E, // Статус буфера FIFO						
	REG_BMG_RANGE = 0x0F, // Диапазон измеряемой угловой скорости	
	REG_BMG_BW = 0x10, // Полоса пропускания (Bandwidths)			
	REG_BMG_LPM1 = 0x11, // Выбор основных режимов питания			
	REG_BMG_LPM2 = 0x12, // Конфигурация режимов питания.			
	REG_BMG_D_HBW = 0x13, // Выбор типа данных и их затенение		
	REG_BMG_RESET = 0x14, // Программная перезагрузка				
	REG_BMG_INT_EN_0 = 0x15, // Флаги разрешения прерываний				
	REG_BMG_INT_OUT_CTRL = 0x16, // Выбор поведения выводов INT3 и INT4		
	REG_BMG_INT_MAP_0 = 0x17, // Флаги направляющие прерывания на INT3	
	REG_BMG_INT_MAP_1 = 0x18, // Флаги направл-е прерывания на INT3,INT4	
	REG_BMG_INT_MAP_2 = 0x19, // Флаги направляющие прерывания на INT4	
	REG_BMG_INT_SRC = 0x1A, // Тип данных используемых для прерываний	
	REG_BMG_INT_0 = 0x1B, // Значения для прерывания любого движения	
	REG_BMG_INT_1 = 0x1C, // Значения для прерывания любого движения	
	REG_BMG_INT_EN_1 = 0x1E, // Разрешение прерывания FIFO по уровню	
	REG_BMG_INT_RST_LATCH = 0x21, // Выбор режима и сброс прерываний			
	REG_BMG_INT_2 = 0x22, // Значения прерывания высокой скорости	
	REG_BMG_INT_3 = 0x23, // Значения прерывания высокой скорости	
	REG_BMG_INT_4 = 0x24, // Значения прерывания высокой скорости	
	REG_BMG_INT_5 = 0x25, // Значения прерывания высокой скорости	
	REG_BMG_INT_6 = 0x26, // Значения прерывания высокой скорости	
	REG_BMG_INT_7 = 0x27, // Значения прерывания высокой скорости	
	REG_BMG_SOFS = 0x31, // Настройки медленной компенсации смещения
	REG_BMG_AOFS_FOFS = 0x32, // Настр-ки авто/быстр компенсации смещения
	REG_BMG_TRIM_NVM_CTRL = 0x33, // Настройка энергонезависимой памяти NVM	
	REG_BMG_BGW_SPI3_WDT = 0x34, // Настройки для цифровых интерфейсов		
	REG_BMG_OFC_0 = 0x36, // Значения компенсации смещения			
	REG_BMG_OFC_1 = 0x37, // Значения компенсации смещения			
	REG_BMG_OFC_2 = 0x38, // Значения компенсации смещения			
	REG_BMG_OFC_3 = 0x39, // Значения компенсации смещения			
	REG_BMG_TRIM_GP_0 = 0x3A, // Значения компенсации смещения			
	REG_BMG_TRIM_GP_1 = 0x3B, // Регистр памяти NVM общего назначения	
	REG_BMG_BI_SELF_TEST = 0x3C, // Запуск и результат самотестирования		
	REG_BMG_FIFO_CONFIG_0 = 0x3D, // Уровень watermark для буфера FIFO		
	REG_BMG_FIFO_CONFIG_1 = 0x3E, // Настройки конфигурации буфера FIFO		
	REG_BMG_FIFO_DATA = 0x3F, // Регистр чтения данных из буфера FIFO	
};

enum BMX055::BMMReg : uint8_t
{
    REG_BMM_CHIPID = 0x40, // ID чипа									
	REG_BMM_X_LSB = 0x42, // Данные магнитного поля по оси X			
	REG_BMM_X_MSB = 0x43, // Данные магнитного поля по оси X			
	REG_BMM_Y_LSB = 0x44, // Данные магнитного поля по оси Y			
	REG_BMM_Y_MSB = 0x45, // Данные магнитного поля по оси Y			
	REG_BMM_Z_LSB = 0x46, // Данные магнитного поля по оси Z			
	REG_BMM_Z_MSB = 0x47, // Данные магнитного поля по оси Z			
	REG_BMM_TEMP_LSB = 0x48, // Данные сопротивления термодатчика		
	REG_BMM_TEMP_MSB = 0x49, // Данные сопротивления термодатчика		
	REG_BMM_INT_STATUS = 0x4A, // Флаги состояния прерываний				
	REG_BMM_CTRL_0 = 0x4B, // Управление питанием и перезагрузка		
	REG_BMM_CTRL_1 = 0x4C, // Режимы работы, скорость и тестирование	
	REG_BMM_INT_EN_0 = 0x4D, // Флаги разрешения прерываний
	REG_BMM_INT_EN_1 = 0x4E, // Флаги разрешения прерываний				
	REG_BMM_LOW_TH = 0x4F, // Граница низкопорогового  прерывания		
	REG_BMM_HIGH_TH = 0x50, // Граница высокопорогового прерывания		
	REG_BMM_REP_XY = 0x51, // Количество выборок для осей XY			
	REG_BMM_REP_Z = 0x52, // Количество выборок для оси  Z 			
	REG_BMM_DIG_X1 = 0x5D, // Значения корректировки					
	REG_BMM_DIG_Y1 = 0x5E, // Значения корректировки					
	REG_BMM_DIG_Z4_LSB = 0x62, // Значения корректировки					
	REG_BMM_DIG_Z4_MSB = 0x63, // Значения корректировки					
	REG_BMM_DIG_X2 = 0x64, // Значения корректировки					
	REG_BMM_DIG_Y2 = 0x65, // Значения корректировки					
	REG_BMM_DIG_Z2_LSB = 0x68, // Значения корректировки					
	REG_BMM_DIG_Z2_MSB = 0x69, // Значения корректировки					
	REG_BMM_DIG_Z1_LSB = 0x6A, // Значения корректировки					
	REG_BMM_DIG_Z1_MSB = 0x6B, // Значения корректировки					
	REG_BMM_DIG_XYZ1_LSB = 0x6C, // Значения корректировки					
	REG_BMM_DIG_XYZ1_MSB = 0x6D, // Значения корректировки					
	REG_BMM_DIG_Z3_LSB = 0x6E, // Значения корректировки					
	REG_BMM_DIG_Z3_MSB = 0x6F, // Значения корректировки					
	REG_BMM_DIG_XY2 = 0x70, // Значения корректировки					
	REG_BMM_DIG_XY1 = 0x71, // Значения корректировки					
};

#endif