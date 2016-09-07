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
        IdleState,
        Connecting,
        Connected
    } State;
    
    typedef enum
    {
        cmdNone,
        cmdEcho,
        cmdReset,
        cmdUart,
        cmdCwMode,
        cmdCwModeReq,
        cmdSetAp,
        cmdIpMode,
        cmdListIp,
        cmdIpStart,
        cmdIpSend,
        cmdIpSendBuf,
        cmdSaveTransLink,
        cmdConnectToAp,
        cmdIpMux,
        cmdIpServer,
    } Command;
    
    bool mTransparentMode;
    bool mConnected;
    int mReceiveSize;
    int mTransmitSize;
    bool mServerActive;
    bool mApMode;
    State mState;
    Command mLastCmd;
    int mLastData;
    
    ByteArray mOutBuffer, mInBuffer, mLineBuffer;
    
    string mApSSID, mApKey;
    string mPeerIp;
    unsigned short mServerPort;
    
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
    
    void sendLine(string line);
    
    bool isReady() const {return mState == ReadyState;}
    bool isWaiting() const {return mState == WaitForConnect;}
    bool isIdle() const {return mState == IdleState;}
    bool isConnecting() const {return mState == Connecting;}
    bool isOpen() const {return mTransparentMode || mConnected;}
    
    void interruptTransparentMode();
    void reset();
    void setBaudrate(int baudrate);
    void setAPMode(string ssid, string pass="");
    void saveTransLink(string translink_string);
    void setOnbStaMode(string autoConnIp);
    void autoConnectToAp(string ssid_and_pass);
    void startServer(unsigned short port);
    void connectToHost(string ip, unsigned short port);
    void autoConnectToHost(string ip, unsigned short port);
    
    NotifyEvent onOK;
    NotifyEvent onError;
    NotifyEvent onReady;
    
    Closure<void(string)> onReceiveLine;
};

}

#endif
