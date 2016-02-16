#ifndef _OBJNETOLD_H
#define _OBJNETOLD_H

#include "objnet/caninterface.h"

using namespace Objnet;

#pragma pack(push,1)
typedef struct
{
    union
    {
        struct
        {
            unsigned char cmd;
            unsigned char group: 4;
            unsigned char frag_no: 3;
            unsigned char frag_next: 1;
            unsigned char address;
            unsigned char response: 1;
            unsigned char version: 2;
            unsigned char full: 1;
            unsigned char not_global: 1; // 0 for global message, since it has higher priority
        };
        unsigned long dword;
        unsigned char bytes[4];
    };
} ObjId;
#pragma pack(pop)

typedef struct
{
    ObjId id;
    ByteArray ba;    
} ObjMessage;
//---------------------------------------------------------

typedef Closure<void(ObjMessage&)> ObjnetOldMessageEvent;
//---------------------------------------------------------

class ObjnetOld
{
private:
    typedef enum
    {
        cmdPing = 0,
        cmdProductId,
        cmdProductClass,
        cmdFirmVersion,
        cmdBurnCount,
        cmdObjCount0_7,
        cmdObjCount8_F,
        cmdNetAddress,
        cmdBoardName,
        cmdObjStore,
        cmdObjRestore,
        cmdStorageWriteCount,
        cmdObjInfo,
        cmdReset,
        cmdCompilationDate0,
        cmdCompilationDate1,
        cmdCompilationTime,
        cmdBoardFullName0,
        cmdBoardFullName1,
        cmdBoardFullName2,
        cmdBoardFullName3,
        cmdStorageObjCount,
        cmdStorageObjSize,
        cmdCpuName,
        cmdCpuFreq
    } Command;
  
    CanInterface *mCan;
    ObjnetOldMessageEvent mMessageEvent;
    
    void task();
    
    void sendSvc(unsigned char address, unsigned char cmd, const ByteArray &ba = ByteArray());
    
public:
    ObjnetOld(CanInterface *canInterface);
    
    void sendLocal(unsigned char address, unsigned char cmd, const ByteArray &ba = ByteArray());
    void setReceiveEvent(ObjnetOldMessageEvent event) {mMessageEvent = event;}
    
    void ping(unsigned char address) {sendSvc(address, cmdPing);}
};


#endif