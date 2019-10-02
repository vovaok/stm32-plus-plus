#ifndef _SCSIINTERFACE_H
#define _SCSIINTERFACE_H

#include <stdint.h>

class ScsiInterface
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char spec[8];
        char vendor[8];
        char product[16];
        char revision[4];
    } Inquiry;
#pragma pack(pop)
    
    const static Inquiry m_defInq;
    Inquiry inquiry;
    inline uint8_t *getInquiry() {return reinterpret_cast<uint8_t*>(&inquiry);}
    
    friend class SCSI;
    
protected:
//    virtual int8_t init(int);
    virtual int8_t getCapacity(uint32_t *block_num, uint32_t *block_size) = 0;
    virtual int8_t read(uint8_t *buf, uint32_t blk_addr, uint16_t count) = 0;
    virtual int8_t write(uint8_t *buf, uint32_t blk_addr, uint16_t count) = 0;
    virtual int8_t isReady() {return 0;}
    virtual int8_t isWriteProtected() {return 0;}
    
public:
    ScsiInterface();
  
    void setVendor(const char *s);
    void setProduct(const char *s);
    void setRevision(const char *s);
};

#endif