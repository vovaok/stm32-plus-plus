#include "objnetmaster.h"

using namespace Objnet;

ObjnetMaster::ObjnetMaster(ObjnetInterface *iface) :
    ObjnetCommonNode(iface)    
{
    for (int i=0; i<16; i++)
    {
        mLocalnetDevices[i] = 0L;
        mNetAddrByMacCache[i] = 0;
    }
    setBusAddress(0);
    mNetAddress = 0x00;
    mInterface->setMasterMode(true);
    mInterface->errorEvent = EVENT(&ObjnetMaster::onError);

    BusType busType = mInterface->busType();
    mSwonbMode = (busType == BusSwonb || busType == BusRadio);
    
    reset();
    
    #ifndef QT_CORE_LIB
    mTimer.setTimeoutEvent(EVENT(&ObjnetMaster::onTimer));
    mInterface->nakEvent = EVENT(&ObjnetMaster::onNak);
    #else
    QObject::connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    #endif
    mTimer.start(200);
}

ObjnetMaster::~ObjnetMaster()
{
    reset();
}

void ObjnetMaster::reset()
{
    mAssignNetAddress = 1;
    mSearchMac = 1;
    while (!mDevices.empty())
        removeDevice(mDevices.begin()->first);
    mRouteTable[0x00] = 0; // сразу записываем, как достучаться до верхнего уровня
}
//---------------------------------------------------------------------------

void ObjnetMaster::task()
{
    ObjnetCommonNode::task();

    if (mRouteTable.size() >= 127) // если вся таблица маршрутизации заполнена, значит что-то пошло не так, и...
    {
        mRouteTable.clear();                    // чистим таблицу маршрутизации
        mRouteTable[0x00] = 0;                  // сразу записываем, как достучаться до верхнего уровня
        sendGlobalServiceMessage((StdAID)(aidConnReset | aidPropagationDown)); // выполняем перенумерацию
    }
}

void ObjnetMaster::onNak(unsigned char mac)
{
    if (mLocalnetDevices[mac])
    {
        if (mLocalnetDevices[mac]->mTimeout)
            --mLocalnetDevices[mac]->mTimeout;
    }
}

void ObjnetMaster::onError(unsigned char mac, ObjnetInterface::Error error)
{
    string cause;
    switch (error)
    {
        case ObjnetInterface::ErrorFrame: cause = "Frame error"; break;
        case ObjnetInterface::ErrorChecksum: cause = "Checksum error"; break;
        case ObjnetInterface::ErrorNoSOF: cause = "No SOF"; break;
        case ObjnetInterface::ErrorInterface: cause = "Internal error"; break;
    }
    printf("SWONB error on address %d: %s\n", mac, cause.c_str());
    
    if (mac && mLocalnetDevices[mac])
    {
        mLocalnetDevices[mac]->mConnectionError = true;
    }
    
    sendServiceMessageToMac(mac, svcFail);
}

void ObjnetMaster::adjacentConnected()
{
    // send devices info
//    for (DeviceIterator it=mDevices.begin(); it!=mDevices.end(); it++)
//    {
//        ObjnetDevice *dev = it->second;
//        ByteArray location;
//        location.append(dev->busAddress());
//        while (dev)
//        {
//            location.append(dev->netAddress());
//            dev = dev->parentDevice();
//        }
//        for (int i=2; i<location.size(); i++)
//            location[i] = mAdjacentNode->natRoute(location[i]);
//        mAdjacentNode->sendServiceMessage(svcHello, location);
//    }
}
//---------------------------------------------------------------------------

void ObjnetMaster::onTimer()
{
    sendGlobalServiceMessage(aidPollNodes);
    
    // if there are no devices then search them
    if (mDevices.empty() && mSwonbMode)
    {
        for (unsigned char mac=1; mac<16; mac++)
        {
//            if (mLocalnetDevices[mac] && mLocalnetDevices[mac]->mConnectionError)
//                sendServiceMessageToMac(mac, svcFail);
//            else
                sendServiceMessageToMac(mac, svcHello);
        }
    }
    else
    {
        // test local devices for presence
        for (unsigned char mac=1; mac<16; mac++)
        {
            ObjnetDevice *dev = mLocalnetDevices[mac];
            if (!dev)
                continue;
            if (!mSwonbMode && dev->mTimeout)
                --dev->mTimeout;
            if (dev->mPresent && !dev->mTimeout)
            {
                disconnectDevice(dev->netAddress());
                if (dev->mAutoDelete)
                    removeDevice(dev->netAddress());
            }
            // nado potom vernut kogda vse zarabotaet
//            if (mSwonbMode)
//            {
//                // request device presence
//                sendServiceMessageToMac(mac, svcHello);
//                if (mSearchMac == mac)
//                    mSearchMac = (mSearchMac < 15)? mSearchMac + 1: 1;
//            }
        }
        if (mSwonbMode)
        {
//            if (mLocalnetDevices[mSearchMac] && mLocalnetDevices[mSearchMac]->mConnectionError)
//                sendServiceMessageToMac(mSearchMac, svcFail);
//            else
                sendServiceMessageToMac(mSearchMac, svcHello);
            mSearchMac = (mSearchMac < 15)? mSearchMac + 1: 1;
        }
    }
}
//---------------------------------------------------------------------------

ObjnetDevice *ObjnetMaster::createDevice(unsigned char mac, ByteArray &location)
{
    ObjnetDevice *dev = 0L;
    char &netaddr = location.data()[1];
    bool localnet = (location.size() == 2);
 
    if (localnet && mNetAddrByMacCache[mac])    // если девайс уже был в текущей подсети
    {
        netaddr = mNetAddrByMacCache[mac];      // пытаемся вспомнить адрес по маку
        mRouteTable[netaddr] = mac;
    }
    else
    {
        netaddr = createNetAddress(mac);        // создаём новый адрес
    }

    if (localnet)
    {
        dev = mLocalnetDevices[mac];
        dev->mNetAddress = netaddr;
    }
    if (!dev)
    {
        dev = new ObjnetDevice(netaddr);          // создаём объект с новым адресом
    //    dev->mAutoDelete = true;                    // раз автоматически создали - автоматически и удалим)
    }
    dev->mMaster = this;
    dev->mAutoDelete = false;     // ***** !!!!! TRY THIS !!!!!! *********
    dev->mBusAddress = location[0];
    dev->mIsLocal = localnet;
    dev->masterRequestObject = EVENT(&ObjnetMaster::requestObject);
    dev->masterSendObject = EVENT(&ObjnetMaster::sendObject);
    dev->masterServiceRequest = EVENT(&ObjnetMaster::sendServiceRequest);
    
//    if (mSwonbMode)                             // TEMPORARY SOLUTION!!!
//    {
//        dev->mBusType = BusSwonb;
//    }

    mDevices[netaddr] = dev;                    // запоминаем для поиска по адресу
    if (localnet)                               // если девайс в текущей подсети...
    {
        mLocalnetDevices[mac] = dev;            // ...запоминаем для поиска по маку
        mNetAddrByMacCache[mac] = netaddr;      // и кэшируем адрес для возврата по маку (чтобы лишний раз не создавать)
    }
    else
    {
        unsigned char paraddr = location[2];
        if (mDevices.count(paraddr))
        {
            ObjnetDevice *par = mDevices[paraddr];
            par->mChildren.push_back(dev);
            dev->mParent = par;
        }
    }
    
    #ifdef QT_CORE_LIB
    QObject::connect(dev, SIGNAL(requestObject(unsigned char,unsigned char)), SLOT(requestObject(unsigned char,unsigned char)));
    QObject::connect(dev, SIGNAL(sendObject(unsigned char,unsigned char,QByteArray)), SLOT(sendObject(unsigned char,unsigned char,QByteArray)));
    QObject::connect(dev, SIGNAL(serviceRequest(unsigned char,SvcOID,QByteArray)), SLOT(sendServiceRequest(unsigned char,SvcOID,QByteArray)));
    #endif    
    
    #ifdef QT_CORE_LIB
    emit devAdded(netaddr, location);                 // устройство добавлено
    #else
    if (onDevAdded)
        onDevAdded(netaddr, location);
    #endif
    
    if (mAdjacentNode && mAdjacentNode->isConnected()) // если мастер связан с узлом, который уже подключен
    {
        bool send = true;
        for (int i=2; i<location.size(); i++)
        {
            location[i] = mAdjacentNode->natRoute(location[i]);
            if ((unsigned char)location[i] == (unsigned char)0x7F)
                send = false;
        }
        if (send)
            mAdjacentNode->sendServiceMessage(svcHello, location); // отправляем дальше
    } 
    
    return dev;
}

void ObjnetMaster::connectDevice(unsigned char netaddr)
{
    if (!mDevices.count(netaddr))
        return;
    ObjnetDevice *dev = mDevices[netaddr];
    dev->mPresent = true;
    dev->mTimeout = 10;
    
    if (mAdjacentNode)
    {
        unsigned char supernetaddr = mAdjacentNode->natRoute(netaddr);
        if (supernetaddr < 0x7F)
            mAdjacentNode->sendServiceMessage(svcConnected, supernetaddr);
    }
    
    #ifdef QT_CORE_LIB
    emit devConnected(netaddr);
    #else
    if (onDevConnected)
        onDevConnected(netaddr);
    #endif      
}

void ObjnetMaster::disconnectDevice(unsigned char netaddr)
{
    if (!mDevices.count(netaddr))
        return;
    ObjnetDevice *dev = mDevices[netaddr];
    dev->mPresent = false;

    // recursively disconnect children
    for (size_t i=0; i<dev->mChildren.size(); i++)
        disconnectDevice(dev->mChildren[i]->netAddress());
    
    if (mAdjacentNode)
    {
        unsigned char supernetaddr = mAdjacentNode->natRoute(netaddr);
        if (supernetaddr < 0x7F)
            mAdjacentNode->sendServiceMessage(svcDisconnected, supernetaddr);
    }
    
    #ifdef QT_CORE_LIB
    emit devDisconnected(netaddr);
    #else
    if (onDevDisconnected)
        onDevDisconnected(netaddr);
    #endif
}

void ObjnetMaster::removeDevice(unsigned char netaddr)
{
    if (!mDevices.count(netaddr))
        return;
    ObjnetDevice *dev = mDevices[netaddr];

    // recursively remove children
    for (size_t i=0; i<dev->mChildren.size(); i++)
        removeDevice(dev->mChildren[i]->netAddress());
    
    if (dev->isPresent())
        disconnectDevice(netaddr);

    if (mAdjacentNode)
    {
        unsigned char supernetaddr = mAdjacentNode->natRoute(netaddr);
        if (supernetaddr < 0x7F)
        {
            mAdjacentNode->sendServiceMessage(svcKill, supernetaddr);
            removeNatPair(supernetaddr, netaddr);
        }
    }

    #ifdef QT_CORE_LIB
    emit devRemoved(netaddr);
    #else
    if (onDevRemoved)
        onDevRemoved(netaddr);
    #endif
    
    unsigned char mac = route(netaddr);
    mRouteTable.erase(netaddr);
    mLocalnetDevices[mac] = 0L;
    delete dev;
    mDevices.erase(netaddr);
}
//---------------------------------------------------------------------------

void ObjnetMaster::parseServiceMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        #ifdef QT_CORE_LIB
        emit globalServiceMessage(msg.globalId().aid);
        #else
        if (onGlobalServiceMessage)
            onGlobalServiceMessage(msg.globalId().aid);
        #endif
        return;
    }

    SvcOID oid = (SvcOID)msg.localId().oid;
    unsigned char netaddr = msg.localId().sender;
    ObjnetDevice *dev = mDevices.count(netaddr)? mDevices[netaddr]: 0L;
    
    switch (oid)
    {
      case svcEcho:
        if (!dev)
        {
            // reset node's connection state
            sendGlobalServiceMessage(aidConnReset, netaddr); // new way more logical
            //sendServiceMessageToMac(mCurMac, svcHello, 0xFF);
        }
        else if (!dev->isPresent())
        {
            if (dev->isValid())
            {
                connectDevice(dev->netAddress());
                if (dev->mIsLocal && !dev->isInfoValid())
                    sendServiceMessage(netaddr, svcRequestAllInfo);
            }
            else
            {
                sendServiceMessage(netaddr, svcClass);
                sendServiceMessage(netaddr, svcName);
            }
        }
        else
        {
            dev->mTimeout = 10;
        }
        break;
        
      case svcHello:
      {  
        ByteArray location = msg.data();
        bool isLocalNet = (location.size() == 1);
        unsigned char mac = location[0];
        unsigned char subnetaddr = isLocalNet? 0: location[1];
        if (isLocalNet)
        {
            dev = mLocalnetDevices[mac];
            if (dev)
                dev->mConnectionError = false;
        }
        else if (dev)
        {
//#warning TODO!! implement ONB devices recursive search
            ObjnetDevice *foundDev = 0L;
            for (int i=0; i<dev->mChildren.size() && !foundDev; i++)
            {
                ObjnetDevice *child = dev->mChildren[i];
                if (location.size() == 2) // subnet level
                {
                    if (child->busAddress() == mac)
                        foundDev = child;
                }
                else if (location.size() == 3)
                {
                    for (int i=0; i<child->mChildren.size() && !foundDev; i++)
                    {
                        ObjnetDevice *grandchild = child->mChildren[i];
                        if (grandchild->busAddress() == mac)
                            foundDev = grandchild;
                    }
                }
            }
            dev = foundDev;
            if (netaddr != 0x7F)
                mac = route(netaddr);
            else
                break; // ERROR!!! this is impossibru! (child device attempts to connect BEFORE parent)
        }
        else
        {
            break; // ERROR!!! this is impossibru! (child device attempts to connect BEFORE parent)
        }
        
        location.append(netaddr); // append sender's address as parent
            
        SvcOID welcomeCmd = svcWelcomeAgain;
        if (!dev)
        {
            dev = createDevice(mac, location);
            welcomeCmd = svcWelcome;
        }
        
        ByteArray response;
        response.append(dev->netAddress());
        if (isLocalNet)
        {
            sendServiceMessage(dev->netAddress(), welcomeCmd, response);
        }
        else
        {
            response.append(subnetaddr);
            sendServiceMessage(netaddr, welcomeCmd, response);
        }
      } break;
      
      case svcWelcome: // this is message from supernet
      case svcWelcomeAgain:
      {
        if (msg.size() == 1)
        {
            for (int i=1; i<16; i++)
            {
                ObjnetDevice *dev = mLocalnetDevices[i];
                if (dev)
                {
                    ByteArray location;
                    location.append(dev->busAddress());
                    location.append(dev->netAddress());  
                    mAdjacentNode->sendServiceMessage(svcHello, location);
                }
            }
        }
        if (msg.size() == 2)
        {
            unsigned char supernetaddr = msg.data()[0];
            unsigned char subnetaddr = msg.data()[1];
            ObjnetDevice *dev = mDevices.count(subnetaddr)? mDevices[subnetaddr]: 0L;
            if (dev)
            {
                addNatPair(supernetaddr, subnetaddr);
                // TODO: send dev's class & name somehow
                mAdjacentNode->sendServiceMessage(netaddr, svcConnected, supernetaddr); // a la echo
                
                for (int i=0; i<dev->mChildren.size(); i++)
                {
                    ObjnetDevice *child = dev->mChildren[i];
                    ByteArray location;
                    location.append(child->busAddress());
                    while (child)
                    {
                        location.append(child->netAddress());
                        child = child->parentDevice();
                    }
                    for (int i=2; i<location.size(); i++)
                        location[i] = mAdjacentNode->natRoute(location[i]);
                    mAdjacentNode->sendServiceMessage(svcHello, location);
                }
            }
            else
            {
                break; // this is IMPOSSIBRU!!
            }
        }
      } break;
      
      case svcConnected:
      {
        unsigned char remoteAddress = msg.data()[0];
        ObjnetDevice *remoteDev = mDevices.count(remoteAddress)? mDevices[remoteAddress]: 0L;
        if (!remoteDev)
            break; // this is IMPOSSIBRU!
        if (remoteDev->isValid())
        {
            if (!remoteDev->isPresent())
                connectDevice(remoteAddress);
        }
        else
        {
            sendServiceMessage(remoteAddress, svcClass);
            sendServiceMessage(remoteAddress, svcName);
            sendServiceMessage(remoteAddress, svcEcho);
        }
      } break;
        
      case svcDisconnected:
      {
        unsigned char remoteAddress = msg.data()[0];
        disconnectDevice(remoteAddress);
      } break;
        
      case svcKill:
      {
        unsigned char remoteAddress = msg.data()[0];
        removeDevice(remoteAddress);
      } break;
      
      case svcObjectCount:
        sendServiceMessage(netaddr, svcRequestObjInfo);
        break;
      
      case svcFail:
        if (msg.size())
        {
            unsigned char failedOid = static_cast<unsigned char>(msg.data()[0]);
            switch (failedOid)
            {
              case svcRequestAllInfo:
                for (unsigned char idx = svcClass; idx <= svcBusType; idx++)
                    sendServiceMessage(netaddr, static_cast<SvcOID>(idx));
                break;
                
              case svcRequestObjInfo:
                if (dev)
                {
                    for (int idx=0; idx<dev->mObjectCount; idx++)
                        sendServiceMessage(netaddr, svcObjectInfo, idx);
                }
                break;
                
              case svcAutoRequest:
                break;
                
              case svcTimedRequest:
                break;
                
              default:;
            }
            
            if (failedOid < svcObjectInfo && dev)
                dev->receiveServiceObject(failedOid, ByteArray());
        }
        break;

      case svcTimedRequest:
      case svcAutoRequest:
        if (dev)
            dev->receiveServiceObject(oid, msg.data());
        break;

      default:;
    }

    if (oid < svcEcho)
    {
        if (dev)
            dev->receiveServiceObject(oid, msg.data());
        #ifdef QT_CORE_LIB
        emit serviceMessageAccepted(netaddr, oid, msg.data());
        #endif
    }
    
    if (onServiceMessage)
        onServiceMessage(msg);
}
//---------------------------------------------------------------------------

void ObjnetMaster::parseMessage(CommonMessage &msg)
{
    if (msg.isGlobal())
    {
        #ifdef QT_CORE_LIB
        emit globalMessage(msg.globalId().aid);
        #else
        if (onGlobalMessage)
            onGlobalMessage(msg.globalId().aid);
        #endif

        unsigned char remoteAddr = msg.globalId().addr;
        ObjnetDevice *dev = mDevices.count(remoteAddr)? mDevices[remoteAddr]: 0L;
        if (dev)
            dev->receiveGlobalMessage(msg.globalId().aid);
    }

    unsigned char oid = msg.localId().oid;
    unsigned char remoteAddr = msg.localId().sender;
    ObjnetDevice *dev = mDevices.count(remoteAddr)? mDevices[remoteAddr]: 0L;
    if (dev)
        dev->receiveObject(oid, msg.data());
}
//---------------------------------------------------------------------------

unsigned char ObjnetMaster::createNetAddress(unsigned char mac)
{
    // сразу избегаем бесконечного цикла
    if (mRouteTable.size() >= 127)
        return 0x7F;

    // ищем в таблице маршрутизации первый свободный адрес
    while (mRouteTable.count(mAssignNetAddress))
    {
        mAssignNetAddress++;
        if (mAssignNetAddress >= 127)
            mAssignNetAddress = 1;
    }
    unsigned char retAddr = mAssignNetAddress++;
    if (mAssignNetAddress >= 127)
        mAssignNetAddress = 1;
    mRouteTable[retAddr] = mac;
    return retAddr;
}
//---------------------------------------------------------------------------

ObjnetDevice *ObjnetMaster::deviceBySerial(uint32_t serial)
{
    for (DeviceIterator it=mDevices.begin(); it!=mDevices.end(); it++)
    {
        ObjnetDevice *dev = it->second;
        if (dev->serial() == serial)
            return dev;
    }
    return 0L;
}
//---------------------------------------------------------------------------

void ObjnetMaster::requestObject(unsigned char netAddress, unsigned char oid)
{
    sendMessage(netAddress, oid);
}

void ObjnetMaster::sendObject(unsigned char netAddress, unsigned char oid, const ByteArray &ba)
{
    sendMessage(netAddress, oid, ba);
}
//---------------------------------------------------------------------------

ObjnetDevice* ObjnetMaster::createStaticDevice(unsigned char busAddress)
{
    ByteArray loc;
    loc.append(busAddress);
    loc.append(0x7F);
    ObjnetDevice *dev = createDevice(busAddress, loc);
    return dev;
}

void ObjnetMaster::registerDevice(ObjnetDevice *dev, unsigned char busAddress)
{
    mLocalnetDevices[busAddress] = dev;
    createStaticDevice(busAddress);
}
