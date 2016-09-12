#include "objnetnode.h"

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
    #ifndef QT_CORE_LIB
    mTimer.setTimeoutEvent(EVENT(&ObjnetNode::onTimeoutTimer));
    mSendTimer.setTimeoutEvent(EVENT(&ObjnetNode::onSendTimer));
    #else
    QObject::connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimeoutTimer()));
    QObject::connect(&mSendTimer, SIGNAL(timeout()), this, SLOT(onSendTimer()));
    #endif
    mTimer.start(200);
    mSendTimer.start(1);

    #ifndef QT_CORE_LIB
    mVersion = stmApp()->version();
    mBuildDate = stmApp()->buildDate();
    mSerial = CpuId::serial();
    mCpuInfo = stmApp()->cpuInfo();
    mBurnCount = stmApp()->burnCount();
    #endif

    registerSvcObject(ObjectInfo("class", mClass, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("name", mName));
    registerSvcObject(ObjectInfo("fullName", mFullName));
    registerSvcObject(ObjectInfo("serial", mSerial, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("version", mVersion, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("buildDate", mBuildDate, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("cpuInfo", mCpuInfo, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("burnCount", mBurnCount));
    registerSvcObject(ObjectInfo("objCount", EVENT(&ObjnetNode::objectCount), ObjectInfo::ReadOnly));
}
//---------------------------------------------------------------------------

unsigned char ObjnetNode::bindObject(const ObjectInfo &info)
{
    mObjects.push_back(info);
    ObjectInfo &obj = mObjects.back();
    obj.mDesc.id = mObjects.size() - 1;
    #ifdef __ICCARM__
    if (obj.isStorable())
        objnetStorage()->load(obj);
    #endif
    return obj.mDesc.id;
}

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
        while (mObjInfoSendCount >= 0 && mObjInfoSendCount < (int)mObjects.size())
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
//    #ifdef QT_CORE_LIB
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

//        #ifdef QT_CORE_LIB
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
            #ifndef QT_CORE_LIB
            if (onPolling)
                onPolling();
            #else
                emit polling();
            #endif
            break;

          case aidConnReset:
//            mNetAddress = 0x00;
//            mNetState = netnConnecting;
            mNetState = netnStart;
            break;
            
          case aidUpgradeStart:
          {
            unsigned long classId = *reinterpret_cast<unsigned long*>(msg.data().data());
            #ifndef QT_CORE_LIB
            if (classId == mClass && onUpgradeRequest)
                onUpgradeRequest();
            #else
            if (classId == mClass)
                emit upgradeRequest();
            #endif
          } break;

          default:;
        }
        return;
    }

    SvcOID oid = (SvcOID)msg.localId().oid;
    unsigned char remoteAddr = msg.localId().sender;

//    #ifdef QT_CORE_LIB
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
        for (unsigned int i=0; i<mObjects.size(); i++)
        {
            ObjectInfo &obj = mObjects[i];
            obj.mAutoPeriod = 0;
        }
        if (msg.data().size() == 1)
        {
            mNetAddress = msg.data()[0];
            mNetState = netnAccepted;
            int len = mName.length();
            if (len > 8)
                len = 8;
            // send different info
            sendServiceMessage(remoteAddr, svcClass, ByteArray(reinterpret_cast<const char*>(&mClass), sizeof(mClass)));
            sendServiceMessage(remoteAddr, svcName, ByteArray(_fromString(mName).c_str(), len));
            sendServiceMessage(remoteAddr, svcEcho); // echo at the end of info
        }
        else if (mAdjacentNode) // remote addr
        {
            mAdjacentNode->acceptServiceMessage(remoteAddr, oid, &msg.data());
        }
        break;

      case svcRequestAllInfo:
        if (isConnected())
        {
            for (size_t i=2; i<mSvcObjects.size(); i++)
                sendServiceMessage(remoteAddr, (SvcOID)i, mSvcObjects[i].read());
        }
        break;

      case svcRequestObjInfo:
        if (isConnected())
        {
            mCurrentRemoteAddress = remoteAddr;
            mObjInfoSendCount = 0; // initiate object info sending task
        }
        break;

      case svcAutoRequest:
        if (isConnected())
        {
            int period = *reinterpret_cast<int*>(msg.data().data());
            unsigned char oid = msg.data()[4];
            if (oid < mObjects.size())
            {
                mObjects[oid].mAutoPeriod = period;
                mObjects[oid].mAutoReceiverAddr = remoteAddr;
            }
        }
        break;

        default:;
    }

    if (isConnected() && oid < mSvcObjects.size())
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
    #ifdef QT_CORE_LIB
        emit globalMessage(msg.globalId().aid);
    #else
        if (onGlobalMessage)
            onGlobalMessage(msg.globalId().aid);
    #endif
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
            #ifdef __ICCARM__
            else if (obj.isStorable())
                objnetStorage()->save(obj);
            #endif
        }
        else
        {
            sendMessage(remoteAddr, oid, obj.read());
        }
    }
}
//---------------------------------------------------------

void ObjnetNode::onTimeoutTimer()
{
    if (mNetState == netnConnecting)
    {
        mNetState = netnDisconnecting;
    }

    mNetTimeout += mTimer.interval();
    if (mNetTimeout >= 1000)
    {
        mNetState = netnStart;
        
        for (unsigned int oid=0; oid<mObjects.size(); oid++)
        {
            ObjectInfo &obj = mObjects[oid];
            obj.mAutoPeriod = 0;
            obj.mAutoTime = 0;
        }
    }
}
//---------------------------------------------------------

void ObjnetNode::onSendTimer()
{
    for (unsigned int oid=0; oid<mObjects.size(); oid++)
    {
        ObjectInfo &obj = mObjects[oid];
        if (obj.mAutoPeriod)
        {
            obj.mAutoTime++;
            if (obj.mAutoTime >= obj.mAutoPeriod)
            {
                obj.mAutoTime = 0;
                sendMessage(obj.mAutoReceiverAddr, oid, obj.read());
            }
        }
    }
}

void ObjnetNode::sendForced(unsigned char oid)
{
    sendMessage(0x00, oid, mObjects[oid].read());
}
//---------------------------------------------------------
