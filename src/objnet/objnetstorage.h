#ifndef _OBJNETSTORAGE_H
#define _OBJNETSTORAGE_H

#include "flash.h"
#include "objectinfo.h"
#include <map>

#define OBJNET_STORAGE_VERSION  0x10 // 1.0

namespace Objnet
{
  
class ObjnetStorage;

//! @warning функция отказывается компилироваться в GCC как френд функция
//! видимо из за экстерна
//! Так как она нигде не используется, закомментил
//! Теперь используется, ГЦЦ должен понять и простить
extern "C" ObjnetStorage *objnetStorage();
  
class ObjnetStorage
{
    friend ObjnetStorage *objnetStorage();
private:
    static ObjnetStorage *mSelf;
    static Flash::Sector mSector1, mSector2;
    ObjnetStorage(Flash::Sector sector1, Flash::Sector sector2);
    
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
        TypeMask    = 0x03,
        Corrupted   = 0x40,
        Valid       = 0x80,
        Blank       = 0xFF
    } Flags;
    
    struct BlockHeader
    {
        struct
        {
            union
            {
                Flags flags;
                struct
                {
                    unsigned char type: 2;
                    unsigned char : 4;
                    unsigned char corrupted: 1;
                    unsigned char valid: 1;
                };
            };
            unsigned char size; // size of this block in words
            unsigned short nextBlock; // relative to this header
        };
        
        BlockHeader() : flags(Blank), size(0xFF), nextBlock(0xFFFF) {}
            
        bool isValid() const
        {
            return this && valid && !corrupted;
        }
        BlockHeader *next()
        {
            if (nextBlock == 0xFFFF)
                return 0L;
            return this + nextBlock;
        }
        BlockHeader *sibling()
        {
            if (size == 0xFF)
                return 0L;
            return this + size;
        }
        
        operator unsigned long() const {return *reinterpret_cast<const unsigned long*>(this);}
    };
    
    struct DescHeader
    {
        unsigned short sid; // StorageID
        unsigned char type; // type of param as in ObjectInfo
        unsigned char size; // in bytes as in ObjectInfo
    };
    
    struct ParamHeader
    {
        unsigned short sid;
        unsigned char _dummy;
        unsigned char size; // actual size in bytes
    };
#pragma pack(pop)
    
#pragma pack(push,4)
    struct Description
    {
        BlockHeader bHdr;
        DescHeader dHdr;
        char name;
        
        Description()
        {
            bHdr.flags = static_cast<Flags>(~Param);
        }
        bool isValid()
        {
            return bHdr.isValid() && ((bHdr.flags & TypeMask) == Desc);
        }
        Description *next()
        {   
            return reinterpret_cast<Description*>(bHdr.next());
        }   
    };
    
    struct Parameter
    {
        BlockHeader bHdr;
        ParamHeader pHdr;
        unsigned char data;
        
        Parameter()
        {
            bHdr.flags = static_cast<Flags>(~Desc);
        }
        bool isValid() const
        {
            return bHdr.isValid() && ((bHdr.flags & TypeMask) == Param);
        }
        Parameter *next()
        {   
            return reinterpret_cast<Parameter*>(bHdr.next());
        }
    };
#pragma pack(pop)

    SectorHeader *mSectorHeader1, *mSectorHeader2;
    unsigned long *mStorage;
    BlockHeader *mFirstBlock;
    BlockHeader *mNewBlock;
    Description *mLastDesc;
    
//    std::vector<unsigned char> mIdMap; // map ObjectID -> StorageID
    std::map<unsigned char, Parameter*> mParamMap; // map ObjectID -> (last written) Parameter ptr
    
    void format(Flash::Sector sector);
    void transfer(Flash::Sector from, Flash::Sector to);
    
    Description *writeDesc(Description &desc, const char *name);
    Parameter *writeParam(unsigned char oid, Parameter &param, const void *data);
    
    bool find(ObjectInfo &info);
    void init(ObjectInfo &info);
    
protected:
    friend class ObjectInfo;
    
public:
    static void setFlashSectors(Flash::Sector sector1, Flash::Sector sector2);
    
    void save(ObjectInfo &info);
    void load(ObjectInfo &info);
    
    unsigned char version() const {return OBJNET_STORAGE_VERSION;}
    unsigned short useCount() const {return mStorage? reinterpret_cast<SectorHeader*>(mStorage)->useCount: 0;}
};

}

#endif
