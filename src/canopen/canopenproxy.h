#ifndef _CANOPENPROXY_H
#define _CANOPENPROXY_H

#include "canopencommon.h"
#include "cansocket.h"
#include <queue>
#include <initializer_list>
#include <utility>
#include <type_traits>

#include "core/timer.h"

using namespace CanOpen;

class CanOpenProxy
{
public:
    /* Object Dictionary entry template */
    template<typename T> struct ODEntry {
        using Type = T;
        static_assert(std::is_integral_v<T> && sizeof(T) <= 4, "Type mismatch");
        
        uint16_t id;
        uint8_t sid;
    };
        
    template<typename T, uint16_t Id_, uint8_t Sid_ = 0>
    struct ODEntryMeta: public ODEntry<T> {
        static constexpr uint16_t Id = Id_;
        static constexpr uint8_t Sid = Sid_;
        static constexpr uint8_t Size = sizeof(T);
        
        static constexpr uint32_t SdoId = (Id_ << 16) | (Sid_ << 8) | (sizeof(T) * 8);
        
        ODEntryMeta(): ODEntry<T>{Id, Sid} {}
    };
    
public:
    CanOpenProxy(CanInterface *can, uint8_t nodeId);
    uint8_t nodeId() const {return m_nodeId;}
    
    NMTState nmtState() const {return static_cast<NMTState>(m_nmtState & 0x7F);}

    void nmtModuleControl(NMTControl cmd);
    void nmtErrorControl(); // request node state
    
    /// Issue SYNC message
    void sync();
    
    void pdoWrite(uint8_t pdo, const ByteArray &value);
    
    void sdoRead(uint16_t id, uint8_t subid, uint8_t size);
    void sdoRead8(uint16_t id, uint8_t subid);
    void sdoRead16(uint16_t id, uint8_t subid);
    void sdoRead32(uint16_t id, uint8_t subid);
    void sdoWrite(uint16_t id, uint8_t subid, uint32_t value, uint8_t size);
    void sdoWrite8(uint16_t id, uint8_t subid, uint8_t value);
    void sdoWrite16(uint16_t id, uint8_t subid, uint16_t value);
    void sdoWrite32(uint16_t id, uint8_t subid, uint32_t value);
    
    /// @brief Generic interface for sdoWrite.
    /// @param entry is Object Dictionary meta-entry defined as ODEntryMeta struct
    /// @param value to be sent of a corresponding type
    /// @note Always inlined to prevent generating symbols on every instantiation
    template<typename EntryMeta>
    inline void sdoWrite(typename EntryMeta::Type value)
        __attribute__((always_inline));
    
    /// Configure PDO
    /// @attention Event-driven transmission and reception ONLY!
    /// @param func must be one of: PDOn_TX, PDOn_RX (n=1...4)
    /// @param sdo_list is the list of SDO entries
    /// @param interval_ms is the transmit interval for TPDO
    /// @return true if success
    /// @todo maybe refactor this?
    bool configPdo(FunctionCode func, std::initializer_list<uint32_t> sdo_list, int interval=0, bool use_sync=false);
  
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
    bool sendPacket(uint16_t cob_id, const ByteArray &payload = ByteArray());
    void readPacket();
    void handlePacket(uint16_t cob_id, const ByteArray &payload);
};

template<typename Entry>
void CanOpenProxy::sdoWrite(typename Entry::Type value)
{
    sdoWrite(Entry::Id, Entry::Sid, value, sizeof(value));
}

#endif // _CANOPENPROXY_H
