#ifndef _USBAUDIODESC_H
#define _USBAUDIODESC_H

#include "../usbdesc.h"

namespace Usb
{
namespace Audio
{
 
typedef enum
{
    TermUsbUndefined        = 0x0100, //!< [I/O] USB Terminal, undefined Type.
    TermUsbStreaming        = 0x0101, //!< [I/O] A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface.
    TermUsbVendorSpecific   = 0x01FF, //!< [I/O] A Terminal dealing with a signal carried over a vendor-specific interface.
    TermInputUndefined      = 0x0200, //!< [I] Input Terminal, undefined Type.
    TermMicrophone          = 0x0201, //!< [I] A generic microphone that does not fit under any of the other classifications.
    TermDesktopMicrophone   = 0x0202, //!< [I] A microphone normally placed on the desktop or integrated into the monitor.
    TermPersonalMicrophone  = 0x0203, //!< [I] A head-mounted or clip-on microphone.
    TermOmniDirMicrophone   = 0x0204, //!< [I] A microphone designed to pick up voice from more than one speaker at relatively long ranges.
    TermMicrophoneArray     = 0x0205, //!< [I] An array of microphones designed for directional processing using host-based signal processing algorithms.
    TermProcMicrophoneArray = 0x0206, //!< [I] An array of microphones with an embedded signal processor.
    TermOutputUndefined     = 0x0300, //!< [O] Output Terminal, undefined Type.
    TermSpeaker             = 0x0301, //!< [O] A generic speaker or set of speakers that does not fit under any of the other classifications.
    TermHeadphones          = 0x0302, //!< [O] A head-mounted audio output device.
    TermHMDAudio            = 0x0303, //!< [O] The audio part of a VR head mounted display.
    TermDesktopSpeaker      = 0x0304, //!< [O] Relatively small speaker or set of speakers normally placed on the desktop or integrated into the monitor. These speakers are close to the user and have limited stereo separation.
    TermRoomSpeaker         = 0x0305, //!< [O] Larger speaker or set of speakers that are heard well anywhere in the room.
    TermCommSpeaker         = 0x0306, //!< [O] Speaker or set of speakers designed for voice communication.
    TermLowFreqSpeaker      = 0x0307, //!< [O] Speaker designed for low frequencies (subwoofer). Not capable of reproducing speech or music.
    TermBiDirUndefined      = 0x0400, //!< [I/O] Bi-directional Terminal, undefined Type.
    TermHandset             = 0x0401, //!< [I/O] Hand-held bi-directional audio device.
    TermHeadset             = 0x0402, //!< [I/O] Head-mounted bi-directional audio device.
    TermSpeakerphone        = 0x0403, //!< [I/O] A hands-free audio device designed for host-based echo cancellation.
    TermEchoSupSpeakerphone = 0x0404, //!< [I/O] A hands-free audio device with echo suppression capable of half-duplex operation.
    TermEchoCancelSpeakerphone=0x0405,//!< [I/O] A hands-free audio device with echo cancellation capable of full-duplex operation.
    TermTelephonyUndefined  = 0x0500, //!< [I/O] Telephony Terminal, undefined Type.
    //!< telephony terminals are not described yet..
    TermExternalUndefined   = 0x0600, //!< [I/O] External Terminal, undefined Type.
    //!< external terminals are not described yet..
    TermEmbeddedUndefined   = 0x0700, //!< [I/O] Embedded Terminal, undefined Type.
    //!< embedded terminals are not described yet..
} TerminalType;
  
  
}
}

#endif