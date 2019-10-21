#ifndef _USBDESC_H
#define _USBDESC_H

#include <string>
//#include "usbd_ioreq.h"
//#include "usbd_req.h"
#include "usbconst.h"
#include "core/bytearray.h"

namespace Usb
{
  
using namespace std;
  
typedef enum
{
    AttrNone            = 0,
    AttrBusPowered      = (1<<7),
    AttrSelfPowered     = (1<<6),
    AttrRemoteWakeup    = (1<<5)
} PowerAttributes;

typedef enum 
{
    TransferControl         = 0,
    TransferIsochronous     = 1,
    TransferBulk            = 2,
    TransferInterrupt       = 3,
    TransferNoSync          = (0 << 2),
    TransferAsync           = (1 << 2),
    TransferAdaptive        = (2 << 2),
    TransferSync            = (3 << 2),
    TransferData            = (0 << 4),
    TransferFeedback        = (1 << 4),
    TransferExplicitFeedback= (2 << 4)
} TransferType;

typedef enum
{
    EndpointOUT = 0x00,
    EndpointIN  = 0x80
} EndpointDirection;
//---------------------------------------------------------------------------

class Descriptor : private ByteArray
{
protected:
    using ByteArray::append;
  
public:
    Descriptor() {}
    Descriptor(int bLength) : ByteArray(bLength, 0) {}
    //Descriptor(Descriptor &d) {*this = d;}
    virtual ~Descriptor() {}
    using ByteArray::length;
    using ByteArray::data;
//    void readBytes(ByteArray &ba) {ba.append(*this);}
    const ByteArray &toByteArray() const {return *this;}
};

class StringDescriptor : public Descriptor
{
public:
    StringDescriptor(wstring str) 
    {
        int slen = str.length();
        append(slen*2 + 2);
        append(USB_DESC_TYPE_STRING);
        append(str.data(), slen*2);
    }
    
    StringDescriptor(string str)
    {
        int slen = str.length();
        append(slen*2 + 2);
        append(USB_DESC_TYPE_STRING);
        const char *strdata = str.data();
        for (int i=0; i<slen; i++)
        {
            append(*strdata++);
            append('\0');
        }
    }
};
  
class DeviceDescriptor : public Descriptor
{
private: 
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 18
        unsigned char bDescriptorType;  // descriptor type is device descriptor
        unsigned short bcdUSB;
        unsigned char bDeviceClass;     // 0x00 for complex device
        unsigned char bDeviceSubClass;
        unsigned char bDeviceProtocol;
        unsigned char bMaxPacketSize0;
        unsigned short idVendor;        // VID    
        unsigned short idProduct;       // PID
        unsigned short bcdDevice;       // rel. 2.00
        unsigned char iManufacturer;    // Index of manufacturer string
        unsigned char iProduct;         // Index of product string
        unsigned char iSerialNumber;    // Index of serial number string
        unsigned char bNumConfigurations;
    } Fields;
#pragma pack(pop)
    
//public:
    DeviceDescriptor() :
      Descriptor(18)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 18;        
        m->bDescriptorType = USB_DESC_TYPE_DEVICE;
        m->bcdUSB = 0x0110; // USB 1.1
        m->bMaxPacketSize0 = USB_OTG_MAX_EP0_SIZE;
        m->idVendor = USBD_VID;
        m->idProduct = USBD_PID;
        m->iManufacturer = 1;
        m->iProduct = 2;
        m->iSerialNumber = 3;
    }  
    
    void setUsbSpecification(unsigned short value) {reinterpret_cast<Fields*>(data())->bcdUSB = value;}
    void setDeviceClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bDeviceClass = value;}
    void setDeviceSubClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bDeviceSubClass = value;}
    void setDeviceProtocol(unsigned char value) {reinterpret_cast<Fields*>(data())->bDeviceProtocol = value;}
    unsigned char maxPacketSize0() const {return reinterpret_cast<const Fields*>(data())->bMaxPacketSize0;}
    void setMaxPacketSize0(unsigned char value) {reinterpret_cast<Fields*>(data())->bMaxPacketSize0 = value;}
    void setVendorId(unsigned short value) {reinterpret_cast<Fields*>(data())->idVendor = value;}
    void setProductId(unsigned short value) {reinterpret_cast<Fields*>(data())->idProduct = value;}
    void setDeviceRevisionNumber(unsigned short value) {reinterpret_cast<Fields*>(data())->bcdDevice = value;}
    unsigned char numConfigurations() const {return reinterpret_cast<const Fields*>(data())->bNumConfigurations;}
    void setNumConfigurations(unsigned char value) {reinterpret_cast<Fields*>(data())->bNumConfigurations = value;}
    
    friend class UsbDevice;
};

class DeviceQualifierDescriptor : public Descriptor
{
private: 
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 10
        unsigned char bDescriptorType;  // descriptor type is device qualifier descriptor
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;
        unsigned char byte4;
        unsigned char byte5;
        unsigned char byte6;
        unsigned char byte7;
        unsigned char byte8;
    } Fields;
#pragma pack(pop)
    
//public:
    DeviceQualifierDescriptor() :
      Descriptor(10)
//      byte1(0x00),
//      byte2(0x02),
//      byte3(0x00),
//      byte4(0x00),
//      byte5(0x00),
//      byte6(0x40),
//      byte7(0x01),
//      byte8(0x00)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 10;
        m->bDescriptorType = USB_DESC_TYPE_DEVICE_QUALIFIER;
        m->byte1 = 0x00;
        m->byte2 = 0x02;
        m->byte3 = 0x00;
        m->byte4 = 0x00;
        m->byte5 = 0x00;
        m->byte6 = 0x40;
        m->byte7 = 0x01;
        m->byte8 = 0x00;
    }  
    
    friend class UsbDevice;
};

class LangIdDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 4
        unsigned char bDescriptorType;  // descriptor type is string descriptor
        unsigned short bcdLangID;
    } Fields;
#pragma pack(pop)

    LangIdDescriptor(unsigned short langId) :
        Descriptor(4)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 4;
        m->bDescriptorType = USB_DESC_TYPE_STRING;
        m->bcdLangID = langId;
    }
    
    friend class UsbDevice;
};

class ConfigurationDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 9
        unsigned char bDescriptorType;  // descriptor type is configuration descriptor
        unsigned short wTotalLength;
        unsigned char bNumInterfaces;
        unsigned char bConfigurationValue;
        unsigned char iConfiguration;
        unsigned char bmAttributes;
        unsigned char bMaxPower;
    } Fields;
#pragma pack(pop)   
    
//public:
    ConfigurationDescriptor() :
      Descriptor(9)
//      bNumInterfaces(0),
//      bConfigurationValue(0),
//      iConfiguration(0),
//      bmAttributes(AttrNone),
//      bMaxPower(0)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 9;
        m->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
    }
        
    unsigned char totalLength() const {return reinterpret_cast<const Fields*>(data())->wTotalLength;}
    void setTotalLength(int value) {reinterpret_cast<Fields*>(data())->wTotalLength = value;}
    void addTotalLength(int delta) {reinterpret_cast<Fields*>(data())->wTotalLength += delta;}
    unsigned char numInterfaces() const {return reinterpret_cast<const Fields*>(data())->bNumInterfaces;}
    void setNumInterfaces(unsigned char value) {reinterpret_cast<Fields*>(data())->bNumInterfaces = value;}
    unsigned char incrementNumInterfaces() {return reinterpret_cast<Fields*>(data())->bNumInterfaces++;}
    unsigned char configurationValue() const {return reinterpret_cast<const Fields*>(data())->bConfigurationValue;}
    void setConfigurationValue(unsigned char value) {reinterpret_cast<Fields*>(data())->bConfigurationValue = value;}
    void setStringIndex(unsigned char value) {reinterpret_cast<Fields*>(data())->iConfiguration = value;}
    unsigned char attributes() const {return reinterpret_cast<const Fields*>(data())->bmAttributes;}
    void setAttributes(unsigned char value) {reinterpret_cast<Fields*>(data())->bmAttributes = value;}
    unsigned char maxPower() const {return reinterpret_cast<const Fields*>(data())->bMaxPower;}
    void setMaxPower(unsigned char value) {reinterpret_cast<Fields*>(data())->bMaxPower = value;}
    
    friend class UsbConfiguration;
    friend class UsbDevice;
};

class InterfaceCollectionDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 8
        unsigned char bDescriptorType;  // descriptor type is IAD descriptor
        unsigned char bFirstInterface;
        unsigned char bNumInterfaces;
        unsigned char bFunctionClass;
        unsigned char bFunctionSubClass;
        unsigned char bFunctionProtocol;
        unsigned char iFunction;
    } Fields;
#pragma pack(pop) 
    
//public:
    InterfaceCollectionDescriptor() :
      Descriptor(8)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 8;
        m->bDescriptorType = USB_DESC_TYPE_INTERFACE_COLLECTION;
    }
    
    void setFunctionClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bFunctionClass = value;}
    void setFunctionSubClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bFunctionSubClass = value;}
    void setFunctionProtocol(unsigned char value) {reinterpret_cast<Fields*>(data())->bFunctionProtocol = value;}
    void setStringIndex(unsigned char value) {reinterpret_cast<Fields*>(data())->iFunction = value;}
    void setFirstInterface(unsigned char value) {reinterpret_cast<Fields*>(data())->bFirstInterface = value;}
    void incrementNumInterfaces() {reinterpret_cast<Fields*>(data())->bNumInterfaces++;}
    
    friend class UsbInterfaceCollection;
};

class InterfaceDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 9
        unsigned char bDescriptorType;  // descriptor type is interface descriptor
        unsigned char bInterfaceNumber;
        unsigned char bAlternateSetting;
        unsigned char bNumEndpoints;
        unsigned char bInterfaceClass;
        unsigned char bInterfaceSubClass;
        unsigned char bInterfaceProtocol;
        unsigned char iInterface;
    } Fields;
#pragma pack(pop) 
    
//public:
    InterfaceDescriptor() :
      Descriptor(9)
//      bNumEndpoints(0),
//      bAlternateSetting(0)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 9;
        m->bDescriptorType = USB_DESC_TYPE_INTERFACE;
    }
    
    unsigned char interfaceNumber() const {return reinterpret_cast<const Fields*>(data())->bInterfaceNumber;}
    void setInterfaceNumber(unsigned char value) {reinterpret_cast<Fields*>(data())->bInterfaceNumber = value;}
    unsigned char alternateSetting() const {return reinterpret_cast<const Fields*>(data())->bAlternateSetting;}
    void setAlternateSetting(unsigned char value) {reinterpret_cast<Fields*>(data())->bAlternateSetting = value;}
    unsigned char numEndpoints() const {return reinterpret_cast<const Fields*>(data())->bNumEndpoints;}
    void setNumEndpoints(unsigned char value) {reinterpret_cast<Fields*>(data())->bNumEndpoints = value;}
    void incrementNumEndpoints() {reinterpret_cast<Fields*>(data())->bNumEndpoints++;}
    unsigned char interfaceClass() const {return reinterpret_cast<const Fields*>(data())->bInterfaceClass;}
    void setInterfaceClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bInterfaceClass = value;}
    unsigned char interfaceSubClass() const {return reinterpret_cast<const Fields*>(data())->bInterfaceSubClass;}
    void setInterfaceSubClass(unsigned char value) {reinterpret_cast<Fields*>(data())->bInterfaceSubClass = value;}
    unsigned char interfaceProtocol() const {return reinterpret_cast<const Fields*>(data())->bInterfaceProtocol;}
    void setInterfaceProtocol(unsigned char value) {reinterpret_cast<Fields*>(data())->bInterfaceProtocol = value;}
    void setStringIndex(unsigned char idx) {reinterpret_cast<Fields*>(data())->iInterface = idx;}
    
    friend class UsbInterface;
};

class EndpointDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 7
        unsigned char bDescriptorType;  // descriptor type is endpoint descriptor
        unsigned char bEndpointAddress;
        unsigned char bmAttributes;
        unsigned short wMaxPacketSize;
        unsigned char bInterval;
    } Fields;
#pragma pack(pop) 
    
//public:
    EndpointDescriptor() :
      Descriptor(7)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 7;
        m->bDescriptorType = USB_DESC_TYPE_ENDPOINT;
    }
    
    unsigned char endpointAddress() const {return reinterpret_cast<const Fields*>(data())->bEndpointAddress;}
    void setEndpointAddress(unsigned char value) {reinterpret_cast<Fields*>(data())->bEndpointAddress = value;}
    unsigned char attributes() const {return reinterpret_cast<const Fields*>(data())->bmAttributes;}
    void setAttributes(unsigned char value) {reinterpret_cast<Fields*>(data())->bmAttributes = value;}
    unsigned short maxPacketSize() const {return reinterpret_cast<const Fields*>(data())->wMaxPacketSize;}
    void setMaxPacketSize(unsigned short value) {reinterpret_cast<Fields*>(data())->wMaxPacketSize = value;}
    unsigned char interval() const {return reinterpret_cast<const Fields*>(data())->bInterval;}
    void setInterval(unsigned char value) {reinterpret_cast<Fields*>(data())->bInterval = value;}
    
    friend class UsbEndpoint;
};

//class ClassSpecificDescriptor : public Descriptor
//{
//protected:
//    unsigned char bDescriptorSubType;
//};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

} // namespace Usb
#endif