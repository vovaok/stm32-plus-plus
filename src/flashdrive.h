#ifndef _FLASHDRIVE_H
#define _FLASHDRIVE_H

#include "flash.h"
#include "usb/usbMsc/scsiinterface.h"

class FlashDrive : public ScsiInterface
{
private:
    unsigned long mAddress;
    unsigned long mStorageSize;
  
public:
    FlashDrive(Flash::Sector firstSector, Flash::Sector lastSector);
    int blockCount() {return mStorageSize / 512;}
    int blockSize() {return 512;}
    
    virtual int8_t getCapacity(uint32_t *block_num, uint32_t *block_size);
    virtual int8_t read(uint8_t *buf, uint32_t blk_addr, uint16_t count);
    virtual int8_t write(uint8_t *buf, uint32_t blk_addr, uint16_t count);
};

#endif