#ifndef _OBJNETCLASSES_H
#define _OBJNETCLASSES_H

namespace Objnet
{

typedef enum
{
    // device groups:     0xXX000000
    cidController       = 0x01000000,
    cidSensor           = 0x02000000,
    cidActuator         = 0x04000000,
    cidPult             = 0x08000000,
    cidDongle           = 0x10000000,
    
    // device types:      0x00XX0000
    // Controller group:
    cidMechanical       = 0x00010000,
    cidOptical          = 0x00020000,
    cidElectrical       = 0x00030000,
    // Sensor group:
    cidEncoder          = 0x00010000,
    cidIMU              = 0x00020000,
    cidBio              = 0x00030000,
    // Actuator group:
    cidBrushedMotor     = 0x00010000,
    cidBrushlessMotor   = 0x00020000,
    cidSteppingMotor    = 0x00030000,
    // Pult group
    cidRadio            = 0x00010000,
    cidInfrared         = 0x00020000,
    
    // device subtypes:   0x0000XX00
    // Controller group:
    cidGeneric          = 0x00000000,
    cidExoskeleton      = 0x00001300,
    cidWheelchair       = 0x00001400,
    cidHandExo          = 0x00001500,
    cidLED              = 0x00002300,
    cidMice             = 0x00006600,
    cidFES              = 0x00001600,
    // Sensor group:
    cidMagnetic         = 0x00000100,
    cidMEMS             = 0x00000200,
    cidEMG              = 0x00000300,
    // Actuator:
    cidPosition         = 0x00000100,
    cidSpeed            = 0x00000200,
    cidTorque           = 0x00000400,
    cidCurrent          = 0x00000800,
    cidTemperature      = 0x00001000,
    cidVoltage          = 0x00002000,
    // pult:
    cidButtons          = 0x00001000,
    cidLeds             = 0x00002000,
    cidAnalog           = 0x00004000,
    
    // implementations:   0x000000XX    
    
    // combinations:
    cidBrushedMotorController = cidActuator | cidBrushedMotor,
    cidExoskeletonController = cidController | cidMechanical | cidExoskeleton,
    cidWheelchairController = cidController | cidMechanical | cidWheelchair,
    cidExoskeletonSensor = cidSensor | cidController | cidEncoder | cidIMU,
    cidLedController = cidController | cidOptical | cidLED,
    cidRotarod = cidController | cidMechanical | cidOptical | cidMice,
    cidHandExoController = cidController | cidMechanical | cidHandExo,
    cidExoPult = cidPult | cidRadio | cidButtons | cidLeds,
    cidEmgSensor = cidSensor | cidBio | cidEMG,
    cidFesController = cidController | cidElectrical | cidFES,
} ClassId;

}

#endif