#include "usbifcol.h"

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
    // bla bla bla
    UsbNode::addChild(child);
}
//---------------------------------------------------------------------------

void UsbInterfaceCollection::nodeAttached()
{
    unsigned char strdesc = device()->addStringDescriptor(mCollectionName);
    mDescriptor->setStringIndex(strdesc);
}
//---------------------------------------------------------------------------
