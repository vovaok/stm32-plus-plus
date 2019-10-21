#ifndef _USBIFCOL_H
#define _USBIFCOL_H

#include "usbdesc.h"
#include "usbnode.h"
#include "usbdev.h"

namespace Usb
{
  
class UsbInterfaceCollection : public UsbNode
{
private:
    InterfaceCollectionDescriptor *mDescriptor;
    string mCollectionName;
    
protected:
    void addChild(UsbNode *child);
    void nodeAttached();
    
public:
    UsbInterfaceCollection(unsigned char classCode, unsigned char subclassCode, unsigned char protocolCode, string name="Interface");
    ~UsbInterfaceCollection();
      
    string name() const {return mCollectionName;}
    
    void setFirstInterfaceNumber(unsigned char idx);
};

};

#endif