#ifndef _USBDEV_H
#define _USBDEV_H

#include "usbcfg.h"
#include "usbdevdrv.h"
#include "usbreq.h"
#include "gpio.h"
#include "usbep.h"
#include <vector>
#include <map>

namespace Usb
{

class UsbConfiguration;
  
class UsbDevice : public UsbNode
{
public:
    typedef enum {OtgFs, OtgHs, OtgHsUlpi} UsbCore;
  
private:
    static UsbDevice *mFsCore, *mHsCore;
    UsbCore mUsbCore;
    USB_OTG_CORE_HANDLE mDev;
    UsbDeviceDriver *mDrv;
    DeviceDescriptor *mDeviceDescriptor;
    DeviceQualifierDescriptor mDeviceQualifierDescriptor;
    
    std::map<unsigned char, UsbEndpoint*> mEndpoints;
    
    unsigned char mDefaultConfig;
    __IO USB_OTG_DCTL_TypeDef mTestMode;
    
    std::vector<ByteArray> mStrings;
    unsigned char mConfigIdx;
    UsbConfiguration *mCurrentConfig;
    
    void bspInit();
    void bspEnableInterrupt();
    
    void setTestMode(USB_OTG_DCTL_TypeDef dctl);
    void runTestMode();
    void setupStage();
    void dataOutStage(unsigned char epnum);
    void dataInStage(unsigned char epnum);
    void sofHandler();
    void reset();
    void suspend();
    void resume();
    void isoInIncompleteHandler();
    void isoOutIncompleteHandler();
    #ifdef VBUS_SENSING_ENABLED
    void devConnected();
    void devDisconnected();
    #endif
    
    // standard requests:
    void handleStdReq(const UsbSetupReq &req);
    void handleStdItfReq(const UsbSetupReq &req);
    void handleStdEpReq(const UsbSetupReq &req);
    void getDescriptor(const UsbSetupReq &req);
    void setAddress(const UsbSetupReq &req);
    void setConfig(const UsbSetupReq &req);
    void getConfig(const UsbSetupReq &req);
    void getStatus(const UsbSetupReq &req);
    void setFeature(const UsbSetupReq &req);
    void clearFeature(const UsbSetupReq &req);
    
    void setConfig(unsigned char cfgidx);
    void clearConfig(unsigned char cfgidx);
  
protected:
    void addChild(UsbNode *configuration);
    
    virtual void classSetupRequest(const UsbSetupReq &req) {}
    virtual void vendorSetupRequest(const UsbSetupReq &req) {}
    
public:
    UsbDevice(UsbCore core = OtgFs);
    ~UsbDevice();
    
    //! For internal use only:
    static UsbDevice *fsCore() {return mFsCore;}
    static UsbDevice *hsCore() {return mHsCore;}
    USB_OTG_CORE_HANDLE *handle() {return &mDev;}
    UsbDeviceDriver *driver() {return mDrv;}
    unsigned char addStringDescriptor(const string &s);
    unsigned char addStringDescriptor(const wstring &s);
    void connectEndpoint(UsbEndpoint *ep);
    void disconnectEndpoint(UsbEndpoint *ep);
    
    UsbConfiguration *defaultConfiguration() {return (UsbConfiguration*)first();}
    
    void ctlError(const UsbSetupReq &req);
    
    unsigned char deviceStatus() const {return mDev.dev.device_status;}
    
    
    /*! Start USB stack.
        Call it after USB descriptor tree completion.
    */
    void start();
    
    /*! Assign class info to the device.
        Should be called before start().
        By default class, subclass and protocol are equal to 0.
        Call this function if device contains only one interface.
        \param devClass The class of the device according to the USB specification.
        \param subClass The subclass of the device according to the USB class-specific documentation. Default value is 0.
        \param protocol The protocol of the device according to the USB class-specific documentation. Default value is 0.
    */
    void setDeviceClass(unsigned char devClass, unsigned char subClass=0, unsigned char protocol=0);
    
    /*! Set maximum packet size of control endpoint transfer.
        Should be called before start().
        Default value is 64.
        \param size The maximum packet size in bytes of control endpoint (EP0).
    */
    void setMaxPacketSize(unsigned char size);
    
    /*! Set Vendor ID and Product ID.
        Should be called before start().
        If the function is not called, default values of VID=0x1234, PID=0x4321 are substituted.
        \param VID The Vendor ID of the device.
        \param PID The Product ID of the device.
    */
    void setVidPid(unsigned short VID, unsigned short PID);
    
    /*! Set the revision number of the device.
        Should be called before start().
        This value is vendor-defined. By default it is 0.0.
        \param majorVersion Revision major version.
        \param minorVersion Revision minor version.
    */
    void setDeviceRevisionNumber(unsigned char majorVersion, unsigned char minorVersion);
    
    /*! Set the LangID string in descriptor.
        Should be called before start().
        \param langId Language ID of the device.
    */
    void setLangId(unsigned short langId);
    
    /*! Set the manufacturer string in descriptor.
        Should be called before start().
        \param manufacturer String containing the manufacturer's name.
    */
    void setManufacturer(const string &manufacturer);
    
    /*! Set the manufacturer string in descriptor.
        This is overloaded function intended for dealing with multi-byte string.
        Should be called before start().
        \param manufacturer String containing the manufacturer's name.
    */
    void setManufacturer(const wstring &manufacturer);
    
    /*! Set the product string in descriptor.
        Should be called before start().
        \param product String containing the name of the product.
    */
    void setProduct(const string &product);
    
    /*! Set the product string in descriptor.
        This is overloaded function intended for dealing with multi-byte string.
        Should be called before start().
        \param product String containing the name of the product.
    */
    void setProduct(const wstring &product);
    
    /*! Set the serial number string in descriptor.
        Should be called before start().
        \param serial String containing the serial number of the product.
    */
    void setSerialNumber(const string &serial);
    
    /*! Set the serial number string in descriptor.
        This is overloaded function intended for dealing with multi-byte string.
        Should be called before start().
        \param serial String containing the serial number of the product.
    */
    void setSerialNumber(const wstring &serial);
    
    void setUsbSpecification(unsigned short spec);
};

}

#endif