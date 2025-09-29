#ifndef _CANOPENPROXY_H
#define _CANOPENPROXY_H

#include <cassert>
#include <initializer_list>
#include <queue>
#include <type_traits>
#include <utility>

#include "canopencommon.h"
#include "cansocket.h"
#include "core/timer.h"

class CanOpenProxy
{
public:
    struct ODEntry
    {
        uint16_t id;
        uint8_t  sid;
        uint8_t  size;
    };
    
    /// Compile-time Object Dictionary Entry description
    /// @attention See objdict.h to see usage
    template<typename T, uint16_t id, uint8_t sid = 0>
    struct ODEntryMeta
    {
        using Type = T;
        static_assert(std::is_integral_v<T> && sizeof(T) <= 4, "Type mismatch");
        
        static constexpr ODEntry value = {id, sid, sizeof(T)};
    };
    
    struct PDOEntry
    {
        uint32_t functionCode;
        uint32_t commIndex;
        uint32_t mapIndex;
        bool     isTx;
    };
    
    /// Compile-time PDO Entry description
    /// @attention See objdict.h to see usage
    template<uint32_t functionCode, uint32_t commIndex, uint32_t mapIndex, bool isTx>
    struct PDOEntryMeta
    {
        static constexpr PDOEntry value = {functionCode, commIndex, mapIndex, isTx};
    };
    
    using SDOList = std::initializer_list<ODEntry>;
    
    /// Base class to define PDO mapping
    template<typename ... Entries>
    struct PDO
    {
        static constexpr SDOList sdoList = {Entries::value...};
    };

public:
    using SDO          = CanOpen::SDO;
    using NMTState     = CanOpen::NMTState;
    using NMTControl   = CanOpen::NMTControl;
    using SDOAbortCode = CanOpen::SDOAbortCode;

public:
    CanOpenProxy(CanInterface *can, uint8_t nodeId);
    uint8_t nodeId() const {return m_nodeId;}
    
    NMTState nmtState() const {return static_cast<NMTState>(m_nmtState & 0x7F);}

    void nmtModuleControl(NMTControl cmd);
    void nmtErrorControl(); // request node state
    
    /// Issue SYNC message
    void sync();
    
    void pdoWrite(uint8_t pdo, const ByteArray &value);
    
    /// Send value via SDO
    /// @param Entry ODEntryMeta destination instance
    /// @param value value to be sent
    template<typename Entry>
    inline void sdoWrite(typename Entry::Type value)
        __attribute__((always_inline));

    /// Request value via SDO
    /// @param Entry ODEntryMeta source instance
    template<typename Entry>
    inline void sdoRead()
        __attribute__((always_inline));
    
    /// Configure PDO with SYNC trigger
    /// @param PDO PDOEntryMeta source instance
    /// @param PDOStruct "PDO" structrure inheritant
    /// @param x period in number of SYNCs received
    template<typename PDO, typename PDOStruct>
    inline void configPDOSync(uint8_t x = 0)
        __attribute__((always_inline));

    /// Configure PDO with ManufacturerEvent trigger
    /// @param PDO PDOEntryMeta source instance
    /// @param PDOStruct "PDO" structrure inheritant
    /// @param eTime event timer
    /// @param iTime inhibit time
    template<typename PDO, typename PDOStruct>
    inline void configPDOManEvent(uint16_t eTime = 0, uint16_t iTime = 0)
        __attribute__((always_inline));
    
    /// Configure PDO with ProfileEvent trigger
    /// @param PDO PDOEntryMeta source instance
    /// @param PDOStruct "PDO" structrure inheritant
    /// @param eTime event timer
    /// @param iTime inhibit time
    template<typename PDO, typename PDOStruct>
    inline void configPDOProfEvent(uint16_t eTime = 0, uint16_t iTime = 0)
        __attribute__((always_inline));

    void sdoRead(uint16_t id, uint8_t sid, uint8_t size);
    void sdoWrite(uint16_t id, uint8_t sid, uint8_t size, uint32_t value);
    
private:
    void sdoReadImpl(ODEntry entry);
    void sdoWriteImpl(ODEntry entry, uint32_t value);

    void configPDOImpl(PDOEntry entry, SDOList sdoList,
                       uint8_t tType, uint16_t eTime, uint8_t iTime);
    
protected:
    virtual void nmtStateChanged() {}
    virtual void emergency() {}
    virtual void pdoReceived(uint8_t pdo, const ByteArray &value) {}
    virtual void sdoReceived(uint16_t id, uint8_t subid, uint32_t value) {}
    virtual void sdoWritten(uint16_t id, uint8_t subid) {}
    virtual void sdoError(SDOAbortCode errcode) {}
    
private:
    CanSocket *m_can;
    uint8_t m_nodeId;
    uint8_t m_nmtState;
    
    std::queue<SDO> m_sdoQueue;
    std::queue<uint8_t> m_nmtQueue;
    bool m_nmtErrorControl = false;
    Timer *m_resendTimer;
    void task();
    void resendSdo();
    
    void sdoEnqueue(SDO &&sdo);
    void sendSdo(uint8_t cmd, uint16_t id, uint8_t sid, uint8_t size, uint32_t value);
    
    bool sendPacket(uint16_t cob_id, const ByteArray &payload = ByteArray());
    void readPacket();
    void handlePacket(uint16_t cob_id, const ByteArray &payload);
};

template<typename Entry>
void CanOpenProxy::sdoWrite(typename Entry::Type data)
{
    sdoWriteImpl(Entry::value, data);
}

template<typename Entry>
void CanOpenProxy::sdoRead()
{
    sdoReadImpl(Entry::value);
}

template<typename PDO, typename PDOStruct>
void CanOpenProxy::configPDOSync(uint8_t x)
{
    configPDOImpl(PDO::value, PDOStruct::Layout::sdoList, x, 0, 0);
}

template<typename PDO, typename PDOStruct>
void CanOpenProxy::configPDOManEvent(uint16_t eTime, uint16_t iTime)
{
    configPDOImpl(PDO::value, PDOStruct::Layout::sdoList, 0xfe, eTime, iTime);
}

template<typename PDO, typename PDOStruct>
void CanOpenProxy::configPDOProfEvent(uint16_t eTime, uint16_t iTime)
{
    configPDOImpl(PDO::value, PDOStruct::Layout::sdoList, 0xff, eTime, iTime);
}

#endif // _CANOPENPROXY_H
