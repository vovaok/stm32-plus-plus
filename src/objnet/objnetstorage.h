#ifndef _OBJNETSTORAGE_H
#define _OBJNETSTORAGE_H

#include "flash.h"
#include "objectinfo.h"

namespace Objnet
{
  
class ObjnetStorage;
  
extern "C" ObjnetStorage *objnetStorage();
  
class ObjnetStorage
{
private:
    static ObjnetStorage *mSelf;
    static Flash::Sector mSector1, mSector2;
    ObjnetStorage(Flash::Sector sector1, Flash::Sector sector2);
    
    friend ObjnetStorage *objnetStorage();
    
#pragma pack(push,1)
    typedef enum
    {
        Erased      = 0xFF,
        Transfer    = 0xF2,
        Active      = 0x42
    } MagicNumber;
    
    typedef struct
    {
        unsigned char magicNumber;
        unsigned char version;
        unsigned short useCount;        
        unsigned long reserve1;
        unsigned long reserve2;
        unsigned long reserve3;
    } SectorHeader;
    
    typedef enum
    {   
        Desc        = 0x01,
        Param       = 0x02,
        Blank       = 0x03,
        TypeMask    = 0x03,
        Corrupted   = 0x40,
        Valid       = 0x80
    } Flags;
    
    typedef struct
    {
        Flags flags;
        unsigned char size;
        unsigned short nextBlockOffset; // relative to header start
    } BlockHeader;
    
    typedef struct
    {
        unsigned short sid;// StorageID
        unsigned char type; // type of param as in ObjectInfo
        unsigned char size; // in bytes as in ObjectInfo
    } DescHeader;
    
    typedef struct
    {
        unsigned short sid;
        unsigned short _dummy;
    } ParamHeader;
#pragma pack(pop)

    SectorHeader *mSectorHeader1, *mSectorHeader2;
    unsigned long *mStorage;
    
    void transfer(Flash::Sector from, Flash::Sector to);
    
protected:
    friend class ObjectInfo;
    
    void save(ObjectInfo &info);
    void load(ObjectInfo &info);
    
public:
    static void setFlashSectors(Flash::Sector sector1, Flash::Sector sector2);
    
    
};

}

#endif