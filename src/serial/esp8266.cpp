#include "esp8266.h"

using namespace Serial;

ESP8266::ESP8266(Usart *usart, Gpio::PinName resetPin) :
    mUsart(usart),
    mTransparentMode(false),
    mState(ResetState),
    mLastCmd(cmdNone),
    mApSSID("stm32wifi")
{
    stmApp()->registerTaskEvent(EVENT(&ESP8266::task));
  
    mResetPin = new Gpio(resetPin);
    mResetPin->setAsOutput();
    mResetPin->write(0);
  
    mUsart->setBaudrate(115200);//78400);
    mUsart->setConfig(Usart::Mode8N1);
    mUsart->setBufferSize(256); // 1024 for 3 ms!!
    mUsart->setUseDmaRx(true);
    mUsart->setUseDmaTx(true);
    mUsart->setLineEnd("\r\n");
    mUsart->open(ReadWrite);
    
    for (int i=0; i<10000; i++);
    mResetPin->setAsInputPullUp();
    
    mTimer.setTimeoutEvent(EVENT(&ESP8266::onTimer));
    mTimer.start(500);
}

void ESP8266::task()
{
    if (!mTransparentMode)
    {
        if (mUsart->canReadLine())
        {
            ByteArray line;
            mUsart->readLine(line);
            if (line.size() >= 2)
            {
                line.resize(line.size() - 2);
                parseLine(line);
            }
        }
    }
}

void ESP8266::parseLine(ByteArray &line)
{
    if (line.size() >= 2 && line[0] == 'A' && line[1] == 'T')
    {
        // this is EEECHOOOO!!
    }
    else if (line == "ready")
    {
        mState = ReadyState;
        mTransparentMode = true;
    }
    else if (line == "OK")
    {
        switch (mLastCmd)
        {
          case cmdReset:
            mTransparentMode = false;
            mState = ResetState;
            mUsart->setBaudrate(1000000);
            break;
                
          case cmdUart:
            mUsart->setBaudrate(mLastData);
            break;
                
          case cmdCwMode:
            {
                char cmd[64];
                mLastCmd = cmdSetAp;
                if (mApKey.length())
                    sprintf(cmd, "AT+CWSAP_CUR=\"%s\",\"%s\",5,3", mApSSID.c_str(), mApKey.c_str()); 
                else
                    sprintf(cmd, "AT+CWSAP_CUR=\"%s\",\"\",5,0", mApSSID.c_str()); 
                sendCmd(cmd);
            }
            break;
            
          case cmdSetAp:
            mLastCmd = cmdIpMode;
            sendCmd("AT+CIPMODE=1");
            break;
            
          case cmdIpMode:
            mState = WaitForConnect;
            break;
            
          case cmdListIp:
            if (!mPeerIp.empty())
            {
                mState = Connecting;
                mLastCmd = cmdIpStart;
                char cmd[64];
                sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",51966,10", mPeerIp.c_str());
                sendCmd(cmd);
            }
            break;
            
          case cmdIpStart:
            mLastCmd = cmdIpSend;
            sendCmd("AT+CIPSEND");
            mState = ReadyState;
            break;
            
          case cmdIpSend:
            mTransparentMode = true;
            break; 
            
          case cmdSaveTransLink:
            hardReset();
            break;
            
          case cmdConnectToAp:
            hardReset();
            break;
        }
        
        if (onOK)
            onOK();
    }
    else if (line == "ERROR")
    {
        if (mLastCmd == cmdIpStart)
        {
            char cmd[64];
            sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",51966,10", mPeerIp.c_str());
            sendCmd(cmd);
        }
        
        if (onError)
            onError();
    }
    else if (line == "CLOSED")
    {
        if (mTransparentMode)
            mTransparentMode = false;
    }
    else if (mState == WaitForConnect && line.size() > 7)
    {
        for (int i=0; i<line.size(); i++)
        {
            if (line[i] == ',')
            {
                line.resize(i);
                break;
            }
        }
        mPeerIp = string(line.data(), line.size());
    }
}

void ESP8266::sendCmd(ByteArray ba)
{
    ba.append(0x0d);
    ba.append(0x0a);
    mUsart->write(ba);
}

void ESP8266::onTimer()
{
    if (mState == WaitForConnect)
    {
        mLastCmd = cmdListIp;
        sendCmd("AT+CWLIF");
    }
}
//---------------------------------------------------------------------------

void ESP8266::hardReset()
{
    mState = ResetState;
    mTransparentMode = false;
    mResetPin->setAsOutput();
    mResetPin->write(0);
    mUsart->setBaudrate(1000000);
    for (int i=0; i<10000; i++);
    mResetPin->setAsInputPullUp();
}
//---------------------------------------------------------------------------

int ESP8266::read(ByteArray &ba)
{
    if (mTransparentMode)
        return mUsart->read(ba);
    return -1;
}

int ESP8266::write(const ByteArray &ba)
{
    if (mTransparentMode)
        return mUsart->write(ba);
    return -1;
}
//---------------------------------------------------------------------------

void ESP8266::interruptTransparentMode()
{
    mUsart->write("+++");
    for (int i=0; i<1000000; i++);
        mTransparentMode = false;
}

void ESP8266::reset()
{
    sendCmd("AT+RST");
    mLastCmd = cmdReset;
}

void ESP8266::setBaudrate(int baudrate)
{
    if (isReady() && !isOpen())
    {
        char cmd[64];
        sprintf(cmd, "AT+UART_CUR=%d,8,1,0,0", baudrate);
        sendCmd(cmd);
        mLastCmd = cmdUart;
        mLastData = baudrate;
    }
}

void ESP8266::setAPMode(string ssid, string pass)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mApSSID = ssid;
    mApKey = pass;
    mLastCmd = cmdCwMode;
    sendCmd("AT+CWMODE_CUR=2");    
}

void ESP8266::saveTransLink(string translink_string)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mLastCmd = cmdSaveTransLink;
    string cmd = "AT+SAVETRANSLINK=" + translink_string;
    sendCmd(cmd.c_str());
}

void ESP8266::setOnbStaMode(string autoConnIp)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mLastCmd = cmdSaveTransLink;
    string cmd = "AT+SAVETRANSLINK=1,\"" + autoConnIp + "\",51966,\"TCP\",10";
    sendCmd(cmd.c_str());
}

void ESP8266::autoConnectToAp(string ssid_and_pass)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mLastCmd = cmdConnectToAp;
    string cmd = "AT+CWJAP_DEF=" + ssid_and_pass;
    sendCmd(cmd.c_str());
}
//---------------------------------------------------------------------------