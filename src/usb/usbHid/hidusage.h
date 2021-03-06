#ifndef _HIDUSAGE_H
#define _HIDUSAGE_H

namespace Usb
{
namespace Hid
{
  
#pragma pack(push,2)
typedef enum
{
    Undefined               = 0x00,
    GenericDesktop          = 0x01,
    SimulationControls      = 0x02,
    VRControls              = 0x03,
    SportControls           = 0x04,
    GameControls            = 0x05,
    GenericDeviceControls   = 0x06,
    KeyboardKeypad          = 0x07,
    LEDs                    = 0x08,
    Buttons                 = 0x09,
    Ordinal                 = 0x0A,
    Telephony               = 0x0B,
    ConsumerDevices         = 0x0C,
    Digitizer_Page          = 0x0D,
    PID_Page                = 0x0F, //!< see "USB Physical Interface Device definitions for force feedback and related devices" for details
    Unicode                 = 0x10,
    AlphaNumericDisplay     = 0x14,
    MedicalInstruments      = 0x40,
    Monitor_Page0           = 0x80, //!< see "USB Device Class Definition for Monitor Devices" for details
    Monitor_Page1           = 0x81,
    Monitor_Page2           = 0x82,
    Monitor_Page3           = 0x83,
    Power_Page0             = 0x84, //!< see "USB Device Class Definition for Power Devices" for details
    Power_Page1             = 0x85,
    Power_Page2             = 0x86,
    Power_Page3             = 0x87,
    BarCodeScanner_Page     = 0x8C, //!< see "USB Device Class Definition for Point of Sale Devices" for details
    Scale_Page              = 0x8D,
    MSRDevices              = 0x8E, //!< Magnetic stripe reading devices
    CameraControl_Page      = 0x90, //!< see "USB Device Class Definition for Image Class Devices" for details
    ArcadePage              = 0x91, //!< see "OAAF Definitions for arcade and coinop related Devices " for details
    VendorDefined         = 0xFF00, //!< first vendor-defined usage page
    VendorDefinedEnd      = 0xFFFF, //!< the last possible vendor-defined page
} UsagePageName;
    
typedef enum
{
    Pointer                 = 0x01,
    Mouse                   = 0x02,
    Joystick                = 0x04,
    GamePad                 = 0x05,
    Keyboard                = 0x06,
    Keypad                  = 0x07,
    MultiAxisController     = 0x08,
    TabletPcSystemControls  = 0x09,
    X                       = 0x30,
    Y                       = 0x31,
    Z                       = 0x32,
    Rx                      = 0x33,
    Ry                      = 0x34,
    Rz                      = 0x35,
    Slider                  = 0x36,
    Dial                    = 0x37,
    Wheel                   = 0x38,
    HatSwitch               = 0x39,
    CountedBuffer           = 0x3A,
    ByteCount               = 0x3B,
    MotionWakeup            = 0x3C,
    Start                   = 0x3D,
    Select                  = 0x3E,
    Vx                      = 0x40,
    Vy                      = 0x41,
    Vz                      = 0x42,
    Vbrx                    = 0x43,
    Vbry                    = 0x44,
    Vbrz                    = 0x45,
    Vno                     = 0x46,
    FeatureNotification     = 0x47,
    ResolutionMultiplier    = 0x48,
    SystemControl           = 0x80,
    SystemPowerDown         = 0x81,
    SystemSleep             = 0x82,
    SystemWakeUp            = 0x83,
    SystemContextMenu       = 0x84,
    SystemMainMenu          = 0x85,
    SystemAppMenu           = 0x86,
    SystemMenuHelp          = 0x87,
    SystemMenuExit          = 0x88,
    SystemMenuSelect        = 0x89,
    SystemMenuRight         = 0x8A,
    SystemMenuLeft          = 0x8B,
    SystemMenuUp            = 0x8C,
    SystemMenuDown          = 0x8D,
    SystemColdRestart       = 0x8E,
    SystemWarmRestart       = 0x8F,
    DpadUp                  = 0x90,
    DpadDown                = 0x91,
    DpadRight               = 0x92,
    DpadLeft                = 0x93,
    SystemDock              = 0xA0,
    SystemUndock            = 0xA1,
    SystemSetup             = 0xA2,
    SystemBreak             = 0xA3,
    SystemDebuggerBreak     = 0xA4,
    ApplicationBreak        = 0xA5,
    ApplicationDebuggerBreak= 0xA6,
    SystemSpeakerMute       = 0xA7,
    SystemHibernate         = 0xA8,
    SystemDisplayInvert     = 0xB0,
    SystemDisplayInternal   = 0xB1,
    SystemDisplayExternal   = 0xB2,
    SystemDisplayBoth       = 0xB3,
    SystemDisplayDual       = 0xB4,
    SystemDisplayToggle     = 0xB5,
    SystemDisplaySwap       = 0xB6,
    SystemDisplayLcdAutoscale = 0xB7,
} GenericDesktopPage;

typedef enum
{
    FlightSimulationDevice      = 0x01,
    AutomobileSimulationDevice  = 0x02,
    TankSimulationDevice        = 0x03,
    SpaceshipSimulationDevice   = 0x04,
    SubmarineSimulationDevice   = 0x05,
    SailingSimulationDevice     = 0x06,
    MotorcycleSimulationDevice  = 0x07,
    SportsSimulationDevice      = 0x08,
    AirplaneSimulationDevice    = 0x09,
    HelicopterSimulationDevice  = 0x0A,
    MagicCarpetSimulationDevice = 0x0B,
    BicycleSimulationDevice     = 0x0C,
    FlightControlStick          = 0x20,
    FlightStick                 = 0x21,
    CyclicControl               = 0x22,
    CyclicTrim                  = 0x23,
    FlightYoke                  = 0x24,
    TrackControl                = 0x25,
    Aileron                     = 0xB0,
    AileronTrim                 = 0xB1,
    AntiTorqueControl           = 0xB2,
    AutopilotEnable             = 0xB3,
    ChaffRelease                = 0xB4,
    CollectiveControl           = 0xB5,
    DiveBrake                   = 0xB6,
    ElectronicCountermeasures   = 0xB7,
    Elevator                    = 0xB8,
    ElevatorTrim                = 0xB9,
    Rudder                      = 0xBA,
    Throttle                    = 0xBB,
    FlightCommunications        = 0xBC,
    FlareRelease                = 0xBD,
    LandingGear                 = 0xBE,
    ToeBrake                    = 0xBF,
    Trigger                     = 0xC0,
    WeaponsArm                  = 0xC1,
    WeaponsSelect               = 0xC2,
    WingFlaps                   = 0xC3,
    Accelerator                 = 0xC4,
    Brake                       = 0xC5,
    Clutch                      = 0xC6,
    Shifter                     = 0xC7,
    Steering                    = 0xC8,
    TurretDirection             = 0xC9,
    BarrelElevation             = 0xCA,
    DivePlane                   = 0xCB,
    Ballast                     = 0xCC,
    BicycleCrank                = 0xCD,
    HandleBars                  = 0xCE,
    FrontBrake                  = 0xCF,
    RearBrake                   = 0xD0
} SimulationControlsPage;

typedef enum
{
    Belt                        = 0x01,
    BodySuit                    = 0x02,
    Flexor                      = 0x03,
    Glove                       = 0x04,
    HeadTracker                 = 0x05,
    HeadMountedDisplay          = 0x06,
    HandTracker                 = 0x07,
    Oculometer                  = 0x08,
    Vest                        = 0x09,
    AnimatronicDevice           = 0x0A,
    StereoEnable                = 0x20,
    DisplayEnable               = 0x21
} VrControlsPage;

typedef enum
{
    _3DGameController           = 0x01,
    PinballDevice               = 0x02,
    GunDevice                   = 0x03,
    PointOfView                 = 0x20,
    TurnRightLeft               = 0x21,
    PitchForwardBackward        = 0x22,
    RollRightLeft               = 0x23,
    MoveRightLeft               = 0x24,
    MoveForwardBackward         = 0x25,
    MoveUpDown                  = 0x26,
    LeanRightLeft               = 0x27,
    LeanForwardBackward         = 0x28,
    HeightOfPov                 = 0x29,
    Flipper                     = 0x2A,
    SecondaryFlipper            = 0x2B,
    Bump                        = 0x2C,
    NewGame                     = 0x2D,
    ShootBall                   = 0x2E,
    Player                      = 0x2F,
    GunBolt                     = 0x30,
    GunClip                     = 0x31,
    GunSelector                 = 0x32,
    GunSingleShot               = 0x33,
    GunBurst                    = 0x34,
    GunAutomatic                = 0x35,
    GunSafety                   = 0x36,
    GamepadFireJump             = 0x37,
    GamepadTrigger              = 0x39
} GameControlsPage;

typedef enum
{
    BatteryStrength             = 0x20,
    WirelessChannel             = 0x21,
    WirelessId                  = 0x22,
    DiscoverWirelessControl     = 0x23,
    SecurityCodeCharacterEntered= 0x24,
    SecurityCodeCharacterErased = 0x25,
    SecurityCodeCleared         = 0x26
} GenericDeviceControlsPage;

typedef enum
{
    KeyboardErrorRollOver       = 0x01,
    KeyboardPostFail            = 0x02,
    KeyboardErrorUndefined      = 0x03,
    KeyboardA                   = 0x04,
    KeyboardB                   = 0x05,
    KeyboardC                   = 0x06,
    KeyboardD                   = 0x07,
    KeyboardE                   = 0x08,
    KeyboardF                   = 0x09,
    KeyboardG                   = 0x0A,
    KeyboardH                   = 0x0B,
    KeyboardI                   = 0x0C,
    KeyboardJ                   = 0x0D,
    KeyboardK                   = 0x0E,
    KeyboardL                   = 0x0F,
    KeyboardM                   = 0x10,
    KeyboardN                   = 0x11,
    KeyboardO                   = 0x12,
    KeyboardP                   = 0x13,
    KeyboardQ                   = 0x14,
    KeyboardR                   = 0x15,
    KeyboardS                   = 0x16,
    KeyboardT                   = 0x17,
    KeyboardU                   = 0x18,
    KeyboardV                   = 0x19,
    KeyboardW                   = 0x1A,
    KeyboardX                   = 0x1B,
    KeyboardY                   = 0x1C,
    KeyboardZ                   = 0x1D,
    Keyboard1                   = 0x1E,
    Keyboard2                   = 0x1F,
    Keyboard3                   = 0x20,
    Keyboard4                   = 0x21,
    Keyboard5                   = 0x22,
    Keyboard6                   = 0x23,
    Keyboard7                   = 0x24,
    Keyboard8                   = 0x25,
    Keyboard9                   = 0x26,
    Keyboard0                   = 0x27,
    KeyboardReturn              = 0x28,
    KeyboardEscape              = 0x29,
    KeyboardDelete              = 0x2A,
    KeyboardTab                 = 0x2B,
    KeyboardSpacebar            = 0x2C,
    KeyboardMinus               = 0x2D,
    KeyboardEqual               = 0x2E,
    KeyboardSquareBracket1      = 0x2F,
    KeyboardSquareBracket2      = 0x30,
    KeyboardBackslash           = 0x31,
    KeyboardNonUsSharp          = 0x32,
    KeyboardSemicolon           = 0x33,
    KeyboardQuotes              = 0x34,
    KeyboardGraveAccent         = 0x35,
    KeyboardComma               = 0x36,
    KeyboardDot                 = 0x37,
    KeyboardSlash               = 0x38,
    KeyboardCapsLock            = 0x39,
    KeyboardF1                  = 0x3A,
    KeyboardF2                  = 0x3B,
    KeyboardF3                  = 0x3C,
    KeyboardF4                  = 0x3D,
    KeyboardF5                  = 0x3E,
    KeyboardF6                  = 0x3F,
    KeyboardF7                  = 0x40,
    KeyboardF8                  = 0x41,
    KeyboardF9                  = 0x42,
    KeyboardF10                 = 0x43,
    KeyboardF11                 = 0x44,
    KeyboardF12                 = 0x45,
    KeyboardPrintScreen         = 0x46,
    KeyboardScrollLock          = 0x47,
    KeyboardPause               = 0x48,
    KeyboardInsert              = 0x49,
    KeyboardHome                = 0x4A,
    KeyboardPageUp              = 0x4B,
    KeyboardDeleteForward       = 0x4C,
    KeyboardEnd                 = 0x4D,
    KeyboardPageDown            = 0x4E,
    KeyboardRightArrow          = 0x4F,
    KeyboardLeftArrow           = 0x50,
    KeyboardDownArrow           = 0x51,
    KeyboardUpArrow             = 0x52,
    KeypadNumLock               = 0x53,
    KeypadDivide                = 0x54,
    KeypadMultiply              = 0x55,
    KeypadMinus                 = 0x56,
    KeypadPlus                  = 0x57,
    KeypadEnter                 = 0x58,
    Keypad1                     = 0x59,
    Keypad2                     = 0x5A,
    Keypad3                     = 0x5B,
    Keypad4                     = 0x5C,
    Keypad5                     = 0x5D,
    Keypad6                     = 0x5E,
    Keypad7                     = 0x5F,
    Keypad8                     = 0x60,
    Keypad9                     = 0x61,
    Keypad0                     = 0x62,
    KeypadDot                   = 0x63,
    KeyboardNonUsBackslash      = 0x64,
    KeyboardApplication         = 0x65,
    KeyboardPower               = 0x66,
    KeypadEqual                 = 0x67,
    KeyboardF13                 = 0x68,
    KeyboardF14                 = 0x69,
    KeyboardF15                 = 0x6A,
    KeyboardF16                 = 0x6B,
    KeyboardF17                 = 0x6C,
    KeyboardF18                 = 0x6D,
    KeyboardF19                 = 0x6E,
    KeyboardF20                 = 0x6F,
    KeyboardF21                 = 0x70,
    KeyboardF22                 = 0x71,
    KeyboardF23                 = 0x72,
    KeyboardF24                 = 0x73,
    KeyboardExecute             = 0x74,
    KeyboardHelp                = 0x75,
    KeyboardMenu                = 0x76,
    KeyboardSelect              = 0x77,
    KeyboardStop                = 0x78,
    KeyboardAgain               = 0x79,
    KeyboardUndo                = 0x7A,
    KeyboardCut                 = 0x7B,
    KeyboardCopy                = 0x7C,
    KeyboardPaste               = 0x7D,
    KeyboardFind                = 0x7E,
    KeyboardMute                = 0x7F,
    KeyboardVolumeUp            = 0x80,
    KeyboardVolumeDown          = 0x81,
    KeyboardLockingCapsLock     = 0x82,
    KeyboardLockingNumLock      = 0x83,
    KeyboardLockingScrollLock   = 0x84,
    KeypadComma                 = 0x85,
    KeypadEqualSign             = 0x86,
    KeypadBlablablaMneNadoelo   = 0x87,
    KeyboardLeftControl         = 0xE0,
    KeyboardLeftShift           = 0xE1,
    KeyboardLeftAlt             = 0xE2,
    KeyboardLeftGui             = 0xE3,
    KeyboardRightControl        = 0xE4,
    KeyboardRightShift          = 0xE5,
    KeyboardRightAlt            = 0xE6,
    KeyboardRightGui            = 0xE7,
} KeyboardKeypadPage;

typedef enum
{
    NumLock                     = 0x01,
    CapsLock                    = 0x02,
    ScrollLock                  = 0x03,
    Compose                     = 0x04,
    Kana                        = 0x05,
    PowerLed                    = 0x06,
    Shift                       = 0x07,
    DoNotDisturb                = 0x08,
    Mute                        = 0x09,
    ToneEnable                  = 0x0A,
    HighCutFilter               = 0x0B,
    LowCutFilter                = 0x0C,
    EqualizerEnable             = 0x0D,
    SoundFieldOn                = 0x0E,
    SurroundOn                  = 0x0F,
    Repeat                      = 0x10,
    Stereo                      = 0x11,
    SamplingRateDetect          = 0x12,
    Spinning                    = 0x13,
    CAV                         = 0x14,
    CLV                         = 0x15,
    RecordingFormatDetect       = 0x16,
    OffHook                     = 0x17,
    Ring                        = 0x18,
    MessageWaiting              = 0x19,
    DataMode                    = 0x1A,
    BatteryOperation            = 0x1B,
    BatteryOk                   = 0x1C,
    BatteryLow                  = 0x1D,
    Speaker                     = 0x1E,
    HeadSet                     = 0x1F,
    Hold                        = 0x20,
    MicrophoneLed               = 0x21,
    Coverage                    = 0x22,
    NightMode                   = 0x23,
    SendCalls                   = 0x24,
    CallPickup                  = 0x25,
    Conference                  = 0x26,
    Standby                     = 0x27,
    CameraOn                    = 0x28,
    CameraOff                   = 0x29,
    OnLine                      = 0x2A,
    OffLine                     = 0x2B,
    Busy                        = 0x2C,
    Ready                       = 0x2D,
    PaperOut                    = 0x2E,
    PaperJam                    = 0x2F,
    Remote                      = 0x30,
    Forward                     = 0x31,
    Reverse                     = 0x32,
    Stop                        = 0x33,
    Rewind                      = 0x34,
    FastForward                 = 0x35,
    Play                        = 0x36,
    Pause                       = 0x37,
    Record                      = 0x38,
    Error                       = 0x39,
    UsageSelectedIndicator      = 0x3A,
    UsageInUseIndicator         = 0x3B,
    UsageMultiModeIndicator     = 0x3C,
    IndicatorOn                 = 0x3D,
    IndicatorFlash              = 0x3E,
    IndicatorSlowBlink          = 0x3F,
    IndicatorFastBlink          = 0x40,
    IndicatorOff                = 0x41,
    FlashOnTime                 = 0x42,
    SlowBlinkOnTime             = 0x43,
    SlowBlinkOffTime            = 0x44,
    FastBlinkOnTime             = 0x45,
    FastBlinkOffTime            = 0x46,
    UsageIndicatorColor         = 0x47,
    IndicatorRed                = 0x48,
    IndicatorGreen              = 0x49,
    IndicatorAmber              = 0x4A,
    GenericIndicator            = 0x4B,
    SystemSuspend               = 0x4C,
    ExternalPowerConnected      = 0x4D
} LedPage;

typedef enum
{
    Button1                     = 0x01,
    Button2                     = 0x02,
    Button3                     = 0x03,
    Button4                     = 0x04,
    Button5                     = 0x05,
    Button6                     = 0x06,
    Button7                     = 0x07,
    Button8                     = 0x08,
    Button9                     = 0x09,
    Button10                    = 0x0A,
    Button11                    = 0x0B,
    Button12                    = 0x0C,
    Button13                    = 0x0D,
    Button14                    = 0x0E,
    Button15                    = 0x0F,
    Button16                    = 0x10,
} ButtonPage;

typedef enum
{
    ConsumerControl             = 0x01,
    NumericKeyPad               = 0x02,
    ProgrammableButtons         = 0x03,
    MicrophoneConsumer          = 0x04,
    Headphone                   = 0x05,
    GraphicEqualizer            = 0x06,
    Plus10                      = 0x20,
    Plus100                     = 0x21,
    AmPm                        = 0x22,
    PowerConsumer               = 0x30,
    Reset                       = 0x31,
    Sleep                       = 0x32,
    SleepAfter                  = 0x33,
    SleepMode                   = 0x34,
    Illumination                = 0x35,
    FunctionButtons             = 0x36,
    Menu                        = 0x40,
    MenuPick                    = 0x41,
    MenuUp                      = 0x42,
    MenuDown                    = 0x43,
    MenuLeft                    = 0x44,
    MenuRight                   = 0x45,
    MenuEscape                  = 0x46,
    MenuValueIncrease           = 0x47,
    MenuValueDecrease           = 0x48,
    DataOnScreen                = 0x60,
    ClosedCaption               = 0x61,
    ClosedCaptionSelect         = 0x62,
    VcrTv                       = 0x63,
    BroadcastMode               = 0x64,
    Snapshot                    = 0x65,
    Still                       = 0x66,
    Selection                   = 0x80,
    AssignSelection             = 0x81,
    ModeStep                    = 0x82,
    RecallLast                  = 0x83,
    EnterChannel                = 0x84,
    OrderMovie                  = 0x85,
    Channel                     = 0x86,
    MediaSelection              = 0x87,
    MediaSelectComputer         = 0x88,
    MediaSelectTv               = 0x89,
    MediaSelectWww              = 0x8A,
    MediaSelectDvd              = 0x8B,
    MediaSelectTelephone        = 0x8C,
    MediaSelectProgramGuide     = 0x8D,
    MediaSelectVideoPhone       = 0x8E,
    MediaSelectGames            = 0x8F,
    MediaSelectMessages         = 0x90,
    MediaSelectCd               = 0x91,
    MediaSelectVcr              = 0x92,
    MediaSelectTuner            = 0x93,
    Quit                        = 0x94,
    Help                        = 0x95,
    MediaSelectTape             = 0x96,
    MediaSelectCable            = 0x97,
    MediaSelectSatellite        = 0x98,
    MediaSelectSecurity         = 0x99,
    MediaSelectHome             = 0x9A,
    MediaSelectCall             = 0x9B,
    ChannelIncrement            = 0x9C,
    ChannelDecrement            = 0x9D,
    MediaSelectSap              = 0x9E,
    VcrPlus                     = 0xA0,
    Once                        = 0xA1,
    Daily                       = 0xA2,
    Blablablablablablablablabla = 0xA3,
    GenericGuiApplicationControls = 0x200,
    AcNew                       = 0x201,
    AcOpen                      = 0x202,
    AcClose                     = 0x203,
    AcExit                      = 0x204,
    AcMaximize                  = 0x205,
    AcMinimize                  = 0x206,
    AcSave                      = 0x207,
    AcPrint                     = 0x208,
    AcProperties                = 0x209,
    AcUndo                      = 0x21A,
    AcCopy                      = 0x21B,
    AcCut                       = 0x21C,
    AcPaste                     = 0x21D,
    AcSelectAll                 = 0x21E,
    AcFind                      = 0x21F,
    AcFindAndReplace            = 0x220,
    AcSearch                    = 0x221,
    AcGoTo                      = 0x222,
    AcHome                      = 0x223,
    AcBack                      = 0x224,
    AcForward                   = 0x225,
    AcStop                      = 0x226,
    AcRefresh                   = 0x227,
    AcPreviousLink              = 0x228,
    AcNextLink                  = 0x229,
    AcBookmarks                 = 0x22A,
    AcHistory                   = 0x22B,
    AcSubscriptions             = 0x22C,
    AcZoomIn                    = 0x22D,
    AcZoomOut                   = 0x22E,
    AcZoom                      = 0x22F,
    AcFullScreenView            = 0x230,
    AcNormalView                = 0x231,
    AcViewToggle                = 0x232,
    AcScrollUp                  = 0x233,
    AcScrollDown                = 0x234,
    AcScroll                    = 0x235,
    AcPanLeft                   = 0x236,
    AcPanRight                  = 0x237,
    AcPan                       = 0x238,
    AcNewWindow                 = 0x239,
    AcTileHorizontally          = 0x23A,
    AcTileVertically            = 0x23B,
    AcFormat                    = 0x23C,
    AcEdit                      = 0x23D,
    AcBold                      = 0x23E,
    AcItalics                   = 0x23F,
    AcUnderline                 = 0x240,
    AcStrikethrough             = 0x241,
    AcSubscript                 = 0x242,
    AcSuperScript               = 0x243,
    AcAllCaps                   = 0x244,
    AcRotate                    = 0x245,
    AcResize                    = 0x246,
    AcFlipHorizontal            = 0x247,
    AcFlipVertical              = 0x248,
    AcMirrorHorizontal          = 0x249,
    AcMirrorVertical            = 0x24A,
    AcFontSelect                = 0x24B,
    AcFontColor                 = 0x24C,
    AcFontSize                  = 0x24D,
    AcJustifyLeft               = 0x24E,
    AcJustofyCenterH            = 0x24F,
    AcJustifyRight              = 0x250,
    AcJustifyBlockH             = 0x251,
    AcJustifyTop                = 0x252,
    AcJustifyCenterV            = 0x253,
    AcJustifyBottom             = 0x254,
    AcJustifyBlockV             = 0x255,
    AcIndentDecrease            = 0x256,
    AcIndentIncrease            = 0x257,
    AcNumberedList              = 0x258,
    AcRestartNumbering          = 0x259,
    AcBulletedList              = 0x25A,
    AcPromote                   = 0x25B,
    AcDemote                    = 0x25C,
    AcYes                       = 0x25D,
    AcNo                        = 0x25E,
    AcCancel                    = 0x25F,
    AcCatalog                   = 0x260,
    AcBuyCheckout               = 0x261,
    AcAddToCart                 = 0x262,
    AcExpand                    = 0x263,
    AcExpandAll                 = 0x264,
    AcCollapse                  = 0x265,
    AcCollapseAll               = 0x266,
    AcPrintPreview              = 0x267,
    AcPasteSpecial              = 0x268,
    AcInsertMode                = 0x269,
    AcDelete                    = 0x26A,
    AcLock                      = 0x26B,
    AcUnlock                    = 0x26C,
    AcProtect                   = 0x26D,
    AcUnprotect                 = 0x26E,
    AcAttachComment             = 0x26F,
    AcDeleteComment             = 0x270,
    AcViewComment               = 0x271,
    AcSelectWord                = 0x272,
    AcSelectInstance            = 0x273,
    AcSelectParagraph           = 0x274,
    AcSelectColumn              = 0x275,
    AcSelectRow                 = 0x276,
    AcSelectTable               = 0x277,
    AcSelectObject              = 0x278,
    AcRedoRepeat                = 0x279,
    AcSort                      = 0x27A,
    AcSortAscending             = 0x27B,
    AcSortDescending            = 0x27C,
    AcFilter                    = 0x27D,
    AcSetClock                  = 0x27E,
    AcViewClock                 = 0x27F,
    AcSelectTimeZone            = 0x280,
    AcEditTimeZones             = 0x281,
    AcSetAlarm                  = 0x282,
    AcClearAlarm                = 0x283,
    AcSnoozeAlarm               = 0x284,
    AcResetAlarm                = 0x285,
    AcSynchronize               = 0x286,
    AcSendReceive               = 0x287,
    AcSendTo                    = 0x288,
    AcReply                     = 0x289,
    AcReplyAll                  = 0x28A,
    AcForwardMsg                = 0x28B,
    AcSend                      = 0x28C,
    AcAttachFile                = 0x28D,
    AcUpload                    = 0x28E,
    AcDownload                  = 0x28F,
    AcSetBorders                = 0x290,
    AcInsertRow                 = 0x291,
    AcInsertColumn              = 0x292,
    AcInsertFile                = 0x293,
    AcInsertPicture             = 0x294,
    AcInsertObject              = 0x295,
    AcInsertSymbol              = 0x296,
    AcSaveAndClose              = 0x297,
    AcRename                    = 0x298,
    AcMerge                     = 0x299,
    AcSplit                     = 0x29A,
    AcDistributeHorizontally    = 0x29B,
    AcDistributeVertically      = 0x29C
} ConsumerPage;

typedef enum
{
    Digitizer                   = 0x01,
    Pen                         = 0x02,
    LightPen                    = 0x03,
    TouchScreen                 = 0x04,
    TouchPad                    = 0x05,
    WhiteBoard                  = 0x06,
    CoordinateMeasuringMachine  = 0x07,
    _3dDigitizer                = 0x08,
    StereoPlotter               = 0x09,
    ArticulatedArm              = 0x0A,
    Armature                    = 0x0B,
    MultiplePointDigitizer      = 0x0C,
    FreeSpaceWand               = 0x0D,
    Stylus                      = 0x20,
    Puck                        = 0x21,
    Finger                      = 0x22,
    TipPressure                 = 0x30,
    BarrelPressure              = 0x31,
    InRange                     = 0x32,
    Touch                       = 0x33,
    UnTouch                     = 0x34,
    Tap                         = 0x35,
    Quality                     = 0x36,
    DataValid                   = 0x37,
    TransducerIndex             = 0x38,
    TabletFunctionKeys          = 0x39,
    ProgramChangeKeys           = 0x3A,
    BatteryStrengthDigitizer    = 0x3B,
    Invert                      = 0x3C,
    XTilt                       = 0x3D,
    YTilt                       = 0x3E,
    Azimuth                     = 0x3F,
    Altitude                    = 0x40,
    Twist                       = 0x41,
    TipSwitch                   = 0x42,
    SecondaryTipSwitch          = 0x43,
    BarrelSwitch                = 0x44,
    Eraser                      = 0x45,
    TabletPick                  = 0x46
} Digitizers;

typedef enum
{
    PhysicalInterfaceDevice     = 0x01,
    Normal                      = 0x20,
    SetEffectReport             = 0x21,
    EffectBlockIndex            = 0x22,
    ParameterBlockOffset        = 0x23,
    RomFlag                     = 0x24,
    EffectType                  = 0x25,
    EtConstantForce             = 0x26,
    EtRamp                      = 0x27,
    EtCustomForceData           = 0x28,
    EtSquare                    = 0x30,
    EtSine                      = 0x31,
    EtTriangle                  = 0x32,
    EtSawtoothUp                = 0x33,
    EtSawtoothDown              = 0x34,
    EtSpring                    = 0x40,
    EtDamper                    = 0x41,
    EtInertia                   = 0x42,
    EtFunction                  = 0x43,
    Duration                    = 0x50,
    SamplePeriod                = 0x51,
    Gain                        = 0x52,
    TriggerButton               = 0x53,
    TriggerRepeatInterval       = 0x54,
    AxesEnable                  = 0x55,
    DirectionEnable             = 0x56,
    Direction                   = 0x57,
    TypeSpecificBlockOffset     = 0x58,
    BlockType                   = 0x59,
    SetEnvelopeReport           = 0x5A,
    AttackLevel                 = 0x5B,
    AttackTime                  = 0x5C,
    FadeLevel                   = 0x5D,
    FadeTime                    = 0x5E,
    SetConditionReport          = 0x5F,
    CpOffset                    = 0x60,
    PositiveCoefficient         = 0x61,
    NegativeCoefficient         = 0x62,
    PositiveSaturation          = 0x63,
    NegativeSaturation          = 0x64,
    DeadBand                    = 0x65,
    DownloadForceSample         = 0x66,
    IsochCustomForceEnable      = 0x67,
    CustomForceDataReport       = 0x68,
    CustomForceData             = 0x69,
    CustomForceVendorDefinedData= 0x6A,
    SetCustomForceReport        = 0x6B,
    CustomForceDataOffset       = 0x6C,
    SampleCount                 = 0x6D,
    SetPeriodicReport           = 0x6E,
    Offset                      = 0x6F,
    Magnitude                   = 0x70,
    Phase                       = 0x71,
    Period                      = 0x72,
    SetConstantForceReport      = 0x73,
    SetRampForceReport          = 0x74,
    RampStart                   = 0x75,
    RampEnd                     = 0x76,
    EffectOperationReport       = 0x77,
    EffectOperation             = 0x78,
    OpEffectStart               = 0x79,
    OpEffectStartSolo           = 0x7A,
    OpEffectStop                = 0x7B,
    LoopCount                   = 0x7C,
    DeviceGainReport            = 0x7D,
    DeviceGain                  = 0x7E,
    PidPoolReport               = 0x7F,
    RamPoolSize                 = 0x80,
    RomPoolSize                 = 0x81,
    RomEffectBlockCount         = 0x82,
    SimultaneousEffectsMax      = 0x83,
    PoolAlignment               = 0x84,
    PidPoolMoveReport           = 0x85,
    MoveSource                  = 0x86,
    MoveDestination             = 0x87,
    MoveLength                  = 0x88,
    PidBlockDataReport          = 0x89,
    BlockLoadStatus             = 0x8B,
    BlockLoadSuccess            = 0x8C,
    BlockLoadFull               = 0x8D,
    BlockLoadError              = 0x8E,
    BlockHandle                 = 0x8F,
    PidBlockFreeReport          = 0x90,
    TypeSpecificBlockHandle     = 0x91,
    PidStateReport              = 0x92,
    EffectPlaying               = 0x94,
    PidDeviceControlReport      = 0x95,
    PidDeviceControl            = 0x96,
    DcEnableActuators           = 0x97,
    DcDisableActuators          = 0x98,
    DcStopAllEffects            = 0x99,
    DcDeviceReset               = 0x9A,
    DcDevicePause               = 0x9B,
    DcDeviceContinue            = 0x9C,
    DevicePaused                = 0x9F,
    ActuatorsEnabled            = 0xA0,
    SafetySwitch                = 0xA4,
    ActuatorOverrideSwitch      = 0xA5,
    ActuatorPower               = 0xA6,
    StartDelay                  = 0xA7,
    ParameterBlockSize          = 0xA8,
    DeviceManagedPool           = 0xA9,
    SharedParameterBlocks       = 0xAA,
    CreateNewEffectReport       = 0xAB,
    RamPoolAvailable            = 0xAC
} PID_PageUsage;


#pragma pack(pop)
    
}    
}

#endif