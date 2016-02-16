#include "usbaudio.h"

using namespace Usb;
using namespace Audio;

InputTerminal::InputTerminal(TerminalType type) :
    UnitOrTerminal(new ITDescriptor(type))
{
  
}
//---------------------------------------------------------------------------

UsbAudioControlInterface::UsbAudioControlInterface(string name) :
    UsbInterface(USB_DEVICE_CLASS_AUDIO, AUDIO_SUBCLASS_AUDIOCONTROL, AUDIO_PROTOCOL_UNDEFINED, name)
{
  
}
//---------------------------------------------------------------------------

UsbAudioStreamInterface::UsbAudioStreamInterface(AudioFormat *format, string name) :
    UsbInterface(USB_DEVICE_CLASS_AUDIO, AUDIO_SUBCLASS_AUDIOSTREAMING, AUDIO_PROTOCOL_UNDEFINED, name),
    mFormatDescriptor(0L)
{
    if (format)
        mFormatDescriptor = new TypeIFormatDescriptor(*format);
}

UsbAudioStreamInterface::~UsbAudioStreamInterface()
{
    if (mFormatDescriptor)
        delete mFormatDescriptor; 
}