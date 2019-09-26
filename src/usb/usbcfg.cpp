#include "usbcfg.h"
#include "usbdev.h"

using namespace Usb;

UsbConfiguration::UsbConfiguration(PowerAttributes attributes, unsigned int powerConsumption_mA, string name) :
  UsbNode(UsbNode::NodeTypeConfig)
{
    mConfigurationName = name;
    mDescriptor = new ConfigurationDescriptor;
    mDescriptor->setTotalLength(0);
    mDescriptor->setAttributes(attributes);
    mDescriptor->setMaxPower(powerConsumption_mA<=500? powerConsumption_mA>>1: 250);
    assignDescriptor(mDescriptor);
}

//UsbConfiguration::~UsbConfiguration()
//{
//}

void UsbConfiguration::addChild(UsbNode* node)
{
//    UsbInterfaceCollection *collection = dynamic_cast<UsbInterfaceCollection*>(node);
//    if (collection)
//        doNothing();
  
    if (node->nodeType() == UsbNode::NodeTypeInterface)
    {
        UsbInterface* interface = dynamic_cast<UsbInterface*>(node);
        if (interface)
            interface->setInterfaceNumber(mDescriptor->incrementNumInterfaces());
    }
        
    UsbNode::addChild(node);
}

void UsbConfiguration::nodeAttached()
{
    unsigned char strdesc = device()->addStringDescriptor(mConfigurationName);
    mDescriptor->setStringIndex(strdesc);
}
//---------------------------------------------------------------------------

void UsbConfiguration::setup(const UsbSetupReq &req)
{
    if (req.wIndex < mChildren.size())
        mChildren[req.wIndex]->setup(req);
}
