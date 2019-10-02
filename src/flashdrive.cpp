#include "flashdrive.h"
#include "core/core.h"

unsigned long aaa[512/4];

FlashDrive::FlashDrive(Flash::Sector firstSector, Flash::Sector lastSector)
{
    setVendor("Neuro");
    setProduct("Flash drive");
    mAddress = Flash::getBeginOfSector(firstSector);
    mStorageSize = Flash::getBeginOfSector(lastSector) + Flash::getSizeOfSector(lastSector) - mAddress;
}

int8_t FlashDrive::getCapacity(uint32_t *block_num, uint32_t *block_size)
{
    *block_size = 512;  
    *block_num = mStorageSize / 512; // 128 kB
    return 0; // OK
}

int8_t FlashDrive::read(uint8_t *buf, uint32_t blk_addr, uint16_t count)
{
//    printf("MSC read from 0x%04X, size=%d\n", blk_addr, count);
    
    unsigned long *src = (unsigned long*)mAddress + blk_addr*512/4;
//    if (!blk_addr && count == 1)
//        src = aaa;
    
    for (int i=0; i<count*512/4; i++)
        ((unsigned long*)buf)[i] = ~src[i];

    return 0; // OK
}

int8_t FlashDrive::write(uint8_t *buf, uint32_t blk_addr, uint16_t count)
{
    //printf("MSC write to 0x%04X, size=%d\n", blk_addr, count);
      
//    writeTimeout = 50;
  
    unsigned long *dst = (unsigned long*)mAddress + blk_addr*512/4;
//    if (!blk_addr && count == 1)
//        dst = aaa;
    
    int errors = 0;
    for (int i=0; i<count*512/4; i++)
    {
        unsigned long v = ~((unsigned long*)buf)[i];
        if ((dst[i] & v) != v)
            errors++;
    }
    if (errors)
        printf("Flash write to 0x%04X errors: %d\n", blk_addr, errors);
    
    Flash::unlock();
    Flash::programDataInverted((unsigned long)dst, buf, count*512);
//    for (int i=0; i<count*512/4; i++)
//        dst[i] = ((unsigned long*)buf)[i];
    Flash::lock();
    
    return 0; // OK
}
