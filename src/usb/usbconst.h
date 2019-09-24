#ifndef _USBCONST_H
#define _USBCONST_H

namespace Usb
{

typedef enum
{
    UsbClassAudio       = 0x01,
    UsbClassCdcControl  = 0x02,
    UsbClassHid         = 0x03,
    UsbClassMsc         = 0x08,
    UsbClassCdcData     = 0x0A,
} UsbClass;
  
#define USB_DEVICE_CLASS_AUDIO                          0x01
//#define USB_DEVICE_CLASS_HID                            0x03


#define USB_DESC_TYPE_DEVICE                               1
#define USB_DESC_TYPE_CONFIGURATION                        2
#define USB_DESC_TYPE_STRING                               3
#define USB_DESC_TYPE_INTERFACE                            4
#define USB_DESC_TYPE_ENDPOINT                             5
#define USB_DESC_TYPE_DEVICE_QUALIFIER                     6
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION            7

#define USB_DESC_TYPE_INTERFACE_COLLECTION              0x0b

#define USB_REQ_RECIPIENT_DEVICE                        0x00
#define USB_REQ_RECIPIENT_INTERFACE                     0x01
#define USB_REQ_RECIPIENT_ENDPOINT                      0x02

#define USB_REQ_GET_STATUS                              0x00
#define USB_REQ_CLEAR_FEATURE                           0x01
#define USB_REQ_SET_FEATURE                             0x03
#define USB_REQ_SET_ADDRESS                             0x05
#define USB_REQ_GET_DESCRIPTOR                          0x06
#define USB_REQ_SET_DESCRIPTOR                          0x07
#define USB_REQ_GET_CONFIGURATION                       0x08
#define USB_REQ_SET_CONFIGURATION                       0x09
#define USB_REQ_GET_INTERFACE                           0x0A
#define USB_REQ_SET_INTERFACE                           0x0B
#define USB_REQ_SYNCH_FRAME                             0x0C

#define USB_FEATURE_EP_HALT                                0
#define USB_FEATURE_REMOTE_WAKEUP                          1
#define USB_FEATURE_TEST_MODE                              2

#define USB_REQ_TYPE_STANDARD                           0x00
#define USB_REQ_TYPE_CLASS                              0x20
#define USB_REQ_TYPE_VENDOR                             0x40
#define USB_REQ_TYPE_MASK                               0x60


// constant defining default settings:
#define USB_OTG_MAX_EP0_SIZE                    64
#define USBD_VID                                0x1234                
#define USBD_PID                                0x4321
#define USBD_LANGID_STRING                      0x409
#define USBD_MANUFACTURER_STRING                L"<Manufacturer>"
#define USBD_PRODUCT_STRING                     L"<Product>"
#define USBD_SERIAL_NUMBER                      L"<Serial>"
  
}

#endif