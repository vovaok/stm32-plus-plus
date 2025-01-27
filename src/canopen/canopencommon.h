#ifndef CANOPENCOMMON_H
#define CANOPENCOMMON_H

#include <stdint.h>

namespace CanOpen
{
  
typedef struct
{
    uint8_t noe;
    uint32_t vendorID;
    uint32_t productCode;
    uint32_t revisionNumber;
    uint32_t serialNumber;
} IdentityObject;

typedef enum
{
    NMT_ModuleControl   = 0x000,
    EMERGENCY           = 0x080,
    TIMESTAMP           = 0x100,
    PDO1_TX             = 0x180,
    PDO1_RX             = 0x200,
    PDO2_TX             = 0x280,
    PDO2_RX             = 0x300,
    PDO3_TX             = 0x380,
    PDO3_RX             = 0x400,
    PDO4_TX             = 0x480,
    PDO4_RX             = 0x500,
    SDO_Response        = 0x580,
    SDO_Request         = 0x600,
    NMT_ErrorControl    = 0x700
} FunctionCode;

typedef enum
{
    // request:
    ccsSegmentDownload      = 0 << 5, // 0x00
    ccsInitiateDownload     = 1 << 5, // 0x20
    ccsInitiateUpload       = 2 << 5, // 0x40
    ccsSegmentUpload        = 3 << 5, // 0x60
    ccsAbortTransfer        = 4 << 5, // 0x80
    ccsBlockUpload          = 5 << 5, // 0xa0
    ccsBlockDownload        = 6 << 5, // 0xc0
    
    // response:
    scsSegmentUpload        = 0 << 5, // 0x00
    scsSegmentDownload      = 1 << 5, // 0x20
    scsInitiateUpload       = 2 << 5, // 0x40
    scsInitiateDownload     = 3 << 5, // 0x60
    
    
    cmdReadParam            = 0x40,
    cmdWriteParam           = 0x23,
    cmdReadParamResponse    = 0x43,
    cmdWriteParamResponse   = 0x60,
    cmdError                = 0x80
} Command;

//typedef enum
//{
//// request:
//    ccsSegmentDownload      = 0, // 0x00
//    ccsInitiateDownload     = 1, // 0x20
//    ccsInitiateUpload       = 2, // 0x40
//    ccsSegmentUpload        = 3, // 0x60
//    ccsAbortTransfer        = 4, // 0x80
//    ccsBlockUpload          = 5, // 0xa0
//    ccsBlockDownload        = 6, // 0xc0
//} CCS;
//    
//typedef enum
//{
//    // response:
//    scsSegmentUpload        = 0, // 0x00
//    scsSegmentDownload      = 1, // 0x20
//    scsInitiateUpload       = 2, // 0x40
//    scsInitiateDownload     = 3, // 0x60
//} SCS;

typedef enum
{
//    NoError             = 0x00000000,
//    SdoToggleBitError   = 0x05030000,
//    SdoCommTimeout      = 0x05040000,
    UnknownSdoCommand   = 0x05040001,
//    InvalidSdoBlockSize = 0x05040002,
//    InvalidSeqNumber    = 0x05040003,
//    CRCError            = 0x05040004,
//    OutOfMemory         = 0x05040005,
//    UnsupportedAccess   = 0x06010000,
//    ReadWO              = 0x06010001,
//    WriteRO             = 0x06010002,
    ObjectNotExist      = 0x06020000,
//    PdoObjectNotMapped  = 0x06040041,
//    PdoLengthExceed     = 0x06040042,
    HardwareError       = 0x06060000,
    SubindexNotExist    = 0x06090011,
//    ValueRangeExceed    = 0x06090030,
//    GeneralError        = 0x08000000,
//    DataNotTransferred  = 0x08000020,
//    DictionaryNotExist  = 0x08000023
} SDOAbortCode;

typedef enum
{
    Unknown     = 0,
    Boolean     = 1,
    Integer8    = 2,
    Integer16   = 3,
    Integer32   = 4,
    Unsigned8   = 5,
    Unsigned16  = 6,
    Unsigned32  = 7,
    Real32      = 8,
    VisibleString = 9,
    OctetString = 10,
    UnicodeString = 11,
    TimeOfDay   = 12,
    TimeDifference = 13,
    BitString   = 14,
    Domain      = 15
} Type;

enum NMTState
{
    Bootup = 0,
    NotConnected = 1,
    Connecting = 2,
    Idle = 3,
    Stopped = 4,
    Operational = 5,
    PreOperational = 0x7F,
};

enum NMTControl
{
    StartRemoteNode = 0x01,
    StopRemoteNode = 0x02,
    EnterPreOperationalState = 0x80,
    ResetNode = 0x81,
    ResetCommunication = 0x82,
};

#pragma pack(push,1)
typedef struct
{
    union
    {
        unsigned char cmd;
        struct
        {
            unsigned char s: 1;
            unsigned char e: 1;
            unsigned char n: 2;
            unsigned char : 1;
            unsigned char ccs: 3;
        };
    };
    unsigned short id;
    unsigned char subid;
    unsigned long value;
} SDO;

typedef struct
{
    struct // Segmented download/upload
    {
        uint8_t c: 1;   // end of transfer
        uint8_t n: 3;   // data length = 8 - n
        uint8_t t: 1;   // toggle bit
        uint8_t ccs: 3; // client/server command specifier
    };
    uint8_t data[7];
} SDO_seg;
#pragma pack(pop)

typedef struct
{
    uint16_t id;
    uint8_t subid;
    uint8_t *data;
    uint32_t size;
    uint32_t offset;
} Segment;

}

#endif // CANOPENCOMMON_H

