#pragma once

#include "caninterface.h"
#include "core/device.h"

class CanSocket : public Device
{
public:
    CanSocket(CanInterface *can, int flags=CanInterface::StdId);

    ~CanSocket();
    
    void setFixedId(int id, int recv_id=-1);

    CanInterface *interface() {return m_can;}
    int fifoChannel() const {return m_fifoChannel;}
    int flags() const {return m_flags;}

    int addFilter(uint32_t id, uint32_t mask);
    bool removeFilter(int index);

    virtual int bytesAvailable() const override;

    //! Receive pending message
    //! In FixedId mode the data is the payload without ID
    //! In StdId mode the first 2 bytes contains 11-bit ID in LE format
    //! In ExtId mode the first 4 bytes contains 29-bit ID in LE format
    //! @return length of the message including size of ID (2 or 4)
    //! @note If there is no message pending readData() returns -1
    virtual int readData(char *data, int size) override;

    //! Transmit a message
    //! In FixedId mode the data is the payload without ID
    //! In StdId mode the first 2 bytes must contain 11-bit ID in LE format
    //! In ExtId mode the first 4 bytes must contain 29-bit ID in LE format
    //! @arg size must include size of ID (2 or 4) and payload length
    //! @return provided size on success or -1 if transmit fails (e.g. TX FIFO is full)
    virtual int writeData(const char *data, int size) override;

private:
    CanInterface *m_can;
    CanInterface::Flags m_flags;
    int m_fixedId = -1;
    int m_fifoChannel = -1;
    int m_filterIdx = -1;
    bool m_shareChannel = false;
    
    inline bool isFixedId() const {return m_fixedId >= 0;}
    
    friend class CanInterface;
};