#ifndef _FLASHDRIVE_H
#define _FLASHDRIVE_H

#include "flash.h"
#include "usb/usbMsc/scsiinterface.h"

class FlashDrive : public ScsiInterface
{
private:
    typedef struct
    {
        union
        {
            unsigned char flags;
            struct
            {
                unsigned char blank: 1;
                unsigned char valid: 1;
            };
        };
        unsigned char _reserve;
        unsigned short sectorNumber;
    } Header;
    
    static const int mSectorSize = 512;
    static const int mChunkSize = mSectorSize + sizeof(Header);
    
    typedef struct
    {
        Header hdr;
        unsigned long data[mSectorSize/4];
    } Sector;
  
    unsigned long mAddress;
    unsigned long mEnd;
    unsigned long mStorageSize;
    
    Sector *findSector(uint32_t blk_addr);
    void invalidateSector(Sector *sect);
  
public:
    FlashDrive(Flash::Sector firstSector, Flash::Sector lastSector);
    int blockCount() {return mStorageSize / mChunkSize;}
    int blockSize() {return mSectorSize;}
    
    virtual int8_t getCapacity(uint32_t *block_num, uint32_t *block_size);
    virtual int8_t read(uint8_t *buf, uint32_t blk_addr, uint16_t count);
    virtual int8_t write(uint8_t *buf, uint32_t blk_addr, uint16_t count);
};

#endif