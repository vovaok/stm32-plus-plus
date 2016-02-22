#ifndef _USBREQ_H
#define _USBREQ_H

namespace Usb
{

typedef struct
{
    unsigned char bmRequest;
    unsigned char bRequest;
    unsigned short wValue;
    unsigned short wIndex;
    unsigned short wLength;
} UsbSetupReq;

}

#endif