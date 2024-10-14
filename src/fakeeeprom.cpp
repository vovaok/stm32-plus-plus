#include "fakeeeprom.h"
#include <map>
#include <intrinsics.h>

FakeEeprom *FakeEeprom::mSelf = nullptr;

FakeEeprom::FakeEeprom() :
    mCurSector(-1),
    mUseCount(0),
    mNextOffset(4)
{
    mSelf = this;
    init();
}

FakeEeprom *FakeEeprom::instance()
{
    if (!mSelf)
        new FakeEeprom();
    return mSelf;
}

FakeEeprom *eeprom()
{
    return FakeEeprom::instance();
}
//---------------------------------------------------------------------------

uint32_t FakeEeprom::pageBase(int sector) const
{
    if (sector < 0)
        return 0;
    return Flash::getBeginOfSector(mSectors[sector]);
}

uint32_t FakeEeprom::pageSize(int sector) const
{
    if (sector < 0)
        return 0;
    return Flash::getSizeOfSector(mSectors[sector]);
}

void FakeEeprom::init()
{
    for (int i=0; i<2; i++)
    {
        Header h = readHeader(i);
        if (h.state == sActive)
        {
            mCurSector = i;
            break;
        }
    }

    if (mCurSector == -1)
    {
        format(0);
        validate(0);
        mCurSector = 0;
    }

    mUseCount = readHeader(mCurSector).count;

    bool pageIsFull = true;
    int sz = pageSize(mCurSector);
    int base = pageBase(mCurSector);
    for (int i=4; i<sz; i+=4)
    {
        unsigned long address = base + i;
        unsigned long data = Flash::readWord(address);
        if (data == 0xFFFFFFFF)
        {
            mNextOffset = i;
            pageIsFull = false;
            break;
        }
    }

    if (pageIsFull)
        transfer(mCurSector);

    for (unsigned char i=0; i<2; i++)
    {
        Header h = readHeader(i);
        mSectorState[i] = h.state;
    }
}

FakeEeprom::Header FakeEeprom::readHeader(unsigned char sector)
{
    Header h;
    h.word = Flash::readWord(pageBase(sector));
    return h;
}

void FakeEeprom::writeHeader(unsigned char sector, Header h)
{
    __disable_interrupt();
    unlock();
    Flash::programWord(pageBase(sector), h.word);
    lock();
    __enable_interrupt();
    mSectorState[sector] = h.state;
}

void FakeEeprom::format(unsigned char sector)
{
    Header h = readHeader(sector);
    if (h.state != sBlank)
    {
        __disable_interrupt();
        Flash::eraseSector(mSectors[mCurSector]);
        __enable_interrupt();
    }
    h.state = sBlank;
    h.reserve = 0xFF;
    h.count = mUseCount + 1;
    writeHeader(sector, h);
    mNextOffset = 4;
}

void FakeEeprom::validate(unsigned char sector)
{
    Header h = readHeader(sector);
    h.state = sActive;
    writeHeader(sector, h);
}

void FakeEeprom::invalidate(unsigned char sector)
{
    Header h = readHeader(sector);
    h.state = sInvalid;
    writeHeader(sector, h);
}

void FakeEeprom::transfer(unsigned char sector)
{
    int newSector = (sector + 1) & 1;
    format(newSector);
    mUseCount = readHeader(newSector).count;
    mCurSector = newSector;

    typedef std::map<unsigned short, unsigned short> smap_t;
    smap_t datamap;
    unsigned long base = pageBase(sector);
    int sz = pageSize(sector);
    for (int i=4; i<sz; i+=4)
    {
        unsigned long address = base + i;
        unsigned long word = Flash::readWord(address);
        datamap[word & 0xFFFF] = word >> 16;
    }

    unlock();
    for (smap_t::iterator it = datamap.begin(); it != datamap.end(); it++)
    {
        unsigned short addr = it->first;
        unsigned short data = it->second;
        write(addr, data);
    }
    lock();

    validate(newSector);
    invalidate(sector);
}
//---------------------------------------------------------------------------

void FakeEeprom::write(unsigned short addr, unsigned short data)
{
    unsigned long base = pageBase(mCurSector);
    int sz = pageSize(mCurSector);
    for (int i=mNextOffset; i<sz; i+=4)
    {
        unsigned long address = base + i;
        unsigned long word = Flash::readWord(address);
        if (word == 0xFFFFFFFF)
        {
            word = (data << 16) | addr;
            __disable_interrupt();
            Flash::programWord(address, word);
            __enable_interrupt();
            mNextOffset += 4;
            return;
        }
    }
    transfer(mCurSector);
}

bool FakeEeprom::read(unsigned short addr, unsigned short &data)
{
    unsigned long base = pageBase(mCurSector);
    for (int i=mNextOffset-4; i>=4; i-=4)
    {
        unsigned long word = Flash::readWord(base + i);
        if ((word & 0xFFFF) == addr)
        {
            data = word >> 16;
            return true;
        }
    }
    return false;
}

void FakeEeprom::eraseAll()
{
    __disable_interrupt();
    unlock();
    Flash::eraseSector(mSectors[0]);
    Flash::eraseSector(mSectors[1]);
    lock();
    __enable_interrupt();
    mCurSector = -1;
    mNextOffset = 4;
    init();
}
//---------------------------------------------------------------------------

int FakeEeprom::curSectorUsage() const
{
    return mNextOffset * 100 / pageSize(mCurSector);
}
//---------------------------------------------------------------------------
