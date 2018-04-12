#include "esp8266.h"

using namespace Serial;

ESP8266::ESP8266(Usart *usart, Gpio::PinName resetPin) :
    mUsart(usart),
    mTransparentMode(false),
    mConnected(false),
    mReceiveSize(0), mTransmitSize(0),
    mServerActive(false),
    mWirelessMode(wmUnknown),
    mBaudrate(460800),
    mState(ResetState),
    mLastCmd(cmdNone),
    mTimeout(0),
    mDefMode(0),
    mApSSID("stm32wifi"),
    mServerPort(0),
    mIpMode(0)
{
    stmApp()->registerTaskEvent(EVENT(&ESP8266::task));
  
    mResetPin = new Gpio(resetPin);
    mResetPin->setAsOutput();
    mResetPin->write(0);
  
    mUsart->setBaudrate(78400);
    mUsart->setConfig(Usart::Mode8N1);
    mUsart->setBufferSize(1024); // 1024 for 3 ms!!
//#if defined(STM32F37X)     
//    mUsart->setUseDmaRx(false);
//    mUsart->setUseDmaTx(false);
//#endif
    mUsart->setLineEnd("\r\n");
    mUsart->open(ReadWrite);
    
    for (int i=0; i<10000; i++);
    mResetPin->write(1);
    
    mTimer.setTimeoutEvent(EVENT(&ESP8266::onTimer));
    mTimer.start(1000);
}

void ESP8266::task()
{
    if (!mTransparentMode)
    {
        if (mConnected)
        {
            mUsart->read(mLineBuffer);
            while (mLineBuffer.size())
            {
                if (!mReceiveSize)
                {
                    if (mLineBuffer.startsWith("+IPD"))
                    {
                        int cidx1=0, cidx2=0;
                        for (int i=4; i<12 && !cidx2; i++)
                        {
                            if (mLineBuffer.size() <= i)
                                break;
                            if (mLineBuffer[i] == ',')
                                cidx1 = i+1;
                            else if (mLineBuffer[i] == ':')
                                cidx2 = i+1;
                        }
                        if (cidx2)
                        {
                            sscanf(mLineBuffer.data()+cidx1, "%d", &mReceiveSize);
                            mLineBuffer.remove(0, cidx2);
                        }
                        else
                        {
                            break;
                        }
                        
                        if (mTransmitSize && mLastCmd == cmdIpSendBuf)
                        {
                            char cmd[32];
                            printf("send req repeat, sz=%d\n", mTransmitSize);
                            sprintf(cmd, "AT+CIPSENDBUF=%d", mTransmitSize);
                            sendCmd(cmd);
                        }
                        
                    }
                    else if (mLineBuffer.startsWith('>'))
                    {
//                        printf((string(mLineBuffer.data(), mLineBuffer.size()) + "\n").c_str());
                        mLineBuffer.clear();
                        if (mTransmitSize)
                            mUsart->write(mOutBuffer.data(), mTransmitSize);
                        mOutBuffer.remove(0, mTransmitSize);
                        mTransmitSize = 0;
                    }
                    else
                    {
                        int leidx = mLineBuffer.indexOf(mUsart->lineEnd());
                        if (leidx == -1)
                            break;
                        
                        ByteArray line = mLineBuffer.left(leidx);
                        mLineBuffer.remove(0, leidx+2);
                        if (leidx)
                        {
                            parseLine(line);
//                            printf((string(line.data(), line.size()) + "\n").c_str());
                        }
                    }
                }
                if (mReceiveSize)
                {
                    if (mLineBuffer.size() >= mReceiveSize)
                    {
                        mInBuffer.append(mLineBuffer.data(), mReceiveSize);
                        mLineBuffer.remove(0, mReceiveSize);
                        mReceiveSize = 0;
                    }
                    else
                    {
                        mInBuffer.append(mLineBuffer);
                        mReceiveSize -= mLineBuffer.size();
                        mLineBuffer.clear();
                    }
                }
            }
        }
        else if (mUsart->canReadLine())
        {
            ByteArray line;
            mUsart->readLine(line);
            if (line.size() > 2)
            {
                line.resize(line.size() - 2);
                parseLine(line);
            }
        }
    }
}

void ESP8266::parseLine(ByteArray &line)
{
//    if (onReceiveLine)
//        onReceiveLine(string(line.data(), line.size()));
  
    if (line.size() >= 2 && line[0] == 'A' && line[1] == 'T')
    {
        // this is EEECHOOOO!!
    }
    else if (line == ">" && mState != ReadyState)
    {
        mTransparentMode = true;
        mState = Connected;
    }
    else if (line == "ready")
    {
        //setBaudrate(mBaudrate);
        mState = ReadyState;
        mLastCmd = cmdNone;
        if (onReady)
            onReady();
    }
    else if (line == "OK")
    {
        switch (mLastCmd)
        {
          case cmdReset:
            mTransparentMode = false;
            mState = ResetState;
            break;
                
          case cmdUart:
            mBaudrate = mLastData;
            mUsart->setBaudrate(mBaudrate);
            mLastCmd = cmdNone;
            //setEchoEnabled(false);
            break;
            
          case cmdEcho:
            mLastCmd = cmdNone;
            break;
            
          case cmdSetEcho:
            mLastCmd = cmdNone;
            break;
                
          case cmdCwMode:
            if (mWirelessMode != mDefMode)
            {
                mLastCmd = cmdCwModeReq;
                sendCmd("AT+CWMODE?");
            }
            break;
            
          case cmdCwModeReq:
            if (mDefMode)
            {
                if (mWirelessMode != mDefMode)
                {
                    char cmd[32];
                    mLastCmd = cmdCwMode;
                    sprintf(cmd, "AT+CWMODE_CUR=%d", mDefMode);
                    sendCmd(cmd);
                    mWirelessMode = wmUnknown;
                    break;
                }
            }
            if (mWirelessMode == wmUnknown)
            {
                sendCmd("AT+CWMODE?");
                break;
            }            
            else if (isApMode())
            {
                char cmd[64];
                mLastCmd = cmdSetAp;
                if (mApKey.length())
                    sprintf(cmd, "AT+CWSAP_CUR=\"%s\",\"%s\",5,3", mApSSID.c_str(), mApKey.c_str()); 
                else
                    sprintf(cmd, "AT+CWSAP_CUR=\"%s\",\"\",5,0", mApSSID.c_str()); 
                sendCmd(cmd);
                mState = WaitForConnect;
            }
            else if (mWirelessMode == wmSta)
            {
                string cmd = "AT+CWJAP_CUR=\""+mStaSSID+"\",\""+mStaPass+"\"";
                mLastCmd = cmdCwjap;
                sendCmd(cmd.c_str());
                mState = WaitForConnect;
            }
            break;
            
          case cmdSetAp:
//            mLastCmd = cmdIpMode;
//            sendCmd("AT+CIPMODE=1");
            break;
            
          case cmdIpMode:
            mState = WaitForConnect;
            if (1) // CIPMODE == 1
                mIpMode = 1;
            break;
            
          case cmdListIp:
            if (!mPeerIp.empty())
            {
                mState = IdleState;
                if (mServerPort)
                {
                    mState = Connecting;
                    cipStart(mPeerIp.c_str(), mServerPort);
                }
            }
            break;
            
          case cmdIpStart:
            mLastCmd = cmdNone;
            break;
            
          case cmdIpSend:
            if (mConnected)
            {
                mUsart->write(mOutBuffer);
                mOutBuffer.remove(0, mLastData);
                mLastCmd = cmdNone;
            }
            else
            {
                mTransparentMode = true;
            }
            break; 
            
          case cmdIpSendBuf:
            mLastCmd = cmdNone;
            break;
            
          case cmdSaveTransLink:
            hardReset();
            break;
            
          case cmdConnectToAp:
            hardReset();
            break;
            
          case cmdIpMux:
          {
            char cmd[32];
            sprintf(cmd, "AT+CIPSERVER=1,%d", mServerPort);
            mLastCmd = cmdIpServer;
            sendCmd(cmd);
          }
            break;
            
          case cmdIpServer:
            mServerActive = true;
            mLastCmd = cmdNone;
            break;
            
          case cmdCwjap:
            mLastCmd = cmdCipsta;
            sendCmd("AT+CIPSTA?");
            break;
            
          case cmdCipsta:
            mLastCmd = cmdNone;
            if (mServerPort)
            {
                if (mPeerIp.empty())
                    mPeerIp = mStaGateway;
                mState = Connecting;
            }
            break;
        }
        
        if (onOK)
            onOK();
    }
    else if (line == "FAIL")
    {
        mLastCmd = cmdNone;
    }
    else if (line == "ERROR")
    {
        if (mLastCmd == cmdIpStart)// && mState == Connecting)
        {
            mLastCmd = cmdNone;
//            char cmd[64];
//            sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", mPeerIp.c_str(), mServerPort);
//            sendCmd(cmd);
        }
        
        if (onError)
            onError();
    }
    else if (line == "CONNECT" || line == "ALREADY CONNECTED")
    {
        mConnected = true;
        mState = Connected;
        if (mIpMode == 1)
            mTransparentMode = true;
    }
    else if (line == "CLOSED" || line == "CONNECT FAIL")
    {
        if (mTransparentMode)
            mTransparentMode = false;
        mConnected = false;
        if (mServerPort)
            mState = Connecting;
        else
            mState = IdleState;
    }
    else if (line.startsWith("WIFI"))
    {
        if (line == "WIFI CONNECTED")
        {
            mState = IdleState;
        }
        else if (line == "WIFI DISCONNECT")
        {
            mState = WaitForConnect;
        }
        else if (line == "WIFI GOT IP")
        {
            if (!mLastCmd)
            {
                mLastCmd = cmdCipsta;
                sendCmd("AT+CIPSTA?");
            }
        }
    }
    else if (line.startsWith("+CWMODE") && line.size() >= 9)
    {
        mWirelessMode = (WirelessMode)(line[8] - '0');
    }
    else if (line.startsWith("+CIPSTA"))
    {
        ByteArray ba = line.mid(8);
        int idx1 = ba.indexOf('\"') + 1;
        int idx2 = ba.indexOf('\"', idx1) - idx1;
        if (ba.startsWith("ip"))
            mStaIp = string(ba.data() + idx1, idx2);
        else if (ba.startsWith("gateway"))
            mStaGateway = string(ba.data() + idx1, idx2);
        else if (ba.startsWith("netmask"))
            mStaNetmask = string(ba.data() + idx1, idx2);
    }
    else if (mUsart->baudrate() == 78400 && line.size() >= 4)
    {
        if (line[0]=='j' && line[1]=='u' && line[2]=='m' && line[3]=='p')
            mUsart->setBaudrate(mBaudrate);
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
    else if (mServerActive)
    {
        ByteArray cc = line.remove(0, 2);
        if (cc == "CONNECT")
        {
            mConnected = true;
        }
        else if (cc == "CLOSED" || cc == "CONNECT FAIL")
        {
            mConnected = false;
        }
    }
    
    if (onReceiveLine)
        onReceiveLine(string(line.data(), line.size()));
}

void ESP8266::sendCmd(ByteArray ba)
{
    ba.append(0x0d);
    ba.append(0x0a);
    mUsart->write(ba);
}

void ESP8266::sendLine(string line)
{
    sendCmd(line.c_str());
}

void ESP8266::onTimer()
{
    if (mLastCmd == cmdSetEcho)
    {
        setEchoEnabled(mLastData);
        return;
    }
    if (mLastCmd == cmdUart)
    {
        setBaudrate(mLastData);
        return;
    }
//    if (mLastCmd == cmdIpSend)
//    {
//        mUsart->write(mOutBuffer);
//        mOutBuffer.remove(0, mLastData);
//        mLastCmd = cmdNone;
//    }
  
    if (mState == ResetState)
    {
        hardReset();
    }
    else if (mState == ReadyState)
    {
        if (mLastCmd == cmdCwModeReq)
        {
            mTimeout++;
            if (mTimeout >= 2)
            {
                mTimeout = 0;
                mLastCmd = cmdNone;
            }
        }
        
        if (mLastCmd == cmdNone)
        {
            mLastCmd = cmdCwModeReq;
            sendCmd("AT+CWMODE?");
        }
    }
    else if (mState == WaitForConnect)
    {
        if (mWirelessMode == wmAp)
        {
            mLastCmd = cmdListIp;
            sendCmd("AT+CWLIF");
        }
        else if (mWirelessMode == wmSta && mLastCmd == cmdNone)
        {
            string cmd = "AT+CWJAP_CUR=\""+mStaSSID+"\",\""+mStaPass+"\"";
            mLastCmd = cmdCwjap;
            sendCmd(cmd.c_str());
        }
    }
    else if (mState == Connecting)
    {
        if (mLastCmd == cmdIpStart)
        {
            mTimeout++;
            if (mTimeout >= 5)
            {
                mTimeout = 0;
                mLastCmd = cmdNone;
            }
        }
        else if (!mPeerIp.empty())
        {
            mTimeout = 0;
            cipStart(mPeerIp.c_str(), mServerPort);
        }
    }
}
//---------------------------------------------------------------------------

void ESP8266::hardReset()
{
    mState = ResetState;
    mTransparentMode = false;
    mResetPin->write(0);
    mUsart->setBaudrate(78400);
    for (int i=0; i<10000; i++);
    mResetPin->write(1);
}
//---------------------------------------------------------------------------

int ESP8266::read(ByteArray &ba)
{
    if (mTransparentMode)
    {
        int result = mUsart->read(ba);
//        if (ba.size())
//            printf((string(ba.data(), ba.size()) + "\n").c_str());
        return result;
    }
    else if (mConnected)
    {
        int cnt = mInBuffer.size();
        ba.append(mInBuffer);
        mInBuffer.clear();
        return cnt;
    }
    return 0;
}

int ESP8266::write(const ByteArray &ba)
{
    if (mTransparentMode)
        return mUsart->write(ba);
    else if (mConnected)
    {
        bool fuckflag = false;
        if (mOutBuffer.size() + ba.size() > 2048)
        {
            mTransmitSize = 0;
            mOutBuffer.clear();
            fuckflag = true;
            printf("fuck...\n");
        }
        else
        {
            mOutBuffer.append(ba);
        }
        
//        printf("write sz=%d\n", ba.size());
        
        char cmd[32];
        if (mServerActive)
        {
            mLastCmd = cmdIpSend;
            mTransmitSize = mOutBuffer.size();
            sprintf(cmd, "AT+CIPSEND=0,%d", mTransmitSize);
            sendCmd(cmd);
        }
        else if (!mTransmitSize)
        {
            mLastCmd = cmdIpSendBuf;
            mTransmitSize = mOutBuffer.size();
//            printf("send request, sz=%d\n", mTransmitSize);
            sprintf(cmd, "AT+CIPSENDBUF=%d", mTransmitSize);
            sendCmd(cmd);
        }
        return fuckflag? 0: ba.size();
    }
    return 0;
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

void ESP8266::setEchoEnabled(bool enabled)
{
    mLastCmd = cmdSetEcho;
    mLastData = enabled;
    if (enabled)
        sendCmd("ATE1");
    else
        sendCmd("ATE0");
}

void ESP8266::setAPMode(string ssid, string pass)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mApSSID = ssid;
    mApKey = pass;
    mDefMode = wmAp;
//    mLastCmd = cmdCwMode;
//    sendCmd("AT+CWMODE_CUR=2");    
}

void ESP8266::setStationMode(string ssid, string pass)
{
    mDefMode = 1;
    mStaSSID = ssid;
    mStaPass = pass;
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

void ESP8266::startServer(unsigned short port)
{
    if (mTransparentMode)
        interruptTransparentMode();
    mServerPort = port;
    mLastCmd = cmdIpMux;
    sendCmd("AT+CIPMUX=1");
}

void ESP8266::connectToHost(string ip, unsigned short port)
{
    if (mTransparentMode)
        interruptTransparentMode();
    char buf[64];
    sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip.c_str(), port);
    mLastCmd = cmdIpStart;
    sendCmd(buf);
}

void ESP8266::autoConnectToHost(string ip, unsigned short port)
{
//    if (mTransparentMode)
//        interruptTransparentMode();
//    if (!ip.empty())
        mPeerIp = ip;
//    if (port)
        mServerPort = port;
//    if (mState == IdleState)
//        mState = Connecting;
}
//---------------------------------------------------------------------------

void ESP8266::cipStart(const char *host, unsigned short port)
{
    char buf[64];
    sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d", host, port);
    mLastCmd = cmdIpStart;
    sendCmd(buf);
}