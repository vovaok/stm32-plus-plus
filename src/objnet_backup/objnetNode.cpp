#include "objnetNode.h"

using namespace Objnet;

ObjnetNode::ObjnetNode(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mNetState(netnStart),
    mClass(0x00000000),
    mName("<node>"),
    mFullName("<Generic objnet node>")
{  
    ObjectInfo obj;
    obj.bindVariableRO("class", &mClass, sizeof(mClass));
    registerSvcObject(obj);
    obj.bindVariableRO("name", const_cast<char*>(mName.c_str()), mName.length());
    registerSvcObject(obj);
}
//---------------------------------------------------------------------------

void ObjnetNode::task()
{
    ObjnetCommonNode::task();
    
    switch (mNetState)
    {
      case netnStart:
        mTimer.stop();
        //mNetState = netnConnecting;
        break;
        
      case netnConnecting:
        if (!mTimer.isRunning())
        {
//            if (mAdjacentNode)
//                mAdjacentNode->acceptServiceMessage(svcHello);
            //sendServiceMessage(svcHello, macAuto, ByteArray(&mClass, sizeof(mClass)));
            sendServiceMessage(svcHello, macAuto, ByteArray(&mBusAddress, 1));
//            ByteArray ba;
//            ba.append(mBusAddress);
//            if (mAdjacentNode)
//            {
//                mAdjacentNode->acceptServiceMessage(svcTree, &ba);
//            }
            mTimer.start();
        }
        if (mTimer.time() > 200)
        {
            mInterface->flush();
            mTimer.stop();
            //mNetState = netnStart;
        }
        break;
        
      case netnAccepted:
      {
        mTimer.stop();
        int len = mName.length();
        if (len > 8)
            len = 8;
        // send different info
        sendServiceMessage(svcEcho);
        sendServiceMessage(svcClass, macAuto, ByteArray(&mClass, sizeof(mClass)));
        sendServiceMessage(svcName, macAuto, ByteArray(mName.c_str(), len));
        mNetState = netnReady;
        break;
      }
      
      case netnReady:
        if (!mTimer.isRunning())
        {
            mTimer.start(); 
        }
        else if (mTimer.time() > 1000)
        {   
            mNetState = netnStart;
        }
        break;
    }
}
//---------------------------------------------------------------------------

void ObjnetNode::acceptServiceMessage(SvcOID oid, ByteArray *ba)
{
    CommonMessage msg;
    switch (oid)
    {
      case svcHello: // translate hello msg
      {
        ba->append(mBusAddress);
        sendServiceMessage(svcHello, macAuto, *ba);
        break;
      }
      
      case svcConnected:
      case svcDisconnected:
      case svcKill:
        sendServiceMessage(oid, macAuto, *ba);
        break;
          
    }
}

void ObjnetNode::parseServiceMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        StdAID aid = (StdAID)msg.globalId().aid;
        switch (aid)
        {
          case aidPollNodes:
            if (isConnected())
            {
                sendServiceMessage(svcEcho);
                if (mNetState == netnReady)
                    mTimer.reset();
            }
            else
            {
                mNetState = netnConnecting;
            }
            break;
            
          case aidConnReset:
            mNetAddress = 0x00;
            mNetState = netnConnecting;
            break;
        }
        return;
    }
  
    SvcOID oid = (SvcOID)msg.localId().oid;    
    unsigned char remoteAddr = msg.localId().sender;
    switch (oid)
    {     
      case svcHello:
        mNetState = netnConnecting;
        break;

      case svcWelcome:
      case svcWelcomeAgain:
        if (msg.data().size() == 1)
        {
            mNetAddress = msg.data()[0];
            mNetState = netnAccepted;
            if (mAdjacentNode)
            {
                mAdjacentNode->mNetAddress = mNetAddress;
            }
        }
        else // retranslate
        {
            mAdjacentNode->acceptServiceMessage(svcWelcome, &msg.data());
        }
        break;
    }
    
    if (oid < mSvcObjects.size())
    {
        ObjectInfo &obj = mSvcObjects[oid];
        if ((obj.mWriteSize > 0) && (msg.data().size() == obj.mWriteSize))
        {
            for (int i=0; i<obj.mWriteSize; i++)
                reinterpret_cast<unsigned char*>(obj.mWritePtr)[i] = msg.data()[i];
        }
        if (obj.mReadSize > 0)
        {
            ByteArray ba(reinterpret_cast<unsigned char*>(obj.mReadPtr), obj.mReadSize);
            sendServiceMessage(oid, macAuto, ba, remoteAddr);
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
    
    SvcOID oid = (SvcOID)msg.localId().oid;    
    unsigned char remoteAddr = msg.localId().sender;
    
    if (oid < mObjects.size())
    {
        ObjectInfo &obj = mObjects[oid];
        if ((obj.mWriteSize > 0) && (msg.data().size() == obj.mWriteSize))
        {
            for (int i=0; i<obj.mWriteSize; i++)
                reinterpret_cast<unsigned char*>(obj.mWritePtr)[i] = msg.data()[i];
        }
        if (obj.mReadSize > 0)
        {
            ByteArray ba(reinterpret_cast<unsigned char*>(obj.mReadPtr), obj.mReadSize);
            sendRemoteMessage(remoteAddr, oid, ba);
        }
    }
}