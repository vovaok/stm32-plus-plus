#ifndef _USBNODE_H
#define _USBNODE_H

#include <vector>
#include "usbdesc.h"
#include "usbreq.h"

namespace Usb
{

class UsbDevice;
  
//! brief USB device tree node
//! p.s. each node owns its children and can destroy them
class UsbNode
{
protected:
    typedef enum
    {
        NodeTypeDevice,
        NodeTypeConfig,
        NodeTypeInterfaceCollection,
        NodeTypeInterface,
        NodeTypeAltSetting,
        NodeTypeCsInterface,
        NodeTypeEndpoint,
        NodeTypeFunc, // X3 est li takoe vawe
    } NodeType;
  
private:
    NodeType mType;
    UsbDevice *mDevice;
    UsbNode *mParent;
    typedef std::vector<UsbNode*> UsbNodeVector;
    UsbNodeVector mChildren;
    Descriptor *mBaseDescriptor;
    
protected:
    void assignDescriptor(Descriptor *descriptor) {mBaseDescriptor = descriptor;}
    virtual void updateDescriptorLength(int deltaLength) {if (mParent) mParent->updateDescriptorLength(deltaLength);}
  
    UsbNodeVector &children() {return mChildren;}
    virtual void addChild(UsbNode *child) {}
    int childrenCount() const {return mChildren.size();}
    virtual void nodeAttached() {}
    
    UsbNode *parent() {return mParent;}
    UsbNode *first() {return mChildren.empty()? 0L: mChildren.front();}
    UsbNode *getChildByNumber(unsigned char i) {return (i-1)<mChildren.size()? mChildren[i-1]: 0L;}
    
    UsbDevice *device() {return mDevice;}
    
    friend class UsbDevice;
    friend class UsbConfiguration;
    friend class UsbInterface;
    friend class UsbEndpoint;
    
public:
    UsbNode(NodeType type);
    virtual ~UsbNode();
    
    // internal use
    NodeType nodeType() const {return mType;}
    virtual void init();
    virtual void deInit();
    virtual void setup(const UsbSetupReq &req);
    virtual void ep0RxReady();
    virtual void ep0TxSent();
//    virtual void dataOut(unsigned char epnum);
//    virtual void dataIn(unsigned char epnum);
    virtual void sof();
    virtual void isoInIncomplete();
    virtual void isoOutIncomplete();
    
//    const unsigned char* getDescriptor(unsigned int *length);
    
    void attachNode(UsbNode *node);
      
    const Descriptor *descriptor() const {return mBaseDescriptor;}
    void readDescriptor(ByteArray &ba, bool recursive=true);  
    //void childCount() const {return mChildCount;}
    
    virtual string name() const {return "";}
};

};

#endif