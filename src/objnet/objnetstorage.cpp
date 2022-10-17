#include "objnetstorage.h"

using namespace Objnet;

ObjnetStorage* ObjnetStorage::mSelf = 0L;
#if defined(STM32F37X)
Flash::Sector ObjnetStorage::mSector1 = (Flash::Sector)126;
Flash::Sector ObjnetStorage::mSector2 = (Flash::Sector)127;
#else
Flash::Sector ObjnetStorage::mSector1 = Flash::Sector10;
Flash::Sector ObjnetStorage::mSector2 = Flash::Sector11;
#endif

void ObjnetStorage::setFlashSectors(Flash::Sector sector1, Flash::Sector sector2)
{
    mSector1 = sector1;
    mSector2 = sector2;
}

#ifdef __cplusplus
extern "C" {
#endif
ObjnetStorage *objnetStorage()
{
    if (!ObjnetStorage::mSelf)
    {
        new ObjnetStorage(ObjnetStorage::mSector1, ObjnetStorage::mSector2);
    }
    return ObjnetStorage::mSelf;
}
#ifdef __cplusplus
}
#endif

ObjnetStorage::ObjnetStorage(Flash::Sector sector1, Flash::Sector sector2) :
    mStorage(0L),
    mFirstBlock(0L),
    mNewBlock(0L),
    mLastDesc(0L)
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
    else if (mSectorHeader1->magicNumber != Transfer && mSectorHeader1->magicNumber != Erased) // trash
    {
        Flash::unlock();
        Flash::eraseSector(mSector1);
        Flash::lock();
        mSectorHeader1 = reinterpret_cast<SectorHeader*>(Flash::getBeginOfSector(mSector1));
    }
    
    if (mSectorHeader1->magicNumber == Transfer)
        transfer(mSector2, mSector1);
    else if (mSectorHeader2->magicNumber == Transfer)
        transfer(mSector1, mSector2);
    else if (mSectorHeader1->magicNumber == Erased)
    {
        format(mSector1);
        mStorage = reinterpret_cast<unsigned long*>(mSectorHeader1);
    }
    
    mFirstBlock = reinterpret_cast<BlockHeader*>(mStorage + sizeof(SectorHeader)/sizeof(unsigned long));
    for (mNewBlock = mFirstBlock; mNewBlock->flags != Blank; mNewBlock = mNewBlock->sibling());
#warning TODO: nado zapilit: if (mNewBlock_address > mStorageSize) { do transfer; }
    
    for (Description *d = reinterpret_cast<Description*>(mFirstBlock); d->isValid(); d = d->next())
        mLastDesc = d;
}
//---------------------------------------------------------------------------

ObjnetStorage::Description *ObjnetStorage::writeDesc(Description &desc, const char *name)
{
    int newaddr = reinterpret_cast<unsigned long>(mNewBlock);
    int headerSize = sizeof(BlockHeader) + sizeof(DescHeader);
    Flash::programData(newaddr, &desc, headerSize);
    Flash::programData(newaddr + headerSize, name, desc.bHdr.size * sizeof(unsigned long) - headerSize);
    BlockHeader blockHdr;
    if (mLastDesc)
    {
        blockHdr = mLastDesc->bHdr;
        blockHdr.nextBlock = mNewBlock - &mLastDesc->bHdr;
        Flash::programWord(reinterpret_cast<unsigned long>(mLastDesc), blockHdr);
    }
    blockHdr = *mNewBlock;
    blockHdr.corrupted = 0;
    Flash::programWord(newaddr, blockHdr);
    mLastDesc = reinterpret_cast<Description*>(mNewBlock);
    mNewBlock = mLastDesc->bHdr.sibling();
    return mLastDesc;
}

ObjnetStorage::Parameter *ObjnetStorage::writeParam(unsigned char oid, Parameter &param, const void *data)
{    
    Parameter *lastParam = mParamMap.count(oid)? mParamMap[oid]: 0L;
    param.bHdr.size = (sizeof(BlockHeader) + sizeof(ParamHeader) + param.pHdr.size + sizeof(unsigned long) - 1) / sizeof(unsigned long);
    int newaddr = reinterpret_cast<unsigned long>(mNewBlock);
    int headerSize = sizeof(BlockHeader) + sizeof(ParamHeader);
    Flash::programData(newaddr, &param, headerSize);
    Flash::programData(newaddr + headerSize, data, param.pHdr.size);
    BlockHeader blockHdr;
    if (lastParam)
    {
        blockHdr = lastParam->bHdr;
        blockHdr.nextBlock = mNewBlock - &lastParam->bHdr;
        Flash::programWord(reinterpret_cast<unsigned long>(lastParam), blockHdr);
    }
    blockHdr = *mNewBlock;
    blockHdr.corrupted = 0;
    Flash::programWord(newaddr, blockHdr);
    lastParam = reinterpret_cast<Parameter*>(mNewBlock);
    mNewBlock = lastParam->bHdr.sibling();
    mParamMap[oid] = lastParam;
    return lastParam;
}
//---------------------------------------------------------------------------

bool ObjnetStorage::find(ObjectInfo &info)
{    
    unsigned char oid = info.mDesc.id;
    if (!mParamMap.count(oid))
        mParamMap[oid] = 0L;
    else if (mParamMap[oid])
        return true;
    
    Parameter *&lastParam = mParamMap[oid];
    
    for (Description *desc = reinterpret_cast<Description*>(mFirstBlock); desc->isValid(); desc = desc->next())
    {
        if ((info.mDesc.wType == desc->dHdr.type) && (info.mDesc.writeSize == desc->dHdr.size)) // if size and type are matched
        {
            if (!strcmp(info.mDesc.name.c_str(), &desc->name)) // and name is matched
            {
//                mIdMap[oid] = desc->dHdr.sid; // assign StorageID to ObjectID

                // find last Param
                for (Parameter *par = reinterpret_cast<Parameter*>(desc->bHdr.sibling()); par; par = par->next())
                {
                    if (par->isValid())
                        lastParam = par;
                }
                
                return true;
            }
        }
    }
    
    return false;
}

void ObjnetStorage::init(ObjectInfo &info)
{
    unsigned short lastsid = mLastDesc? mLastDesc->dHdr.sid: 0; 
    
    Description desc;
    desc.bHdr.size = (sizeof(BlockHeader) + sizeof(DescHeader) + info.mDesc.name.length() + 1 + sizeof(unsigned long) - 1) / sizeof(unsigned long);
    desc.dHdr.sid = lastsid + 1;
    desc.dHdr.type = info.mDesc.wType;
    desc.dHdr.size = info.mDesc.writeSize;
    
    Parameter par;
    par.bHdr.valid = 0;
    par.pHdr.sid = desc.dHdr.sid;
    par.pHdr._dummy = 0xFF;
    par.pHdr.size = 0;
    
    Flash::unlock();
    writeDesc(desc, info.mDesc.name.c_str());
    writeParam(info.mDesc.id, par, 0L);
    Flash::lock();
}

void ObjnetStorage::save(ObjectInfo &info)
{
    if (info.mDesc.flags & (ObjectInfo::Function | ObjectInfo::Dual | ObjectInfo::Volatile))
        return;
    if (!(info.mDesc.flags & ObjectInfo::Save))
        return;
    
    if (!find(info))
        init(info);
    
    // do write to flash
    Parameter par;
    ByteArray ba = info.read();
    par.pHdr.sid = mParamMap[info.mDesc.id]->pHdr.sid;
    par.pHdr._dummy = 0xFF;
    par.pHdr.size = ba.size();
    
    Flash::unlock();
    writeParam(info.mDesc.id, par, ba.data());
    Flash::lock();
}
  
void ObjnetStorage::load(ObjectInfo &info)
{
    if (info.mDesc.flags & (ObjectInfo::Function | ObjectInfo::Dual | ObjectInfo::Volatile))
        return;
    if (!(info.mDesc.flags & ObjectInfo::Save))
        return;
    
    if (find(info))
    {
        // do read from flash
        Parameter *par = mParamMap[info.mDesc.id];
        ByteArray ba(&par->data, par->pHdr.size); 
        info.write(ba);
    }
}
//---------------------------------------------------------------------------

void ObjnetStorage::format(Flash::Sector sector)
{
    unsigned long address = Flash::getBeginOfSector(sector);
    
    SectorHeader hdr;
    hdr.magicNumber = Active;
    hdr.version = OBJNET_STORAGE_VERSION;
    hdr.useCount = useCount() + 1;
    hdr.reserve1 = 0xFFFFFFFF;
    hdr.reserve2 = 0xFFFFFFFF;
    hdr.reserve3 = 0xFFFFFFFF;
    
    Flash::unlock();
    Flash::programData(address, &hdr, sizeof(SectorHeader));
    Flash::lock();
}

void ObjnetStorage::transfer(Flash::Sector from, Flash::Sector to)
{
    
}
//---------------------------------------------------------------------------
