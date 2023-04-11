#include "objnetnode.h"

using namespace Objnet;

unsigned char ObjnetNode::mNodesCount = 0;

ObjnetNode::ObjnetNode(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mNetState(netnStart),
    mNetTimeout(0),
    mCurrentRemoteAddress(0x00),
    mObjInfoSendCount(-1),
    mTimestamp(0),
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
    
    mBusType = iface->busType();
    
    // sendTimer must be running for mTimestamp++
//    if (mBusType == BusSwonb || mBusType == BusRadio)
//        mSendTimer.stop();
    
    if (mNodesCount)
        mSerial ^= rand();

    registerSvcObject(ObjectInfo("class", mClass, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("name", mName));
    registerSvcObject(ObjectInfo("fullName", mFullName));
    registerSvcObject(ObjectInfo("serial", mSerial, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("version", mVersion, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("buildDate", mBuildDate, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("cpuInfo", mCpuInfo, ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("burnCount", mBurnCount));
    registerSvcObject(ObjectInfo("objCount", EVENT(&ObjnetNode::objectCount), ObjectInfo::ReadOnly));
    registerSvcObject(ObjectInfo("busType", mBusType, ObjectInfo::ReadOnly));
    
    mNodesCount++;
}
//---------------------------------------------------------------------------

ObjectInfo &ObjnetNode::bindObject(const ObjectInfo &info)
{
    mObjects.push_back(info);
    ObjectInfo &obj = mObjects.back();
    obj.mDesc.id = mObjects.size() - 1;
    #ifdef __ICCARM__
    if (obj.isStorable())
        objnetStorage()->load(obj);
    #endif
    obj.onValueChanged = EVENT(&ObjnetNode::objectValueChanged);
    return obj;//.mDesc.id;
}

void ObjnetNode::task()
{
    ObjnetCommonNode::task();

    switch (mNetState)
    {
      case netnStart:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            mNetAddress = 0xFF;
            mNetTimeout = 0;
        }
        else
        {
//            mInterface->flush();
            mNetAddress = 0xFF;
            mTimer.stop();
        }
        break;

      case netnConnecting:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            mNetTimeout = 0;
        }
        else if (!mTimer.isActive())
        {
            ByteArray ba;
            ba.append(mBusAddress);
            sendServiceMessage(svcHello, ba);
            mTimer.start();
        }
        break;

      case netnDisconnecting:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            mNetTimeout = 0;
            mNetState = netnStart;
        }
        else
        {
            //mInterface->flush();
            mTimer.stop();
            mNetState = netnStart;
        }
        break;

      case netnAccepted:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            mNetTimeout = 0;
            mNetState = netnReady;
        }
        else
        {
            mTimer.stop();
            mNetState = netnReady;
        }
        
        if (mAdjacentNode)
        {
            // call event for master
            mAdjacentNode->adjacentConnected();
        }
        break;

      case netnReady:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
          
        }
        else
        {
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
        }
        break;
    }
}
//---------------------------------------------------------------------------

//void ObjnetNode::acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba)
//{
////    #ifdef QT_CORE_LIB
////    qDebug() << "node" << QString::fromStdString(mName) << "accept" << oid;
////    #endif
//
////    CommonMessage msg;
//    switch (oid)
//    {
//      case svcHello: // translate hello msg
//      {
//        ba->append(mBusAddress);
//        sendServiceMessage(svcHello, *ba);
//        break;
//      }
//
//      case svcConnected:
//      case svcDisconnected:
//      case svcKill:
//        sendServiceMessage(oid, *ba);
//        break;
//
//      default:;
//    }
//}

void ObjnetNode::parseServiceMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        StdAID aid = static_cast<StdAID>(msg.globalId().aid & 0x3F);
        unsigned char payload = msg.globalId().payload;

        switch (aid)
        {
          case aidPollNodes:
            if (isConnected())
            {
                if (mBusType != BusSwonb && mBusType != BusRadio)
                    sendServiceMessage(svcEcho);
                if (mNetState == netnReady)
                    mNetTimeout = 0;
            }
            else if (mBusType != BusSwonb && mBusType != BusRadio)
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
            if (!payload || payload == mNetAddress)
            {
                mNetState = netnStart;
                if (mAdjacentNode) // remote addr
                {
    //                mAdjacentNode->acceptServiceMessage(remoteAddr, aidConnReset);
                }
            }
            break;
            
          case aidUpgradeStart:
          {
            uint32_t classId = *reinterpret_cast<uint32_t*>(msg.data().data());
            #ifndef QT_CORE_LIB
            if (classId == mClass)
            {
                if (onUpgradeRequest)
                    onUpgradeRequest();
                Application::startOnbBootloader();
            }
            #else
            if (classId == mClass)
                emit upgradeRequest();
            #endif
          } break;
          
          case aidSync:
            mTimestamp = 0;
            break;

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
      case svcEcho:
        sendServiceMessage(remoteAddr, svcEcho);
        break;
      
      case svcHello:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            bool reset = false;
            if (msg.data().size())
            {
                if ((unsigned char)msg.data()[0] == (unsigned char)0xFF)
                    reset = true;
            }
            if (isConnected() && !reset)
            {
                sendServiceMessage(remoteAddr, svcEcho);
            }
            else
            {
                mNetState = netnConnecting;
                ByteArray ba;
                ba.append(mBusAddress);
                sendServiceMessage(svcHello, ba);
            }
        }
        else
        {
//            mNetState = netnConnecting;
            mNetState = netnStart;
            mInterface->flush();
        }
        break;

      case svcWelcome:
      case svcWelcomeAgain:
        if (msg.data().size() == 1)
        {
            if (mBusType == BusSwonb)// || mBusType == BusRadio)
            {
                mNetAddress = msg.data()[0];
                mNetState = netnAccepted;
                sendServiceMessage(remoteAddr, svcEcho);
            }
            else
            {
                for (unsigned int i=0; i<mObjects.size(); i++)
                {
                    ObjectInfo &obj = mObjects[i];
                    obj.mAutoPeriod = 0;
                    obj.mTimedRequest = false;
                }
              
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
        }
        if (mAdjacentNode)
        {
            mAdjacentNode->parseServiceMessage(msg);
        }
        break;

      case svcObjectInfo:
      {
        ByteArray ba;
        uint8_t _oid = msg.data()[0];
        ObjectInfo *obj = 0L;
        if (_oid < mObjects.size())
            obj = &mObjects[_oid];
        for (uint8_t i=1; i<msg.data().size(); i++)
        {
            ba.append(0xFF);
            ba.append(_oid);
            _oid = msg.data()[i];
            if (obj && _oid < obj->subobjectCount())
                obj = &obj->subobject(_oid);
        }
        if (obj)
        {
            obj->mDesc.read(ba);
            sendServiceMessage(remoteAddr, svcObjectInfo, ba);
        }
        else
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
      } break;
      
      case svcGetTimedObject:
      {
        uint8_t _oid = msg.data()[0];
        if (_oid < mObjects.size())
        {
            ByteArray ba;
            ObjectInfo &obj = mObjects[_oid];
            ba.append(reinterpret_cast<const char*>(&_oid), sizeof(unsigned char));
            ba.append('\0'); // reserved byte
            ba.append(reinterpret_cast<const char*>(&mTimestamp), sizeof(uint32_t));
            ba.append(obj.read());
            sendServiceMessage(remoteAddr, svcTimedObject, ba);
        }
        else
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
      } break;
        
      case svcRequestAllInfo:
        if (mBusType == BusSwonb)// || mBusType == BusRadio)
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
            //sendServiceMessage(remoteAddr, svcBusType, mSvcObjects[svcBusType].read());
        }
        else if (isConnected())
        {
            for (size_t i=2; i<mSvcObjects.size(); i++)
                sendServiceMessage(remoteAddr, (SvcOID)i, mSvcObjects[i].read());
        }
        break;

      case svcRequestObjInfo:
        if (mBusType == BusSwonb)
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
        else if (mBusType == BusRadio)
        {
            // TODO: add support of subobjects in the Radio bus
            for (int i=0; i<mObjects.size(); i++)
            {
                ByteArray ba;
                mObjects[i].mDesc.read(ba);
                /*bool success =*/ sendServiceMessage(remoteAddr, svcObjectInfo, ba);
            }
        }
        else if (isConnected())
        {
            mCurrentRemoteAddress = remoteAddr;
            mObjInfoSendCount = 0; // initiate object info sending task
        }
        break;

      case svcAutoRequest:
      case svcTimedRequest:
        if (mBusType == BusSwonb || mBusType == BusRadio)
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
        else if (isConnected())
        {
            if (msg.data().size())
            {
                unsigned char _oid = msg.data()[4];
                if (_oid < mObjects.size())
                {
                    int period = *reinterpret_cast<int*>(msg.data().data());
                    if (period >= 0)
                    {
                        mObjects[_oid].mAutoPeriod = period;
                        mObjects[_oid].mAutoReceiverAddr = remoteAddr;
                        if (oid == svcTimedRequest)
                            mObjects[_oid].mTimedRequest = true;
                    }
//                    else
//                    {
                        *reinterpret_cast<int*>(msg.data().data()) = mObjects[_oid].mAutoPeriod;
                        sendServiceMessage(remoteAddr, oid, msg.data());
//                    }
                }
            }
        }
        else
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
        break;
        
      case svcGroupedRequest:
      {
        ByteArray ba;
        int cnt = msg.data().size();
        for (int i=0; i<cnt; i++)
        {
            unsigned char local_oid = msg.data()[i];
            if (local_oid >= mObjects.size())
                continue;
            ObjectInfo &obj = mObjects[local_oid];
            ba.append(local_oid);
            ba.append(obj.read());
        }
        sendServiceMessage(remoteAddr, svcGroupedObject, ba);
        break;
      }
        
      case svcUpgradeRequest:
      {
        uint32_t classId = *reinterpret_cast<uint32_t*>(msg.data().data());
        #ifndef QT_CORE_LIB
        if (classId == mClass)
        {
            if (onUpgradeRequest)
              onUpgradeRequest();
            Application::startOnbBootloader();
        }
        #else
        if (classId == mClass)
            emit upgradeRequest();
        #endif
      } break;
        
      default:;
    }

    if (isConnected() && oid < svcObjectInfo)
    {
        if (oid < (unsigned char)mSvcObjects.size())
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
        else
        {
            sendServiceMessage(remoteAddr, svcFail, oid);
        }
    }
}
//---------------------------------------------------------------------------

void ObjnetNode::parseMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
    #ifdef QT_CORE_LIB
        if (msg.data().size())
            emit globalDataMessage(msg.globalId().aid, msg.data());
        else
            emit globalMessage(msg.globalId().aid);
    #else
        if (msg.data().size())
            onGlobalDataMessage(msg.globalId().aid, msg.data());
        else if (onGlobalMessage)
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
            if (ret.size() || mBusType == BusSwonb || mBusType == BusRadio)
                sendMessage(remoteAddr, oid, ret);
        }
        else if (msg.data().size()) // write
        {
            obj.write(msg.data());
            if (obj.isDual() || mBusType == BusSwonb || mBusType == BusRadio)
                sendMessage(remoteAddr, oid, obj.read());
            #ifndef QT_CORE_LIB
            if (obj.isStorable() && !obj.isDual())
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
    if (mBusType == BusSwonb || mBusType == BusRadio)
    {
        mNetTimeout += mTimer.interval();
        return;
    }
  
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
            obj.mTimedRequest = false;
        }
    }
}
//---------------------------------------------------------

void ObjnetNode::onSendTimer()
{
    if (mBusType != BusSwonb && mBusType != BusRadio)
    {
        for (unsigned char oid=0; oid<mObjects.size(); oid++)
        {
            ObjectInfo &obj = mObjects[oid];
            if (obj.mAutoPeriod)
            {
                obj.mAutoTime++;
                if (obj.mAutoTime >= obj.mAutoPeriod)
                {
                    obj.mAutoTime = 0;
                    if (obj.mTimedRequest)
                    {
                        ByteArray ba;
                        ba.append(reinterpret_cast<const char*>(&oid), sizeof(unsigned char));
                        ba.append('\0'); // reserved byte
                        ba.append(reinterpret_cast<const char*>(&mTimestamp), sizeof(uint32_t));
                        ba.append(obj.read());
                        sendServiceMessage(obj.mAutoReceiverAddr, svcTimedObject, ba);
                    }
                    else // usual request
                    {
                        sendMessage(obj.mAutoReceiverAddr, oid, obj.read());
                    }
                }
            }
        }
    }
    mTimestamp++;
}

void ObjnetNode::sendForced(unsigned char oid)
{
    sendMessage(0x00, oid, mObjects[oid].read());
}
//---------------------------------------------------------

void ObjnetNode::objectValueChanged(unsigned char oid)
{
    if (oid < mObjects.size())
    {
        #ifdef QT_CORE_LIB

        #else
        if (onObjectValueChanged)
            onObjectValueChanged(mObjects[oid].name());
        #endif
    }
}
