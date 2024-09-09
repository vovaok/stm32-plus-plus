#pragma once

#include "modbusproxy.h"

class CommonProxy : public ModbusProxy
{
public:
    CommonProxy(uint8_t address);
    virtual ~CommonProxy();
    
    //! —инхронизаци€ регистров и коилов с удаленным устройством
    void sync();

    //! ѕроверка наличи€ ведомого устройства
    bool isValid() const {return m_validityTimeout;}

    //! ѕрив€зка блока холдинг регистров
    template <typename T>
    void bindHoldingRegisters(uint16_t baseAddress, T &regs)
    {
        m_holdingRegs = reinterpret_cast<uint16_t *>(&regs);
        m_holdingCount = sizeof(T) / sizeof(uint16_t);
        m_holdingBase = baseAddress;
        if (m_shadowRegs)
            delete [] m_shadowRegs;
        m_shadowRegs = new uint16_t[m_holdingCount];
        for (int i=0; i<m_holdingCount; i++)
            m_shadowRegs[i] = m_holdingRegs[i];
    }

    //! ѕрив€зка блока инпут регистров
    template <typename T>
    void bindInputRegisters(uint16_t baseAddress, T &regs)
    {
        m_inputRegs = reinterpret_cast<uint16_t *>(&regs);
        m_inputCount = sizeof(T) / sizeof(uint16_t);
        m_inputBase = baseAddress;
    }

    //! ѕрив€зка блока коилов
    //!  аждый коил должен быть uint8_t и принимать значени€ 0 или 1
    template <typename T>
    void bindCoils(uint16_t baseAddress, T &coils)
    {
        m_coils = reinterpret_cast<uint8_t *>(&coils);
        m_coilsCount = sizeof(T);
        m_coilsBase = baseAddress;
    }

    //! «апись указанного значени€ холдинг регистра
    //! (регистр определ€етс€ по ссылке из прив€занного блока)
    template<typename T>
    void writeHoldingRegister(T &reg, T value)
    {
        writeRegs(&reg, sizeof(T) / sizeof(uint16_t), &value);
    }

    //! ѕолучение адреса регистра по его ссылке из прив€занного блока
    template<typename T>
    uint16_t getRegAddress(T &reg)
    {
        return holdingRegAddr(&reg);
    }

    //! «апись нескольких регистров данными из прив€занного блока
    //! по заданному адресу и количеству регистров
    void writeHoldingRegisters(uint16_t addr, int count);
    
    uint16_t *holdingRegs() {return m_holdingRegs;}
    uint16_t *inputRegs() {return m_inputRegs;}
    uint8_t *coils() {return m_coils;}
    
    int holdingCount() const {return m_holdingCount;}
    int inputCount() const {return m_inputCount;}
    int coilsCount() const {return m_coilsCount;}
    
    int holdingBase() const {return m_holdingBase;}
    int inputBase() const {return m_inputBase;}
    int coilsBase() const {return m_coilsBase;}

protected:
    virtual void errorEvent(Modbus::ExceptionCode e) override;
    virtual void coilReceived(uint16_t addr, bool value) override;
//    virtual void discreteInputReceived(uint16_t addr, bool value) override;
    virtual void holdingRegisterReceived(uint16_t addr, uint16_t value) override;
    virtual void inputRegisterReceived(uint16_t addr, uint16_t value) override;

    // таймер работоспособности удалЄнного устройства:
    int m_validityTimeout = 0;
    // указатели на регистры и коилы (надо их забиндить):
    uint16_t *m_holdingRegs = nullptr;
    uint16_t *m_inputRegs = nullptr;
    uint8_t *m_coils = nullptr;
    // количество регистров и коилов (задаютс€ в бинде):
    uint16_t m_holdingCount = 0;
    uint16_t m_inputCount = 0;
    uint16_t m_coilsCount = 0;
    // начальные адреса регистров и коилов (задаютс€ в бинде):
    uint16_t m_holdingBase = 0;
    uint16_t m_inputBase = 0;
    uint16_t m_coilsBase = 0;

    uint16_t holdingRegAddr(void *ptr);
    void readRegs(void *ptr, int count);
    void writeRegs(void *ptr, int count, void *data);
    
    void writeChanges();
    
private:
    // локальна€ копи€ холдинг регистров (дл€ правильной логики чтени€/записи)
    // хранит предыдущие значени€ регистров до синхронизации
    uint16_t *m_shadowRegs = nullptr;
};