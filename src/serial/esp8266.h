#ifndef _ESP8266_H
#define _ESP8266_H

#include "serialinterface.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include <string>

namespace Serial
{

class ESP8266 : public SerialInterface
{
private:
    Usart *mUsart;
    Gpio *mResetPin;
    Timer mTimer;

    typedef enum
    {
        ResetState = 0,
        ReadyState,
        WaitForConnect,
        Connecting
    } State;
    
    typedef enum
    {
        cmdNone,
        cmdEcho,
        cmdReset,
        cmdUart,
        cmdCwMode,
        cmdSetAp,
        cmdIpMode,
        cmdListIp,
        cmdIpStart,
        cmdIpSend,
        cmdSaveTransLink,
        cmdConnectToAp
    } Command;
    
    bool mTransparentMode;
    State mState;
    Command mLastCmd;
    int mLastData;
    
    string mApSSID, mApKey;
    string mPeerIp;
    
    void task();
    void parseLine(ByteArray &line);
    void onTimer();
  
public:
    ESP8266(Usart *usart, Gpio::PinName resetPin);
    
    string autoSSID, autoKey;
    
    void hardReset();
    void sendCmd(ByteArray ba);
    
    int read(ByteArray &ba);
    int write(const ByteArray &ba);
    
    bool isOpen() const {return mTransparentMode;}
    bool isReady() const {return mState == ReadyState;}
    
    void interruptTransparentMode();
    void reset();
    void setBaudrate(int baudrate);
    void setAPMode(string ssid, string pass="");
    void saveTransLink(string translink_string);
    void setOnbStaMode(string autoConnIp);
    void autoConnectToAp(string ssid_and_pass);
    
    NotifyEvent onOK;
    NotifyEvent onError;
};

}

#endif
