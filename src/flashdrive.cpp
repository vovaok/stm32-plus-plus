#include "flashdrive.h"
#include "core/core.h"

FlashDrive::FlashDrive(Flash::Sector firstSector, Flash::Sector lastSector)
{
    setVendor("Neuro");
    setProduct("Flash drive");
    mAddress = Flash::getBeginOfSector(firstSector);
    mEnd = Flash::getBeginOfSector(lastSector) + Flash::getSizeOfSector(lastSector);
    mStorageSize = mEnd - mAddress;
}

int8_t FlashDrive::getCapacity(uint32_t *block_num, uint32_t *block_size)
{
    *block_size = mSectorSize;  
    *block_num = mStorageSize / (mChunkSize);
    return 0; // OK
}

FlashDrive::Sector *FlashDrive::findSector(uint32_t blk_addr)
{
    for (Sector *sect=(Sector*)mAddress; sect<(Sector*)mEnd; sect++)
    {
        const Header *hdr = &sect->hdr;
        if (hdr->blank)
            return sect;
        if (hdr->sectorNumber == blk_addr && hdr->valid)
            return sect;
    }
    return 0L;
}

int8_t FlashDrive::read(uint8_t *buf, uint32_t blk_addr, uint16_t count)
{
//    printf("MSC read from 0x%04X, size=%d\n", blk_addr, count);
    
    for (int i=0; i<count; i++)
    {
        Sector *sect = findSector(blk_addr + i);
        if (!sect)
            return -1;
        for (int j=0; j<mSectorSize/4; j++)
            ((unsigned long*)buf)[j] = ~sect->data[j];
        buf += mSectorSize;
    }

    return 0; // OK
}

int8_t FlashDrive::write(uint8_t *buf, uint32_t blk_addr, uint16_t count)
{
//    printf("MSC write to 0x%04X, size=%d\n", blk_addr, count);
      
//    writeTimeout = 50;
  
    for (int i=0; i<count; i++)
    {
        Sector *sect = findSector(blk_addr + i);
        if (!sect)
            return -1;
        
        for (int j=0; j<mSectorSize/4; j++)
        {
            unsigned long v = ~((unsigned long*)buf)[j];
            if ((sect->data[j] & v) != v)
            {
                invalidateSector(sect);
                sect = findSector(-1); // get new blank sector
                if (!sect)
                    return -1;
                break;
            }
        }
        
        Header hdr = sect->hdr;
        hdr.blank = 0;
        hdr.sectorNumber = blk_addr + i;
        
        Flash::unlock();
        Flash::programWord((unsigned long)sect, *((unsigned long*)&hdr));
        Flash::programDataInverted((unsigned long)sect->data, buf, mSectorSize);
        Flash::lock();
        
        buf += mSectorSize;
    }
    
    return 0; // OK
}

void FlashDrive::invalidateSector(Sector *sect)
{
    Header hdr = sect->hdr;
    hdr.valid = 0;
    Flash::unlock();
    Flash::programWord((unsigned long)sect, *((unsigned long*)&hdr));
    Flash::lock();
}