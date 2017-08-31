#include "usbif.h"

using namespace Usb;

UsbInterface::UsbInterface(unsigned char classCode, unsigned char subclassCode, unsigned char protocolCode, string name) :
    UsbNode(UsbNode::NodeTypeInterface),
    mCsInterface(0),
    mAltSetting(0)
{
    mInterfaceName = name;
    mDescriptor = new InterfaceDescriptor;
    mDescriptor->setInterfaceClass(classCode);
    mDescriptor->setInterfaceSubClass(subclassCode);
    mDescriptor->setInterfaceProtocol(protocolCode);
    assignDescriptor(mDescriptor);
}

UsbInterface::~UsbInterface()
{
//    if (csInterface)
//        delete csInterface;
}

void UsbInterface::addChild(UsbNode *child)
{
//    UsbCsInterface *csInterface = dynamic_cast<UsbCsInterface*>(child);
//    UsbAltSetting *altSetting = dynamic_cast<UsbAltSetting*>(child);
    UsbEndpoint *endpoint = dynamic_cast<UsbEndpoint*>(child);
    if (child->nodeType() == UsbNode::NodeTypeCsInterface)
    {
        mCsInterface = child;
    }
    else if (child->nodeType() == UsbNode::NodeTypeAltSetting)
    {
        // blabla
    }
    else if (child->nodeType() == UsbNode::NodeTypeEndpoint)
    {
        mDescriptor->incrementNumEndpoints();
        if (endpoint)
            endpoint->setEndpointNumber(mDescriptor->numEndpoints());
        if (mCsInterface)
            mCsInterface->addChild(child); // inform class specific interface about endpoints
    }
  
    UsbNode::addChild(child);
}

void UsbInterface::nodeAttached()
{
    unsigned char strdesc = device()->addStringDescriptor(mInterfaceName);
    mDescriptor->setStringIndex(strdesc);
}
//---------------------------------------------------------------------------

//void UsbInterface::addChild(UsbNode *altSetting)
//{
//    int ifNumber = mAltInterfacesCount;
//    mAltInterfacesCount++;
//    UsbInterface **temp = new UsbInterface*[mAltInterfacesCount];
//    for (int i=0; i<ifNumber; i++)
//        temp[i] = mAltInterfaces[i]; 
//    temp[ifNumber] = interface;
//    interface->setInterfaceNumber(mDescriptor.bInterfaceNumber);
//    interface->setAlternateSetting(mAltInterfacesCount);
//    if (mAltInterfaces)
//        delete [] mAltInterfaces;
//    mAltInterfaces = temp;
//}

//void UsbInterface::addEndpoint(UsbNode *endpoint)
//{
//    int epNumber = mDescriptor.bNumEndpoints;
//    mDescriptor.bNumEndpoints++;
//    UsbEndpoint **temp = new UsbEndpoint*[mDescriptor.bNumEndpoints];
//    for (int i=0; i<epNumber; i++)
//        temp[i] = mEndpoints[i]; 
//    temp[epNumber] = endpoint;
//    if (mEndpoints)
//        delete [] mEndpoints;
//    mEndpoints = temp;
//}
//---------------------------------------------------------------------------