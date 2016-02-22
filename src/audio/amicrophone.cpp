#include "amicrophone.h"

AbstractMicrophone::AbstractMicrophone(const AudioFormat &format) :
    AudioDevice(format)
{    
}