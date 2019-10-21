#ifndef _USBIF_H
#define _USBIF_H

#include "usbdesc.h"
#include "usbnode.h"
#include "usbdev.h"
#include "usbep.h"

namespace Usb
{

//class UsbCsInterface : public UsbNode
//{
//private:
////    ClassSpecific
//    Descriptor *mDescriptor;
//    
//public:
//    UsbCsInterface(UsbNode *parent, Descriptor *descriptor) :
//        UsbNode(parent)
//    {
//        mDescriptor = descriptor;
//        assignDescriptor(mDescriptor);
//    }
//};
//---------------------------------------------------------------------------
  
class UsbInterface : public UsbNode
{
private:
    InterfaceDescriptor *mDescriptor;
    string mInterfaceName;
    UsbNode *mCsInterface;
    unsigned char mAltSetting;
    
protected:
    void addChild(UsbNode *child);
    void nodeAttached();
    
public:
    UsbInterface(unsigned char classCode, unsigned char subclassCode, unsigned char protocolCode, string name="Interface");
    ~UsbInterface();
      
    string interfaceName() const {return mInterfaceName;}
    
    void setInterfaceNumber(unsigned char num) {mDescriptor->setInterfaceNumber(num);}
    unsigned char interfaceNumber() const {return mDescriptor->interfaceNumber();}
    
//    void addClassSpecificInterface(UsbCsInterface *iface) {csInterface = iface;}
    
    //void addEndpoint(UsbNode *endpoint);
};

};

#endif