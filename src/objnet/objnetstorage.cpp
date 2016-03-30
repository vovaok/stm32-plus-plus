#include "objnetstorage.h"

using namespace Objnet;

ObjnetStorage* ObjnetStorage::mSelf = 0L;
Flash::Sector ObjnetStorage::mSector1 = Flash::Sector10;
Flash::Sector ObjnetStorage::mSector2 = Flash::Sector11;

void ObjnetStorage::setFlashSectors(Flash::Sector sector1, Flash::Sector sector2)
{
    mSector1 = sector1;
    mSector2 = sector2;
}

extern "C" ObjnetStorage *objnetStorage()
{
    if (!ObjnetStorage::mSelf)
    {
        new ObjnetStorage(ObjnetStorage::mSector1, ObjnetStorage::mSector2);
    }
    return ObjnetStorage::mSelf;
}

ObjnetStorage::ObjnetStorage(Flash::Sector sector1, Flash::Sector sector2)
{
    mSelf = this;
    mSector1 = sector1;
    mSector2 = sector2;
    
    mSectorHeader1 = reinterpret_cast<SectorHeader*>(Flash::getBeginOfSector(mSector1));
    mSectorHeader2 = reinterpret_cast<SectorHeader*>(Flash::getBeginOfSector(mSector2));
    
    if (mSectorHeader1->magicNumber == Active)
        mStorage = reinterpret_cast<unsigned long*>(mSectorHeader1);
    else if (mSectorHeader2->magicNumber == Active)
        mStorage = reinterpret_cast<unsigned long*>(mSectorHeader2);
    
    if (mSectorHeader1->magicNumber == Transfer)
        transfer(mSector2, mSector1);
    else if (mSectorHeader2->magicNumber == Transfer)
        transfer(mSector1, mSector2);
    
    else if (mSectorHeader1->magicNumber == Erased)
    {
        format(mSector1);
    }
}
//---------------------------------------------------------------------------

void ObjnetStorage::save(ObjectInfo &info)
{
  
}
  
void ObjnetStorage::load(ObjectInfo &info)
{
    
}
//---------------------------------------------------------------------------

void ObjnetStorage::transfer(Flash::Sector from, Flash::Sector to)
{
  
}