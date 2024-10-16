#ifndef _ESP8266_H
#define _ESP8266_H

#include "usart.h"
#include "gpio.h"
#include "core/timer.h"
#include <string>

namespace Serial
{

using namespace std;

class ESP8266 : public Device
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
        cmdSetEcho,
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
        cmdCwjap,
        cmdCipsta,
        cmdCipAp
    } Command;

    typedef enum
    {
        wmUnknown=0,
        wmSta=1,
        wmAp=2,
        wmApSta=3
    } WirelessMode;

    bool mTransparentMode;
    bool mConnected;
    int mReceiveSize;
    int mTransmitSize;
    bool mServerActive;
    WirelessMode mWirelessMode;
    int mBaudrate;
    State mState;
    Command mLastCmd;
    int mLastData;
    int mTimeout;
    int mIpMode;

    int mDefMode;
    string mStaSSID, mStaPass;
    string mStaIp, mStaGateway, mStaNetmask;

    ByteArray mOutBuffer, mInBuffer, mLineBuffer;

    string mApSSID, mApKey;
    string mPeerIp;
    unsigned short mServerPort;

    void task();
    void parseLine(ByteArray &line);
    void onTimer();

    void cipStart(const char *host, unsigned short port);


public:
    ESP8266(Usart *usart, Gpio::PinName resetPin);

    string autoSSID, autoKey;

    void hardReset();
    void sendCmd(ByteArray ba);

//    int read(ByteArray &ba);
//    int write(const ByteArray &ba);

    int bytesAvailable() const;
    int readData(char *data, int size);
    int writeData(const char *data, int size);

    void sendLine(string line);

    bool isReady() const {return mState == ReadyState;}
    bool isWaiting() const {return mState == WaitForConnect;}
    bool isIdle() const {return mState == IdleState;}
    bool isConnecting() const {return mState == Connecting;}
    bool isOpen() const {return mTransparentMode || mConnected;}

    bool isStaMode() const {return mWirelessMode & wmSta;}
    bool isApMode() const {return mWirelessMode & wmAp;}

    void interruptTransparentMode();
    void reset();
    void setBaudrate(int baudrate);
    void setDefaultBaudrate(int baudrate) {mBaudrate = baudrate;}
    void setEchoEnabled(bool enabled);
    void setAPMode(string ssid, string pass="");
    void setAP_IP(string ip);
    void setStationMode(string ssid, string pass="");
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
