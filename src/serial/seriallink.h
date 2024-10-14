#ifndef _SERIALLINK_H
#define _SERIALLINK_H

#include <vector>
#include <string>
#include "core/device.h"

#define USE_EEPROM

#if defined(USE_EEPROM)
#include "fakeeeprom.h"
#endif

using namespace std;

#define UARTLINK_PROTOCOL_VERSION   0x0102  // v1.1

class SerialLink
{
public:
    explicit SerialLink(Device *device);

    typedef enum
    {
        pfVolatile  = 0x01,
        pfRead      = 0x02,
        pfWrite     = 0x04,
        pfPermanent = 0x08,
        pfIo        = 0x10,
        pfReadWrite = pfVolatile | pfRead | pfWrite,
        pfReadOnly  = pfVolatile | pfRead,
        pfWriteOnly = pfVolatile | pfWrite,
        pfConstant  = pfRead,
        pfStorage   = pfRead | pfWrite | pfPermanent,
    } ParamFlags;

    void setAddress(uint8_t addr);
    uint8_t address() const {return mAddress;}
    void setProductInfo(unsigned long productId, string productName, unsigned short productVersion, string productSerial);
    void registerParam(string name, void *ptr, size_t size, ParamFlags flags=pfReadWrite);
    void registerFunc(string name, DataEvent func);
    void sendParam(string name);
    void saveParam(string name);

    template <typename T>
    void registerParam(string name, T &ref, ParamFlags flags=pfReadWrite)
    {
        registerParam(name, &ref, sizeof(T), flags);
    }

    void storeParams();
    void restoreParams();
    bool enableSaving = true;

private: // typedefs
#pragma pack(push,1)
    typedef enum
    {
        // service commands: 0x0...0x7
        cmdEcho         = 0x0,
        cmdInfo         = 0x1,
    //    cmdRetrieveCaps = 0x4,
        cmdUartLinkVersion = 0x7,

        // parameter commands: 0x8...0xB
        cmdGetParamInfo = 0x8,
        cmdGetParam     = 0x9,
        cmdSetParam     = 0xA,
        //cmdStoreParam   = 0xB,

        // remote function call commands: 0xC...0xF
        cmdGetFuncInfo  = 0xC,
        cmdCallFunc     = 0xD
    } Command;

    typedef enum
    {
        icmdProductId       = 0x01,
        icmdProductName     = 0x02,
        icmdProductVersion  = 0x03,
        icmdProductSerial   = 0x04,
        icmdCompilationDate = 0x05,
        icmdCompilationTime = 0x06,
        icmdResources       = 0x07
    } InfoCmd;

    typedef struct
    {
        unsigned char cmd: 4; // Command
        unsigned char addr: 4;

        unsigned char frag_no: 4; // current fragment number
        unsigned char response: 1;
        unsigned char error: 1;
        unsigned char fragmented: 1;
        unsigned char eof: 1; // end of fragments
    } Header;

private:
    typedef struct
    {
        char size;
        ParamFlags flags;
        unsigned short virtAddr; // ex ptr;
        char name[8];
    } SUartParam;

    // remote function call code declarations:

//    typedef struct
//    {
//        unsigned char params_size: 4;
//        unsigned char return_size: 4;
//        unsigned char params_ptr:  1;
//        unsigned char return_ptr:  1;
//    } SUartFuncSizes;

    typedef struct
    {
        unsigned short dummy1; // ex ptr;
        unsigned short dummy2; // ex sizes;
        char name[8];
    } SUartFunc;

//    typedef struct
//    {
//        unsigned char idx;
//        unsigned char params[UART_DATA_BUF_SIZE-1];
//    } SUartFuncCmd;
#pragma pack(pop)

private:
    Device *m_device;
    vector<SUartParam> mParamVector;
    vector<SUartFunc> mFuncVector;
    vector<unsigned long> mParamPtrVector; // corresponds to parameter vector. protocol must be upgraded. a To roBHo KaKoe-To.
    vector<DataEvent> mEventVector; // corresponds to function vector. protocol must be upgraded. a To roBHo KaKoe-To.
    vector<bool> mParamChanged;

    unsigned short mUartLinkVersion;
    unsigned long mProductId;
    string mProductName;
    unsigned short mProductVersion;
    string mProductSerial;
    string mCompilationDate;
    string mCompilationTime;
    bool mEepromInitialized;
    unsigned char mAddress;

    void onDataReceived();
    void saveParam(unsigned char idx);
};

#endif