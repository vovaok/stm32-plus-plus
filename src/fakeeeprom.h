#ifndef _FAKEEEPROM_H
#define _FAKEEEPROM_H

#include "flash.h"
#include <vector>

class FakeEeprom
{
public:
    typedef enum
    {
        sInvalid = 0x7E,
        sActive = 0x7F,
        sBlank = 0xFF
    } State;

private:
    union Header
    {
        unsigned long word;
        struct
        {
            unsigned char state;
            unsigned char reserve;
            unsigned short count;
        };
    };

private:
    static FakeEeprom *mSelf;
    FakeEeprom();
    const Flash::Sector mSectors[2] = {Flash::Sector2, Flash::Sector3};
    int mCurSector;
    uint32_t pageBase(int sector) const;
    uint32_t pageSize(int sector) const;

    int mUseCount;
    unsigned long mNextOffset;
    union
    {
        uint16_t mSectorsState;
        unsigned char mSectorState[2];
    };

    void init();
    Header readHeader(unsigned char sector);
    void writeHeader(unsigned char sector, Header h);
    void format(unsigned char sector);
    void validate(unsigned char sector);
    void invalidate(unsigned char sector);
    void transfer(unsigned char sector);

public:
    static FakeEeprom *instance();

    void write(unsigned short addr, unsigned short data);
    bool read(unsigned short addr, unsigned short &data);
    void eraseAll();

    int useCount() const {return mUseCount;}
    unsigned long sectorsState() const {return mSectorsState;}
    int curSectorUsage() const;

    void unlock() const {Flash::unlock();}
    void lock() const {Flash::lock();}
};

FakeEeprom *eeprom();

#endif