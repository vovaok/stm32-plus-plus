#ifndef _USBAUDIO
#define _USBAUDIO

#include "../usbdesc.h"
#include "../usbif.h"
#include "../usbcfg.h"
#include "../../audio/audioformat.h"
#include "usbaudiodesc.h"

//#define AUDIO_DESCRIPTOR_TYPE                         0x21
//#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_PROTOCOL_UNDEFINED                      0x00
#define AUDIO_STREAMING_GENERAL                       0x01
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

#define AUDIO_CONTROL_HEADER                          0x01
#define AUDIO_CONTROL_INPUT_TERMINAL                  0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL                 0x03
#define AUDIO_CONTROL_FEATURE_UNIT                    0x06

#define AUDIO_FORMAT_TYPE_I                           0x01
#define AUDIO_FORMAT_TYPE_III                         0x03

namespace Usb
{ 
namespace Audio
{
  

class UsbAudioControlInterface;
class UsbAudioStreamInterface;
  
class ACInterfaceDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 8
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned short bcdADC;
        unsigned short wTotalLength;
        unsigned char bInCollection;
        unsigned char baInterfaceNr;
    } Fields;
#pragma pack(pop) 

    ACInterfaceDescriptor() :
        Descriptor(8)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 8;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_CONTROL_HEADER;
        m->bcdADC = 0x0100;
    }
    
//    const unsigned char* getAdditional(unsigned int *len) const {*len = bInCollection; return baInterfaceNr;}
    
    void addInterfaceNr(unsigned char ifNr)
    {
//        int oldNr = bInCollection;
//        bInCollection++;
//        bLength++;
//        unsigned char *temp = new unsigned char[bInCollection];
//        for (int i=0; i<oldNr; i++)
//            temp[i] = baInterfaceNr[i];
//        temp[oldNr] = ifNr;
//        if (baInterfaceNr)
//            delete baInterfaceNr;
//        baInterfaceNr = temp;
    }
    
    friend class UsbAudioControlInterface;
};

class ASInterfaceDescriptor : public Descriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 7
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned char bTerminalLink;
        unsigned char bDelay;
        unsigned short wFormatTag;
    } Fields;
#pragma pack(pop) 

    ASInterfaceDescriptor() :
      Descriptor(7)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 7;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_STREAMING_GENERAL;
        m->bDelay = 1;
        m->wFormatTag = 0x0001;
    }
    
    friend class UsbAudioStreamInterface;
};

class UnitOrTerminalDescriptor : public Descriptor
{
public:
    UnitOrTerminalDescriptor(int length) :
        Descriptor(length)
        {
        }
    virtual ~UnitOrTerminalDescriptor();
    virtual void setId(unsigned char id) = 0;
    virtual void setSource(unsigned char id) {}
};

class ITDescriptor : public UnitOrTerminalDescriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 12
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned char bTerminalID;
        unsigned short wTerminalType;
        unsigned char bAssocTerminal;
        unsigned char bNrChannels;
        unsigned short wChannelConfig;
        unsigned char iChannelNames;
        unsigned char iTerminal;
    } Fields;
#pragma pack(pop) 
    
    ITDescriptor(TerminalType type) :
        UnitOrTerminalDescriptor(12)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 12;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_CONTROL_INPUT_TERMINAL;
        m->wTerminalType = type;
    }
    
    void setId(unsigned char id) {reinterpret_cast<Fields*>(data())->bTerminalID = id;}
    
    friend class InputTerminal;
};

class OTDescriptor : public UnitOrTerminalDescriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 9
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned char bTerminalID;
        unsigned short wTerminalType;
        unsigned char bAssocTerminal;
        unsigned char bSourceId;
        unsigned char iTerminal;
    } Fields;
#pragma pack(pop) 
    
    OTDescriptor(TerminalType type) :
        UnitOrTerminalDescriptor(9)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 9;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_CONTROL_OUTPUT_TERMINAL;
        m->wTerminalType = type;
    }
    
    void setId(unsigned char id) {reinterpret_cast<Fields*>(data())->bTerminalID = id;}
    void setSource(unsigned char id) {reinterpret_cast<Fields*>(data())->bSourceId = id;}
    
    friend class OutputTerminal;
};

class FUDescriptor : public UnitOrTerminalDescriptor
{
public:
    typedef enum
    {
        None        = 0,
        Mute        = (1<<0),
        Volume      = (1<<1),
        Bass        = (1<<2),
        Mid         = (1<<3),
        Treble      = (1<<4),
        Equalizer   = (1<<5),
        AutoGain    = (1<<6),
        Delay       = (1<<7)
    } FeatureControls;
  
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 8
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned char bUnitID;
        unsigned char bSourceId;
        unsigned char bControlSize;
        unsigned char bmaControls0;
        unsigned char iFeature;
    } Fields;
#pragma pack(pop)
    
    FUDescriptor() :
        UnitOrTerminalDescriptor(8)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 8;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_CONTROL_FEATURE_UNIT;
        m->bControlSize = 1;
    }
    
    void setId(unsigned char id) {reinterpret_cast<Fields*>(data())->bUnitID = id;}
    void setSource(unsigned char id) {reinterpret_cast<Fields*>(data())->bSourceId = id;}
    void setFeatures(FeatureControls feat) {reinterpret_cast<Fields*>(data())->bmaControls0 = feat;}
    
    friend class FeatureUnit;
};

class AudioFormatDescriptor : public Descriptor
{      
public:
    AudioFormatDescriptor(int length) :
        Descriptor(length)
        {
        }
};

class TypeIFormatDescriptor : public AudioFormatDescriptor
{
private:
#pragma pack(push,1)
    typedef struct
    {
        unsigned char bLength;          // the length of the descriptor is equal to 11
        unsigned char bDescriptorType;  // descriptor type is class-specific
        unsigned char bDescriptorSubType;
        unsigned char bFormatType;
        unsigned char bNrChannels;
        unsigned char bSubFrameSize;
        unsigned char bBitResolution;
        unsigned char bSamFreqType;
        unsigned long tSamFreq; // 3 bytes used
    } Fields;
#pragma pack(pop)
    
    TypeIFormatDescriptor(const AudioFormat &format = AudioFormat()) :
        AudioFormatDescriptor(11)
    {
        Fields *m = reinterpret_cast<Fields*>(data());
        m->bLength = 11;
        m->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
        m->bDescriptorSubType = AUDIO_STREAMING_FORMAT_TYPE;
        m->bFormatType = AUDIO_FORMAT_TYPE_I;
        m->bNrChannels = format.channelCount();
        m->bSubFrameSize = format.bytesPerSample();
        m->bBitResolution = format.sampleSize();
        m->bSamFreqType = 1;
        m->tSamFreq = format.sampleRate();
    }
    
    friend class UsbAudioStreamInterface;
};
//---------------------------------------------------------------------------  

class UnitOrTerminal
{
private:
    UnitOrTerminalDescriptor *mDescriptor;
public:
    UnitOrTerminal(UnitOrTerminalDescriptor *descriptor) {mDescriptor = descriptor;}
    ~UnitOrTerminal() {delete mDescriptor;}
};

class InputTerminal : public UnitOrTerminal
{
public:
    InputTerminal(TerminalType type);
};
//---------------------------------------------------------------------------

class UsbAudioStreamZeroBandwidthInterface : public UsbInterface
{
public:
    UsbAudioStreamZeroBandwidthInterface() :
      UsbInterface(0L, USB_DEVICE_CLASS_AUDIO, AUDIO_SUBCLASS_AUDIOSTREAMING, AUDIO_PROTOCOL_UNDEFINED)
    {}
};

class UsbAudioStreamInterface : public UsbInterface
{
private :
    ASInterfaceDescriptor mDescriptor;
    AudioFormatDescriptor *mFormatDescriptor;
  
public:
    UsbAudioStreamInterface(AudioFormat *format, string name="Audio Stream");
    ~UsbAudioStreamInterface();
    
    //void add
};

class UsbAudioControlInterface : public UsbInterface
{
private:
    ACInterfaceDescriptor mDescriptor;
    UnitOrTerminal **mNodes;
    int mNodesCount;
  
public:
    UsbAudioControlInterface(string name="Audio Control");
    
    void linkStreamInterface(UsbAudioStreamInterface *interface);
};

}
}
#endif