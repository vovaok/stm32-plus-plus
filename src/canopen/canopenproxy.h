#ifndef _CANOPENPROXY_H
#define _CANOPENPROXY_H

#include "canopencommon.h"
#include "cansocket.h"
#include <queue>
#include <initializer_list>
#include <utility>
#include "core/timer.h"

using namespace CanOpen;

class CanOpenProxy
{
public:
    CanOpenProxy(CanInterface *can, uint8_t nodeId);
    uint8_t nodeId() const {return m_nodeId;}
    
    NMTState nmtState() const {return static_cast<NMTState>(m_nmtState & 0x7F);}

    void nmtModuleControl(NMTControl cmd);
    void nmtErrorControl(); // request node state
    
    void pdoWrite(uint8_t pdo, const ByteArray &value);
    
    void sdoRead(uint16_t id, uint8_t subid, uint8_t size);
    void sdoRead8(uint16_t id, uint8_t subid);
    void sdoRead16(uint16_t id, uint8_t subid);
    void sdoRead32(uint16_t id, uint8_t subid);
    void sdoWrite(uint16_t id, uint8_t subid, uint32_t value, uint8_t size);
    void sdoWrite8(uint16_t id, uint8_t subid, uint8_t value);
    void sdoWrite16(uint16_t id, uint8_t subid, uint16_t value);
    void sdoWrite32(uint16_t id, uint8_t subid, uint32_t value);
    
    /// Configure PDO
    /// @attention Event-driven transmission and reception ONLY!
    /// @param func must be one of: PDOn_TX, PDOn_RX (n=1...4)
    /// @param sdo_list is the list of SDO entries
    /// @param interval_ms is the transmit interval for TPDO
    /// @return true if success
    /// @todo maybe refactor this?
    bool configPdo(FunctionCode func, std::initializer_list<uint32_t> sdo_list, int interval_ms=0);
  
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

#endif // _CANOPENPROXY_H
