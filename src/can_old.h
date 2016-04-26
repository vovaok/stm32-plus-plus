#ifndef _CANOLD_H
#define _CANOLD_H

#include "can.h"

#define OBJNET_PROTOCOL_VERSION     1
#define OBJ_OUT_BUFFER_SIZE         64
#define OBJNET_ID_FULL              1

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

typedef struct
{
    ObjId id;
    void *data;
    unsigned char size;    
} ObjMessage;
//---------------------------------------------------------------------------

class OldCan : protected Can
{
private:
    unsigned char canAddr;
    CanTxMsg TxMessage;
    
public:
    OldCan(int canNumber, int baudrate, Gpio::Config pinRx=Gpio::NoConfig, Gpio::Config pinTx=Gpio::NoConfig);
//    
//    unsigned long createInId(int group, int lid) const {
//        return (((unsigned long)canAddr<<12) | ((group&0xF)<<8) | (lid&0xFF));}
    
    void setParameter(unsigned char address, unsigned char group, unsigned char cmd, void *data, unsigned char size);
    bool getResponse(ObjMessage &msg);
    
//    void flush() {Can::flush();}
};

#endif
