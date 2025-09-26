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
    
    enum BMAReg : uint8_t;
    enum BMGReg : uint8_t;
    enum BMMReg : uint8_t;
    
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
    REG_BMA_CHIPID = 0x00, // ID ����
	REG_BMA_X_LSB = 0x02, // ������ �������� ��������� �� ��� X		
	REG_BMA_X_MSB = 0x03, // ������ �������� ��������� �� ��� X		
	REG_BMA_Y_LSB = 0x04, // ������ �������� ��������� �� ��� Y		
	REG_BMA_Y_MSB = 0x05, // ������ �������� ��������� �� ��� Y		
	REG_BMA_Z_LSB = 0x06, // ������ �������� ��������� �� ��� Z		
	REG_BMA_Z_MSB = 0x07, // ������ �������� ��������� �� ��� Z		
	REG_BMA_TEMP = 0x08, // ������ ����������� ����	� �C			
	REG_BMA_INT_ST_0 = 0x09, // ����� ��������� ����������				
	REG_BMA_INT_ST_1 = 0x0A, // ����� ��������� ����������				
	REG_BMA_INT_ST_2 = 0x0B, // ����� ��������� ����������				
	REG_BMA_INT_ST_3 = 0x0C, // ����� ��������� ����������				
	REG_BMA_FIFO_ST = 0x0E, // ������ ������ FIFO						
	REG_BMA_RANGE = 0x0F, // �������� ����������� �������� ���������	
	REG_BMA_BW = 0x10, // ������ ����������� (Bandwidths)			
	REG_BMA_LPW = 0x11, // ����� �������� ������� �������			
	REG_BMA_LOW_POWER = 0x12, // ��������. ������� �������������.		
	REG_BMA_D_HBW = 0x13, // ����� ���� ������ � �� ���������		
	REG_BMA_RESET = 0x14, // ����������� ������������				
	REG_BMA_INT_EN_0 = 0x16, // ����� ���������� ����������				
	REG_BMA_INT_EN_1 = 0x17, // ����� ���������� ����������				
	REG_BMA_INT_EN_2 = 0x18, // ����� ���������� ����������				
	REG_BMA_INT_MAP_0 = 0x19, // ����� ������������ ���������� �� INT1	
	REG_BMA_INT_MAP_1 = 0x1A, // ����� �������-� ���������� �� INT1,INT2	
	REG_BMA_INT_MAP_2 = 0x1B, // ����� ������������ ���������� �� INT2	
	REG_BMA_INT_SRC = 0x1E, // ��� ������ ������������ ��� ����������	
	REG_BMA_INT_OUT_CTRL = 0x20, // ����� ��������� ������� INT1 � INT2		
	REG_BMA_INT_RST_LATCH = 0x21, // ����� ������ � ����� ����������			
	REG_BMA_INT_0 = 0x22, // �������� ��� ���������� ����. ���������	
	REG_BMA_INT_1 = 0x23, // �������� ��� ���������� ����. ���������	
	REG_BMA_INT_2 = 0x24, // �������� ��� ����. � �����. ���������	
	REG_BMA_INT_3 = 0x25, // �������� ��� ���������� �����. ���������
	REG_BMA_INT_4 = 0x26, // �������� ��� ���������� �����. ���������
	REG_BMA_INT_5 = 0x27, // �������� ��� ����������	������� � ...	
	REG_BMA_INT_6 = 0x28, // �������� ��� ���������� �������			
	REG_BMA_INT_7 = 0x29, // �������� ��� ���������� ����. ��������	
	REG_BMA_INT_8 = 0x2A, // �������� ��� ����������/�������� �������
	REG_BMA_INT_9 = 0x2B, // �������� ��� ����������/�������� �������
	REG_BMA_INT_A = 0x2C, // �������� ��� ���������� ����������		
	REG_BMA_INT_B = 0x2D, // �������� ��� ���������� ����������		
	REG_BMA_INT_C = 0x2E, // �������� ��� ���������� � ����� �����.	
	REG_BMA_INT_D = 0x2F, // �������� ��� ���������� � ����� �����.	
	REG_BMA_FIFO_CONFIG_0 = 0x30, // ������� watermark ��� ������ fifo		
	REG_BMA_PMU_SELF_TEST = 0x32, // ��������� � ������ ����������������		
	REG_BMA_TRIM_NVM_CTRL = 0x33, // ��������� ����������������� ������ NVM	
	REG_BMA_BGW_SPI3_WDT = 0x34, // ��������� ��� �������� �����������		
	REG_BMA_OFC_CTRL = 0x36, // ��������. ����������� �������. ��������	
	REG_BMA_OFC_SETTING = 0x37, // ��������� ���������� �������. ��������	
	REG_BMA_OFC_OFFSET_X = 0x38, // �������� ����������� �������� �� ��� X	
	REG_BMA_OFC_OFFSET_Y = 0x39, // �������� ����������� �������� �� ��� Y	
	REG_BMA_OFC_OFFSET_Z = 0x3A, // �������� ����������� �������� �� ��� Z	
	REG_BMA_TRIM_GPO = 0x3B, // ������� ������ NVM ������ ����������	
	REG_BMA_TRIM_GP1 = 0x3C, // ������� ������ NVM ������ ����������	
	REG_BMA_FIFO_CONFIG_1 = 0x3E, // ��������� ������������ ������ FIFO		
	REG_BMA_FIFO_DATA = 0x3F, // ������� ������ ������ �� ������ FIFO	
};

enum BMX055::BMGReg : uint8_t
{
	REG_BMG_CHIPID = 0x00, // ID ����									
	REG_BMG_X_LSB = 0x02, // ������ ������� �������� �� ��� X		
	REG_BMG_X_MSB = 0x03, // ������ ������� �������� �� ��� X		
	REG_BMG_Y_LSB = 0x04, // ������ ������� �������� �� ��� Y		
	REG_BMG_Y_MSB = 0x05, // ������ ������� �������� �� ��� Y		
	REG_BMG_Z_LSB = 0x06, // ������ ������� �������� �� ��� Z		
	REG_BMG_Z_MSB = 0x07, // ������ ������� �������� �� ��� Z		
	REG_BMG_INT_ST_0 = 0x09, // ����� ��������� ����������				
	REG_BMG_INT_ST_1 = 0x0A, // ����� ��������� ����������				
	REG_BMG_INT_ST_2 = 0x0B, // ����� ��������� ����������				
	REG_BMG_INT_ST_3 = 0x0C, // ����� ��������� ����������				
	REG_BMG_FIFO_ST = 0x0E, // ������ ������ FIFO						
	REG_BMG_RANGE = 0x0F, // �������� ���������� ������� ��������	
	REG_BMG_BW = 0x10, // ������ ����������� (Bandwidths)			
	REG_BMG_LPM1 = 0x11, // ����� �������� ������� �������			
	REG_BMG_LPM2 = 0x12, // ������������ ������� �������.			
	REG_BMG_D_HBW = 0x13, // ����� ���� ������ � �� ���������		
	REG_BMG_RESET = 0x14, // ����������� ������������				
	REG_BMG_INT_EN_0 = 0x15, // ����� ���������� ����������				
	REG_BMG_INT_OUT_CTRL = 0x16, // ����� ��������� ������� INT3 � INT4		
	REG_BMG_INT_MAP_0 = 0x17, // ����� ������������ ���������� �� INT3	
	REG_BMG_INT_MAP_1 = 0x18, // ����� �������-� ���������� �� INT3,INT4	
	REG_BMG_INT_MAP_2 = 0x19, // ����� ������������ ���������� �� INT4	
	REG_BMG_INT_SRC = 0x1A, // ��� ������ ������������ ��� ����������	
	REG_BMG_INT_0 = 0x1B, // �������� ��� ���������� ������ ��������	
	REG_BMG_INT_1 = 0x1C, // �������� ��� ���������� ������ ��������	
	REG_BMG_INT_EN_1 = 0x1E, // ���������� ���������� FIFO �� ������	
	REG_BMG_INT_RST_LATCH = 0x21, // ����� ������ � ����� ����������			
	REG_BMG_INT_2 = 0x22, // �������� ���������� ������� ��������	
	REG_BMG_INT_3 = 0x23, // �������� ���������� ������� ��������	
	REG_BMG_INT_4 = 0x24, // �������� ���������� ������� ��������	
	REG_BMG_INT_5 = 0x25, // �������� ���������� ������� ��������	
	REG_BMG_INT_6 = 0x26, // �������� ���������� ������� ��������	
	REG_BMG_INT_7 = 0x27, // �������� ���������� ������� ��������	
	REG_BMG_SOFS = 0x31, // ��������� ��������� ����������� ��������
	REG_BMG_AOFS_FOFS = 0x32, // �����-�� ����/����� ����������� ��������
	REG_BMG_TRIM_NVM_CTRL = 0x33, // ��������� ����������������� ������ NVM	
	REG_BMG_BGW_SPI3_WDT = 0x34, // ��������� ��� �������� �����������		
	REG_BMG_OFC_0 = 0x36, // �������� ����������� ��������			
	REG_BMG_OFC_1 = 0x37, // �������� ����������� ��������			
	REG_BMG_OFC_2 = 0x38, // �������� ����������� ��������			
	REG_BMG_OFC_3 = 0x39, // �������� ����������� ��������			
	REG_BMG_TRIM_GP_0 = 0x3A, // �������� ����������� ��������			
	REG_BMG_TRIM_GP_1 = 0x3B, // ������� ������ NVM ������ ����������	
	REG_BMG_BI_SELF_TEST = 0x3C, // ������ � ��������� ����������������		
	REG_BMG_FIFO_CONFIG_0 = 0x3D, // ������� watermark ��� ������ FIFO		
	REG_BMG_FIFO_CONFIG_1 = 0x3E, // ��������� ������������ ������ FIFO		
	REG_BMG_FIFO_DATA = 0x3F, // ������� ������ ������ �� ������ FIFO	
};

enum BMX055::BMMReg : uint8_t
{
    REG_BMM_CHIPID = 0x40, // ID ����									
	REG_BMM_X_LSB = 0x42, // ������ ���������� ���� �� ��� X			
	REG_BMM_X_MSB = 0x43, // ������ ���������� ���� �� ��� X			
	REG_BMM_Y_LSB = 0x44, // ������ ���������� ���� �� ��� Y			
	REG_BMM_Y_MSB = 0x45, // ������ ���������� ���� �� ��� Y			
	REG_BMM_Z_LSB = 0x46, // ������ ���������� ���� �� ��� Z			
	REG_BMM_Z_MSB = 0x47, // ������ ���������� ���� �� ��� Z			
	REG_BMM_TEMP_LSB = 0x48, // ������ ������������� ������������		
	REG_BMM_TEMP_MSB = 0x49, // ������ ������������� ������������		
	REG_BMM_INT_STATUS = 0x4A, // ����� ��������� ����������				
	REG_BMM_CTRL_0 = 0x4B, // ���������� �������� � ������������		
	REG_BMM_CTRL_1 = 0x4C, // ������ ������, �������� � ������������	
	REG_BMM_INT_EN_0 = 0x4D, // ����� ���������� ����������
	REG_BMM_INT_EN_1 = 0x4E, // ����� ���������� ����������				
	REG_BMM_LOW_TH = 0x4F, // ������� ���������������  ����������		
	REG_BMM_HIGH_TH = 0x50, // ������� ���������������� ����������		
	REG_BMM_REP_XY = 0x51, // ���������� ������� ��� ���� XY			
	REG_BMM_REP_Z = 0x52, // ���������� ������� ��� ���  Z 			
	REG_BMM_DIG_X1 = 0x5D, // �������� �������������					
	REG_BMM_DIG_Y1 = 0x5E, // �������� �������������					
	REG_BMM_DIG_Z4_LSB = 0x62, // �������� �������������					
	REG_BMM_DIG_Z4_MSB = 0x63, // �������� �������������					
	REG_BMM_DIG_X2 = 0x64, // �������� �������������					
	REG_BMM_DIG_Y2 = 0x65, // �������� �������������					
	REG_BMM_DIG_Z2_LSB = 0x68, // �������� �������������					
	REG_BMM_DIG_Z2_MSB = 0x69, // �������� �������������					
	REG_BMM_DIG_Z1_LSB = 0x6A, // �������� �������������					
	REG_BMM_DIG_Z1_MSB = 0x6B, // �������� �������������					
	REG_BMM_DIG_XYZ1_LSB = 0x6C, // �������� �������������					
	REG_BMM_DIG_XYZ1_MSB = 0x6D, // �������� �������������					
	REG_BMM_DIG_Z3_LSB = 0x6E, // �������� �������������					
	REG_BMM_DIG_Z3_MSB = 0x6F, // �������� �������������					
	REG_BMM_DIG_XY2 = 0x70, // �������� �������������					
	REG_BMM_DIG_XY1 = 0x71, // �������� �������������					
};

#endif