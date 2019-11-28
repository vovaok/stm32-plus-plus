#ifndef _USBCDC_H
#define _USBCDC_H

#include "../usbdesc.h"
#include "../usbep.h"
#include "../usbif.h"
#include "../usbcfg.h"

#define CDC_DESCRIPTOR_TYPE         0x21
#define CDC_CS_INTERFACE_TYPE       0x24

namespace Usb
{
  
typedef enum
{
    CdcCtrlSubclassACM          = 0x02
} CdcControlSubclass;

typedef enum
{
    CdcCtrlProtocolCommonAT     = 0x01
} CdcControlProtocol;
  
typedef enum
{
    cdcSendEncapsulatedCommand  = 0x00,
    cdcGetEncapsulatedResponse  = 0x01,
    cdcSetCommFeature           = 0x02,
    cdcGetCommFeature           = 0x03,
    cdcClearCommFeature         = 0x04,
    cdcSetLineCoding            = 0x20,
    cdcGetLineCoding            = 0x21,
    cdcSetControlLineState      = 0x22,
    cdcSendBreak                = 0x23,
    cdcNoCmd                    = 0xFF
} CdcRequest;

typedef enum
{
    CdcDescHeader       = 0x00,
    CdcDescCM           = 0x01,
    CdcDescACM          = 0x02,
    CdcDescUnion        = 0x06
} CdcDescriptorSubtype;
//---------------------------------------------------------------------------

class CdcHeaderFuncDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;              // the length of the descriptor is equal to 5
        unsigned char bDescriptorType;      // descriptor type is CS_INTERFACE
        unsigned char bDescriptorSubtype;
        unsigned short bcdCDC;
    } Fields;
#pragma pack(pop) 
    
public:
    CdcHeaderFuncDescriptor() :
      Descriptor(5)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 5;
        m->bDescriptorType = CDC_CS_INTERFACE_TYPE;
        m->bDescriptorSubtype = CdcDescHeader;
        m->bcdCDC = 0x0110;
    }
    ~CdcHeaderFuncDescriptor()
    { 
    }
};  
  
class CdcCMFuncDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;              // the length of the descriptor is equal to 5
        unsigned char bDescriptorType;      // descriptor type is CS_INTERFACE
        unsigned char bDescriptorSubtype;
        unsigned char bmCapabilities;
        unsigned char bDataInterface;
    } Fields;
#pragma pack(pop) 
    
public:
    CdcCMFuncDescriptor(unsigned char ifnum) :
      Descriptor(5)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 5;
        m->bDescriptorType = CDC_CS_INTERFACE_TYPE;
        m->bDescriptorSubtype = CdcDescCM;
        m->bmCapabilities = 0x00;
        m->bDataInterface = ifnum + 1;
    }
    ~CdcCMFuncDescriptor()
    { 
    }
};  

class CdcACMFuncDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;              // the length of the descriptor is equal to 4
        unsigned char bDescriptorType;      // descriptor type is CS_INTERFACE
        unsigned char bDescriptorSubtype;
        unsigned char bmCapabilities;
    } Fields;
#pragma pack(pop) 
    
public:
    CdcACMFuncDescriptor() :
      Descriptor(4)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 4;
        m->bDescriptorType = CDC_CS_INTERFACE_TYPE;
        m->bDescriptorSubtype = CdcDescACM;
        m->bmCapabilities = 0x02;
    }
    ~CdcACMFuncDescriptor()
    { 
    }
};  

class CdcUnionFuncDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;              // the length of the descriptor is equal to 5
        unsigned char bDescriptorType;      // descriptor type is CS_INTERFACE
        unsigned char bDescriptorSubtype;
        unsigned char bControlInterface;
        unsigned char bDataInterface;
    } Fields;
#pragma pack(pop) 
    
public:
    CdcUnionFuncDescriptor(unsigned char ifnum) :
      Descriptor(5)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 5;
        m->bDescriptorType = CDC_CS_INTERFACE_TYPE;
        m->bDescriptorSubtype = CdcDescUnion;
        m->bControlInterface = ifnum;
        m->bDataInterface = ifnum + 1;
    }
    ~CdcUnionFuncDescriptor()
    { 
    }
};
//---------------------------------------------------------------------------

class CdcHeaderFuncNode : public UsbNode
{
private:
    CdcHeaderFuncDescriptor *mDescriptor;
    
public:
    CdcHeaderFuncNode() : UsbNode(NodeTypeFunc)
    {
        mDescriptor = new CdcHeaderFuncDescriptor();
        assignDescriptor(mDescriptor);
    }    
};

class CdcCMFuncNode : public UsbNode
{
private:
    CdcCMFuncDescriptor *mDescriptor;
    
public:
    CdcCMFuncNode(unsigned char ifnum) : UsbNode(NodeTypeFunc)
    {
        mDescriptor = new CdcCMFuncDescriptor(ifnum);
        assignDescriptor(mDescriptor);
    }    
};

class CdcACMFuncNode : public UsbNode
{
private:
    CdcACMFuncDescriptor *mDescriptor;
    
public:
    CdcACMFuncNode() : UsbNode(NodeTypeFunc)
    {
        mDescriptor = new CdcACMFuncDescriptor();
        assignDescriptor(mDescriptor);
    }    
};

class CdcUnionFuncNode : public UsbNode
{
private:
    CdcUnionFuncDescriptor *mDescriptor;
    
public:
    CdcUnionFuncNode(unsigned char ifnum) : UsbNode(NodeTypeFunc)
    {
        mDescriptor = new CdcUnionFuncDescriptor(ifnum);
        assignDescriptor(mDescriptor);
    }    
};
//---------------------------------------------------------------------------

class UsbVcpCtrlInterface : public UsbInterface
{
private:
#pragma pack(push,1)
    typedef enum {StopBits1=0, StopBits15=1, StopBits2=2} StopBits;
    typedef enum {ParityNo=0, ParityEven=1, ParityOdd=2} ParityType;
    typedef struct
    {
        unsigned long   baudrate;
        StopBits        stopbits;
        ParityType      parity;
        unsigned char   wordlength;
    } LineCoding;
#pragma pack(pop)
  
    UsbEndpoint *mCtrlEp;
    UsbSetupReq mReqBuffer;
    ByteArray mCtlBuffer;
    unsigned char cdcCmd; 
    
    LineCoding mLineCoding;
    bool mLineState;
    
    unsigned char mAltSet;
  
protected:
//    void addChild(UsbNode *child);
    void setup(const UsbSetupReq &req);
    void ep0RxReady();
//    virtual void sof();
    
public:
    UsbVcpCtrlInterface(UsbNode *parent);
   
    bool isOpened() {return mLineState;}
};

class UsbVcpDataInterface : public UsbInterface
{
private:
    UsbEndpoint *mInEp, *mOutEp;
//    UsbSetupReq mReqBuffer;
//    ByteArray mCtlBuffer;
    
//    unsigned char mProtocol;
//    unsigned char mIdleState;
//    unsigned char mAltSet;
  
protected:
    //void addChild(UsbNode *child);
    //void setup(const UsbSetupReq &req);
    //void ep0RxReady();
    //virtual void sof();
    
    virtual void receiveData(const ByteArray &ba);
    
public:
    UsbVcpDataInterface(UsbNode *parent);
   
    void sendData(const ByteArray &ba);
    void setReceiveEvent(const ConstDataEvent &e);
};
//---------------------------------------------------------------------------
  
}

#endif