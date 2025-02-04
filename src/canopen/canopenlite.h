#ifndef _CANOPENLITE_H
#define _CANOPENLITE_H

#include "caninterface.h"

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
    SDO_Response    = 0x580,
    SDO_Request     = 0x600
} CobId;

//typedef enum
//{  
//    cmdReadParam            = 0x40,
//    cmdWriteParam           = 0x23,
//    cmdReadParamResponse    = 0x43,
//    cmdWriteParamResponse   = 0x60,
//    cmdError                = 0x80
//} Command;

typedef enum
{
// request:
    ccsSegmentDownload      = 0, // 0x00
    ccsInitiateDownload     = 1, // 0x20
    ccsInitiateUpload       = 2, // 0x40
    ccsSegmentUpload        = 3, // 0x60
    ccsAbortTransfer        = 4, // 0x80
    ccsBlockUpload          = 5, // 0xa0
    ccsBlockDownload        = 6, // 0xc0
} CCS;
    
typedef enum
{
    // response:
    scsSegmentUpload        = 0, // 0x00
    scsSegmentDownload      = 1, // 0x20
    scsInitiateUpload       = 2, // 0x40
    scsInitiateDownload     = 3, // 0x60
} SCS;

#pragma pack(push,1)
typedef struct
{
    struct // Initiate download/upload
    {
        uint8_t s: 1;   // size is specified
        uint8_t e: 1;   // expedited transfer
        uint8_t n: 2;   // data length = 4 - n
        uint8_t : 1;    // not used
        uint8_t ccs: 3; // client/server command specifier
    };
    uint16_t id;
    uint8_t subid;
    union
    {
        uint32_t value;
        uint8_t data[4];
    };
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

typedef bool (*ReadSDO_t)(uint16_t id, uint8_t subid, uint8_t **data, uint32_t *size, uint32_t offset);
typedef bool (*WriteSDO_t)(uint16_t id, uint8_t subid, const uint8_t *data, uint32_t size, uint32_t offset);
typedef void (*WriteSDOEnd_t)(uint16_t id, uint8_t subid, uint32_t offset);

class CanOpenLite
{
public:
    CanOpenLite(CanInterface *can, uint8_t address);
    
    ReadSDO_t readSDO;
    WriteSDO_t writeSDO;
    WriteSDOEnd_t writeSDOEnd;
    
    void loop();
    
private:
    CanInterface *m_can;
    uint8_t m_address;
    Segment m_seg;
};

#endif