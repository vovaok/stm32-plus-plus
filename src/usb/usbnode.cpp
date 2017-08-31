#include "usbnode.h"
#include "usbdev.h" // poka tak

using namespace Usb;

UsbNode::UsbNode(NodeType type) :
  mType(type),
  mDevice(0L),
  mParent(0L),
  mBaseDescriptor(0L)
{
    if (mType == NodeTypeDevice)
        mDevice = reinterpret_cast<UsbDevice*>(this); // noxep, u6o dynamic_cast TyT He Pa6oTaeT!!
}
    
UsbNode::~UsbNode()
{
    while (!mChildren.empty())
    {
        delete mChildren.back();
        mChildren.pop_back();
    }
    delete mBaseDescriptor;
}
//---------------------------------------------------------------------------
 
void UsbNode::attachNode(UsbNode *node)
{   
    node->mParent = this;
    node->mDevice = mDevice;
    for (int i=0; i<node->mChildren.size(); i++)
        node->mChildren[i]->mDevice = mDevice;
//    if (node->mBaseDescriptor)
//        node->updateDescriptorLength(node->mBaseDescriptor->length());
    if (node->mBaseDescriptor)
    {
        ByteArray dummyBa;
        node->readDescriptor(dummyBa, true);
        node->updateDescriptorLength(dummyBa.size());
    }
    addChild(node);
    mChildren.push_back(node);
    node->nodeAttached();
//    if (mParent)
//    {   
//        if (mParent->nodeType() == NodeTypeDevice)
//        {
//            mDevice = dynamic_cast<UsbDevice*>(mParent);
//            mParent->mDevice = mDevice;
//        }
//        mDevice = mParent->mDevice;
//        mParent->addChild(this);
//    }
}
//---------------------------------------------------------------------------

//const unsigned char* UsbNode::getDescriptor(unsigned int *length)
//{
//    *length = mDescriptor? mDescriptor->length(): 0;
//    return mDescriptor? mDescriptor->data(): 0L;
//}
  
void UsbNode::readDescriptor(ByteArray &ba, bool recursive)
{
    if (mBaseDescriptor)
    {
        ba += mBaseDescriptor->toByteArray();
        //mBaseDescriptor->readBytes(ba);
    }
    if (recursive)
    {
        for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
            (*it)->readDescriptor(ba);
    }
}
//---------------------------------------------------------------------------

void UsbNode::init()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->init();
}

void UsbNode::deInit()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->deInit();
}

void UsbNode::setup(const UsbSetupReq &req)
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->setup(req);
}

void UsbNode::ep0RxReady()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->ep0RxReady();
}

void UsbNode::ep0TxSent()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->ep0TxSent();
}

//void UsbNode::dataOut(unsigned char epnum)
//{
//    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
//        (*it)->dataOut(epnum);
//}
//
//void UsbNode::dataIn(unsigned char epnum)
//{
//    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
//        (*it)->dataIn(epnum);
//}

void UsbNode::sof()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->sof();
}

void UsbNode::isoInIncomplete()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->isoInIncomplete();
}

void UsbNode::isoOutIncomplete()
{
    for (UsbNodeVector::iterator it=mChildren.begin(); it!=mChildren.end(); it++)
        (*it)->isoOutIncomplete();
}
//---------------------------------------------------------------------------
