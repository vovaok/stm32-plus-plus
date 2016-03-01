#include "objnetNode.h"

using namespace Objnet;

ObjnetNode::ObjnetNode(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mNetState(netnStart),
    mNetTimeout(0),
    mCurrentRemoteAddress(0x00),
    mObjInfoSendCount(-1),
    mClass(0xFFFF0000),
    mName("<node>"),
    mFullName("<Generic objnet node>"),
    mSerial(0x00000000),
    mVersion(0x0100),
    mBurnCount(0)
{
    #ifdef __ICCARM__
    mTimer.setTimeoutEvent(EVENT(&ObjnetNode::onTimer));
    #else
    QObject::connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    #endif
    mTimer.start(200);

    #ifdef __ICCARM__
    mVersion = 0x0100;
    mBuildDate = string(__DATE__" "__TIME__);
    unsigned long *signature = reinterpret_cast<unsigned long*>(0x1FFF7A10);
    mSerial = signature[0] ^ signature[1] ^ signature[2];
    char tempstr[64];
    int flash = *reinterpret_cast<unsigned short*>(0x1FFF7A22);
    unsigned long cpuid = *reinterpret_cast<unsigned long*>(0xE0042000);
    unsigned short cpu = cpuid & 0xFFF;
    switch (cpu)
    {
        case 0x413: mCpuInfo = "STM32F40x"; break;
        default: mCpuInfo = "STM32 family";
    }
    sprintf(tempstr, " @ %d MHz, %dK flash", (int)(SystemCoreClock / 1000000), flash);
    mCpuInfo += string(tempstr);
    #endif
    
    registerSvcObject(ObjectInfo("class", mClass, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("name", mName));
    registerSvcObject(ObjectInfo("fullName", mFullName));
    registerSvcObject(ObjectInfo("serial", mSerial, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("version", mVersion, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("buildDate", mBuildDate, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("cpuInfo", mCpuInfo, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("burnCount", mBurnCount));
}
//---------------------------------------------------------------------------

void ObjnetNode::task()
{
    ObjnetCommonNode::task();

    switch (mNetState)
    {
      case netnStart:
//        mInterface->flush();
        mNetAddress = 0xFF;
        mTimer.stop();
        break;

      case netnConnecting:
        if (!mTimer.isActive())
        {
            ByteArray ba;
            ba.append(mBusAddress);
            sendServiceMessage(svcHello, ba);
            mTimer.start();
        }
        break;

      case netnDisconnecting:
        //mInterface->flush();
        mTimer.stop();
        break;

      case netnAccepted:
      {
        mTimer.stop();
        mNetState = netnReady;
        break;
      }

      case netnReady:
        if (!mTimer.isActive())
        {
            mTimer.start();
        }
        while (mObjInfoSendCount >= 0 && mObjInfoSendCount < mObjects.size())
        {
            ByteArray ba;
            mObjects[mObjInfoSendCount].mDesc.read(ba);
            bool success = sendServiceMessage(mCurrentRemoteAddress, svcObjectInfo, ba);
            if (success)
                mObjInfoSendCount++;
            else
                break;
        }
        break;      
    }
}
//---------------------------------------------------------------------------

void ObjnetNode::acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba)
{
//    #ifndef __ICCARM__
//    qDebug() << "node" << QString::fromStdString(mName) << "accept" << oid;
//    #endif

//    CommonMessage msg;
    switch (oid)
    {
      case svcHello: // translate hello msg
      {
        ba->append(mBusAddress);
        sendServiceMessage(svcHello, *ba);
        break;
      }

      case svcConnected:
      case svcDisconnected:
      case svcKill:
        sendServiceMessage(oid, *ba);
        break;

      default:;
    }
}

void ObjnetNode::parseServiceMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {        
        StdAID aid = (StdAID)msg.globalId().aid;

//        #ifndef __ICCARM__
//        qDebug() << "node" << QString::fromStdString(mName) << "global" << aid;
//        #endif

        switch (aid)
        {
          case aidPollNodes:
            if (isConnected())
            {
                sendServiceMessage(svcEcho);
                if (mNetState == netnReady)
                    mNetTimeout = 0;
            }
            else
            {
                mNetState = netnConnecting;
            }
            #ifdef __ICCARM__
            onPolling();
            #endif
            break;

          case aidConnReset:
//            mNetAddress = 0x00;
//            mNetState = netnConnecting;
            mNetState = netnStart;
            break;

          default:;
        }
        return;
    }

    SvcOID oid = (SvcOID)msg.localId().oid;
    unsigned char remoteAddr = msg.localId().sender;

//    #ifndef __ICCARM__
//    qDebug() << "node" << QString::fromStdString(mName) << "parse" << oid;
//    #endif

    switch (oid)
    {
      case svcHello:
//        mNetState = netnConnecting;
        mNetState = netnStart;
        mInterface->flush();
        break;

      case svcWelcome:
      case svcWelcomeAgain:
        if (msg.data().size() == 1)
        {
            mNetAddress = msg.data()[0];
            mNetState = netnAccepted;
            int len = mName.length();
            if (len > 8)
                len = 8;
            // send different info
            sendServiceMessage(remoteAddr, svcClass, ByteArray(reinterpret_cast<const char*>(&mClass), sizeof(mClass)));
            sendServiceMessage(remoteAddr, svcName, ByteArray(mName.c_str(), len));
            sendServiceMessage(remoteAddr, svcEcho); // echo at the end of info
        }
        else if (mAdjacentNode) // remote addr
        {
            mAdjacentNode->acceptServiceMessage(remoteAddr, oid, &msg.data());
        }
        break;

      case svcRequestAllInfo:
        for (size_t i=2; i<mSvcObjects.size(); i++)
            sendServiceMessage(remoteAddr, (SvcOID)i, mSvcObjects[i].read());
        break;
        
      case svcRequestObjInfo:
        mCurrentRemoteAddress = remoteAddr;
        mObjInfoSendCount = 0; // initiate object info sending task
        break;
        
        default:;
    }

    if (oid < mSvcObjects.size())
    {
        ObjectInfo &obj = mSvcObjects[oid];
        if (msg.data().size()) // write
        {
            obj.write(msg.data());
        }
        else
        {
            sendServiceMessage(remoteAddr, oid, obj.read());
        }
    }
}
//---------------------------------------------------------------------------

void ObjnetNode::parseMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        return;
    }

    unsigned char oid = msg.localId().oid;
    unsigned char remoteAddr = msg.localId().sender;

    if (oid < mObjects.size())
    {
        ObjectInfo &obj = mObjects[oid];
        if (obj.isInvokable())
        {
            ByteArray ret = obj.invoke(msg.data());
            if (ret.size())
                sendMessage(remoteAddr, oid, ret);
        }
        else if (msg.data().size()) // write
        {
            obj.write(msg.data());
            if (obj.isDual())
                sendMessage(remoteAddr, oid, obj.read());
        }
        else
        {
            sendMessage(remoteAddr, oid, obj.read());
        }
    }
}
//---------------------------------------------------------

void ObjnetNode::onTimer()
{
    if (mNetState == netnConnecting)
    {
        mNetState = netnDisconnecting;
    }

    mNetTimeout += mTimer.interval();
    if (mNetTimeout >= 1000)
    {
        mNetState = netnStart;
    }
}
