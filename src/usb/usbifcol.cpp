#include "usbifcol.h"
#include "usbcfg.h"

using namespace Usb;

UsbInterfaceCollection::UsbInterfaceCollection(unsigned char classCode, unsigned char subclassCode, unsigned char protocolCode, string name) :
    UsbNode(UsbNode::NodeTypeInterfaceCollection)
{
    mCollectionName = name;
    mDescriptor = new InterfaceCollectionDescriptor;
    mDescriptor->setFunctionClass(classCode);
    mDescriptor->setFunctionSubClass(subclassCode);
    mDescriptor->setFunctionProtocol(protocolCode);
    assignDescriptor(mDescriptor);
}

UsbInterfaceCollection::~UsbInterfaceCollection()
{

}
//---------------------------------------------------------------------------

void UsbInterfaceCollection::addChild(UsbNode *child)
{
    if (child->nodeType() == UsbNode::NodeTypeInterface)
    {
        UsbInterface* iface = static_cast<UsbInterface*>(child);
        if (iface && parent())
        {
            if (parent()->nodeType() == NodeTypeConfig)
            {
                UsbConfiguration *conf = static_cast<UsbConfiguration*>(parent());
                iface->setInterfaceNumber(conf->incrementNumInterfaces());
            }
        }
        mDescriptor->incrementNumInterfaces();
    }
    
    UsbNode::addChild(child);
}
//---------------------------------------------------------------------------

void UsbInterfaceCollection::nodeAttached()
{
    unsigned char strdesc = device()->addStringDescriptor(mCollectionName);
    mDescriptor->setStringIndex(strdesc);
}
//---------------------------------------------------------------------------

void UsbInterfaceCollection::setFirstInterfaceNumber(unsigned char idx)
{
    mDescriptor->setFirstInterface(idx);
}