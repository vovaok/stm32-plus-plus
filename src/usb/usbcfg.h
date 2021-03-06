#ifndef _USBCFG_H
#define _USBCFG_H

#include "usbdesc.h"
#include "usbnode.h"
#include "usbif.h"
#include "usbifcol.h"
#include "core/core.h"

namespace Usb
{

class UsbConfiguration : public UsbNode
{
private:
    string mConfigurationName;
    ConfigurationDescriptor *mDescriptor;
    
protected:
    void addChild(UsbNode* node);
    void nodeAttached();
    void setup(const UsbSetupReq &req);
    friend class UsbDevice;
    
    void updateDescriptorLength(int deltaLength) {mDescriptor->addTotalLength(deltaLength); UsbNode::updateDescriptorLength(deltaLength);}
  
public:
    UsbConfiguration(PowerAttributes attributes=AttrSelfPowered, unsigned int powerConsumption_mA=100, string name="Configuration");
//    ~UsbConfiguration();
    
    const ConfigurationDescriptor *descriptor() const {return mDescriptor;}
      
    string configurationName() const {return mConfigurationName;}
    void setConfigurationValue(unsigned char value) {(mDescriptor)->setConfigurationValue(value);}
    
    unsigned char incrementNumInterfaces() {return mDescriptor->incrementNumInterfaces();}
};

};

#endif