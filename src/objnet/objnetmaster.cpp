#include "objnetmaster.h"

using namespace Objnet;

//unsigned char ObjnetDeviceTreeNode::mAddressToAssign = 0;

ObjnetMaster::ObjnetMaster(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mAssignNetAddress(1),
    mAdjIfConnected(false),
    mSwonbMode(false),
    mCurMac(1),
    mSwonbReset(true)
{
    for (int i=0; i<16; i++)
    {
        mLocalnetDevices[i] = 0L;
        mNetAddrByMacCache[i] = 0;
    }
    setBusAddress(0);
    mNetAddress = 0x00;
    
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
    for (DeviceIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        delete it->second;
    mDevices.clear();
    mRouteTable.clear();
    mRouteTable[0x00] = 0; // сразу записываем, как достучаться до верхнего уровня
}
//---------------------------------------------------------------------------

void ObjnetMaster::task()
{
    ObjnetCommonNode::task();

    if (mAdjacentNode)
    {
        if (!mAdjIfConnected && mAdjacentNode->isConnected())
        {
            if (mSwonbMode)
                swonbEnumerate(); // re-enumerate nodes
            else
                sendGlobalServiceMessage((StdAID)(aidConnReset | aidPropagationDown)); // reset subnet state on adjacent node connection
        }
        mAdjIfConnected = mAdjacentNode->isConnected(); // store previous value of connection state
    }

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
        //mLocalnetDevices[mac]->mPresent = false;
    }
}

void ObjnetMaster::onTimer()
{
    sendGlobalServiceMessage(aidPollNodes);
    
    std::vector<unsigned char> addressesToRemove;
    
    if (mSwonbMode)
    {
        for (DeviceIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        {       
            ObjnetDevice *dev = it->second;
            if (!dev->mTimeout)
                dev->mPresent = false;
            if ((!dev->mPresent && !dev->mTimeout && dev->mAutoDelete) || mSwonbReset)
                addressesToRemove.push_back(it->first);
        }
    }
    else
    {
        for (DeviceIterator it=mDevices.begin(); it!=mDevices.end(); it++)
        {
            ObjnetDevice *dev = it->second;
            if (dev->mTimeout)
                dev->mTimeout--;
            if (dev->mPresent && !dev->mTimeout)
            {
                dev->mPresent = false;
                dev->mObjectCount = 0;
                if (dev->mAutoDelete)
                {
                    addressesToRemove.push_back(it->first);
                }
                else
                {
                    if (mAdjacentNode)
                    {
                        ByteArray ba;
                        ba.append(dev->mNetAddress);
                        mAdjacentNode->acceptServiceMessage(0, svcDisconnected, &ba);
                    }
                    #ifdef QT_CORE_LIB
                    emit devDisconnected(dev->mNetAddress);
                    #else
                    if (onDevDisconnected)
                        onDevDisconnected(dev->mNetAddress);
                    #endif
                }
            }
        }
    }
    
    
    // удаляем отсутствующие девайсы, если у них включено автоудаление
    for (std::vector<unsigned char>::iterator it=addressesToRemove.begin(); it!=addressesToRemove.end(); it++)
    {
        if (!mDevices.count(*it))
            continue;
        ObjnetDevice *dev = mDevices[*it];
        if (mAdjacentNode)
        {
            unsigned char supernetaddr = mAdjacentNode->natRoute(dev->mNetAddress);
            if (supernetaddr < 0x7F)
            {
                ByteArray ba;
                ba.append(supernetaddr);
                mAdjacentNode->acceptServiceMessage(0, svcKill, &ba);
                removeNatPair(supernetaddr, dev->mNetAddress);
            }
        }
        #ifdef QT_CORE_LIB
        emit devDisconnected(dev->mNetAddress);
        emit devRemoved(dev->mNetAddress);
        #else
        if (onDevDisconnected)
            onDevDisconnected(dev->mNetAddress);
        if (onDevRemoved)
            onDevRemoved(dev->mNetAddress);
        #endif
        unsigned char mac = route(dev->mNetAddress);
        mRouteTable.erase(dev->mNetAddress);
        mLocalnetDevices[mac] = 0L;
        delete dev;
        mDevices.erase(*it);
    }
    
    if (mSwonbMode)
    {
        if (mSwonbReset)
        {
            for (mCurMac=1; mCurMac<16; mCurMac++)
                sendServiceMessageToMac(mCurMac, svcHello);
            mCurMac = 0;
            mSwonbReset = false;
        }
        else
        {
            mCurMac++;
            sendServiceMessageToMac(mCurMac, svcHello);
            mCurMac &= 0xF;
        }
          
//        if (mCurMac)
//        {
//            for (; mCurMac<16; mCurMac++)
//                sendServiceMessageToMac(mCurMac, svcHello);
////            mCurMac++;
//            mCurMac = 0; // temporary!! or not?
////            mCurMac &= 0xF;
//        }
        return;
    }
}
//---------------------------------------------------------------------------

void ObjnetMaster::acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba)
{
//    #ifdef QT_CORE_LIB
//    qDebug() << "master" << QString::fromStdString(mName) << "accept" << oid;
//    #endif

    switch (oid)
    {
      case svcWelcome:
//      case svcWelcomeAgain:
      {
        unsigned char supernetaddr = ba->data()[0];
        unsigned char netaddr = ba->data()[1];
        addNatPair(supernetaddr, netaddr);        // добавляем в таблицу NAT
        ObjnetDevice *dev = mDevices.count(netaddr)? mDevices[netaddr]: 0L;
        if (dev)
        {
//            if (dev->mPresent) // && dev->isValid())
//            {
                ByteArray ba;
                ba.append(supernetaddr);
                mAdjacentNode->sendServiceMessageSheduled(sender, svcConnected, ba);
//            }
        }
        break;
      }

      default:; // warning elimination
    }
}

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

//    #ifdef QT_CORE_LIB
//    qDebug() << "master" << QString::fromStdString(mName) << "parse" << oid;
//    #endif

    unsigned char netaddr = msg.localId().sender;
    ObjnetDevice *dev = mDevices.count(netaddr)? mDevices[netaddr]: 0L;
    switch (oid)
    {
      case svcEcho:
        if (!dev)
        {
            sendGlobalServiceMessage(aidConnReset);
            //sendServiceMessage(netaddr, svcHello); // reset node's connection state
        }
        else
        {
            if (!dev->mPresent)
            {
//                if (mAdjacentNode)
//                {
//                    unsigned char addr = mAdjacentNode->natRoute(dev->mNetAddress);
//                    if (addr != 0x7F)
//                    {
//                        ByteArray ba;
//                        ba.append(addr);
//                        mAdjacentNode->acceptServiceMessage(netaddr, svcConnected, &ba);
//                    }
//                }
                //unsigned char mac = msg.localId().mac;
//                if (mLocalnetDevices[mac] == dev) // if device is in local network
                dev->mInfoValidCnt = 0;
                if (mSwonbMode)
                {
                    // request all info manually:
                    sendServiceMessage(netaddr, svcClass);
                    sendServiceMessage(netaddr, svcName);
                    sendServiceMessage(netaddr, svcFullName);
                    sendServiceMessage(netaddr, svcSerial);
                    sendServiceMessage(netaddr, svcVersion);
                    sendServiceMessage(netaddr, svcBuildDate);
                    sendServiceMessage(netaddr, svcCpuInfo);
                    sendServiceMessage(netaddr, svcBurnCount);
                    sendServiceMessage(netaddr, svcObjectCount);
                }
                else
                {
                    sendServiceMessage(netaddr, svcRequestAllInfo);
                    sendServiceMessage(netaddr, svcRequestObjInfo);
                    #ifdef QT_CORE_LIB
                    emit devConnected(dev->mNetAddress);
                    #else
                    if (onDevConnected)
                        onDevConnected(dev->mNetAddress);
                    #endif
                }
            }
            dev->mPresent = true;
            dev->mTimeout = 10;
        }
        break;

      case svcHello:
      {
        SvcOID welcomeCmd = svcWelcomeAgain;             // если девайс уже добавлен, команда будет svcWelcomeAgain
        ByteArray ba = msg.data();
        unsigned char mac = ba[0];
        bool localnet = (ba.size() == 1);
        if (localnet)
            dev = mLocalnetDevices[mac];
        unsigned char tempaddr = netaddr;
        if (!dev || !localnet)                           // сначала добавляем девайс с маком, который в id-шнике, если он ещё не добавлен
        {
            if (!localnet)
            {
                mac = route(tempaddr);
                netaddr = createNetAddress(mac);  // создаём новый адрес
            }
            else if (mNetAddrByMacCache[mac])     // если девайс в текущей подсети
            {
                netaddr = mNetAddrByMacCache[mac];// пытаемся вспомнить адрес по маку
                mRouteTable[netaddr] = mac;
            }
            else
            {
                netaddr = createNetAddress(mac);  // создаём новый адрес
            }

            dev = new ObjnetDevice(netaddr);             // создаём объект с новым адресом
            dev->mMaster = this;
            dev->mAutoDelete = true;                     // раз автоматически создали - автоматически и удалим)
            dev->mBusAddress = mac;
            dev->masterRequestObject = EVENT(&ObjnetMaster::requestObject);
            dev->masterSendObject = EVENT(&ObjnetMaster::sendObject);
            dev->masterServiceRequest = EVENT(&ObjnetMaster::sendServiceRequest);
            if (mSwonbMode)
            {
                dev->mBusType = BusSwonb;
            }
            if (localnet)                                // если девайс в текущей подсети...
            {
                mLocalnetDevices[mac] = dev;             // ...запоминаем для поиска по маку
                mNetAddrByMacCache[mac] = netaddr;       // и кэшируем адрес для возврата по маку (чтобы лишний раз не создавать)
            }
#ifndef _MSC_VER
#warning NEED TO IMPLEMENT: kill mLocalnetDevices[mac] on svcKill. A mb i ne nado!!
#endif
            mDevices[netaddr] = dev;                     // запоминаем для поиска по адресу
            welcomeCmd = svcWelcome;                     // меняем команду на svcWelcome

            #ifdef QT_CORE_LIB
            QObject::connect(dev, SIGNAL(requestObject(unsigned char,unsigned char)), SLOT(requestObject(unsigned char,unsigned char)));
            QObject::connect(dev, SIGNAL(sendObject(unsigned char,unsigned char,QByteArray)), SLOT(sendObject(unsigned char,unsigned char,QByteArray)));
            QObject::connect(dev, SIGNAL(serviceRequest(unsigned char,SvcOID,QByteArray)), SLOT(sendServiceRequest(unsigned char,SvcOID,QByteArray)));
            #endif
        }

        if (dev)
        {
            netaddr = dev->netAddress();
            dev->mLocalBusAddress = ba[0];
        }

        if (localnet)                          // если это девайс текущей подсети...
        {
            ByteArray outBa;
            outBa.append(netaddr);
            sendServiceMessage(netaddr, welcomeCmd, outBa);     // тупо отправляем сообщение с присвоенным адресом
            ba.append(netaddr);                             // добавляем в конец созданный логический адрес узла
        }
        else
        {
            ByteArray outBa;
            outBa.append(netaddr);
            unsigned char subnetaddr = ba[1];       // узнаём его адрес в той подсети
            outBa.append(subnetaddr);
            sendServiceMessage(tempaddr, welcomeCmd, outBa);     // тупо отправляем сообщение с присвоенными адресами
            ba[1] = netaddr;                        // теперь здесь адрес в этой подсети
            ba.append(tempaddr);
            if (mAdjacentNode)
            {
                for (int i=2; i<ba.size(); i++)
                    ba[i] = mAdjacentNode->natRoute(ba[i]);
            }
        }

//        if (mAdjacentNode)
//        {
//            for (int i=1; i<ba.count(); i++)
//                ba[i] = mAdjacentNode->natRoute(tempaddr);                // меняем адрес в той подсети на адрес в этой подсети
//        }

//        ba.append(netaddr);                         // добавляем в конец созданный логический адрес текущего узла

        #ifdef QT_CORE_LIB
        emit devAdded(netaddr, ba);                 // устройство добавлено
        #else
        if (onDevAdded)
            onDevAdded(netaddr, ba);
        #endif

        if (mAdjacentNode && welcomeCmd != svcWelcomeAgain)                               // если мастер связан с узлом, то он не верхний
        {
            if (mAdjacentNode->isConnected())            // и если смежный узел подключён к своему мастеру
            {
                //ba[0] = mAdjacentNode->mBusAddress;                                // меняем физический адрес на свой
                mAdjacentNode->sendServiceMessage(svcHello, ba);    // и отправляем дальше
            }
        }

        break;
      }

      case svcConnected:
      {
        unsigned char netaddr2 = msg.data()[0];
        if (mAdjacentNode)
        {
            unsigned char addr = mAdjacentNode->natRoute(netaddr2);
            if (addr != 0x7F)
            {
                ByteArray ba;
                ba.append(addr);
                mAdjacentNode->acceptServiceMessage(0, svcConnected, &ba);
            }
        }
//#warning if (localnet) '// nado dobavit!'
//        sendServiceMessage(netaddr, svcRequestAllInfo);
        #ifdef QT_CORE_LIB
        emit devConnected(netaddr2);
        #else
        if (onDevConnected)
            onDevConnected(netaddr2);
        #endif
        break;
      }

      case svcDisconnected:
      {
        unsigned char netaddr = msg.data()[0];
        if (mAdjacentNode)
        {
            unsigned char addr = mAdjacentNode->natRoute(netaddr);
            if (addr != 0x7F)
            {
                ByteArray ba;
                ba.append(addr);
                mAdjacentNode->acceptServiceMessage(0, svcDisconnected, &ba);
                removeNatPair(addr, netaddr);
            }
        }
        #ifdef QT_CORE_LIB
        emit devDisconnected(netaddr);
        #else
        if (onDevDisconnected)
            onDevDisconnected(netaddr);
        #endif
        break;
      }

      case svcKill:
      {
        unsigned char netaddr = msg.data()[0];
        mRouteTable.erase(netaddr);
        if (mAdjacentNode)
        {
            unsigned char addr = mAdjacentNode->natRoute(netaddr);
            if (addr != 0x7F)
            {
                ByteArray ba;
                ba.append(addr);
                mAdjacentNode->acceptServiceMessage(0, svcKill, &ba);
                removeNatPair(addr, netaddr);
            }
        }
        #ifdef QT_CORE_LIB
        emit devRemoved(netaddr);
        #endif
        break;
      }

      case svcClass:
        if (dev)
            mDevices[netaddr]->setClassId(*reinterpret_cast<const unsigned long*>(msg.data().data()));
        break;
        
      case svcName:
        if (dev)
        {
            string name(msg.data().data(), msg.data().size());
            name.resize(strlen(name.c_str()));
            mDevices[netaddr]->setName(name);
            if (mSwonbMode)
            {
                #ifdef QT_CORE_LIB
                emit devConnected(dev->mNetAddress);
                #else
                if (onDevConnected)
                    onDevConnected(dev->mNetAddress);
                #endif
            }
        }
        break;

      case svcFullName:
        if (dev)
        {
            dev->mFullName = string(msg.data().data(), msg.data().size());
            dev->mInfoValidCnt++;
        }
        break;

      case svcSerial:
        if (dev)
        {
            dev->mSerial = *reinterpret_cast<const unsigned long*>(msg.data().data());
            dev->mInfoValidCnt++;
        }
        break;

      case svcVersion:
        if (dev)
        {
            dev->mVersion = *reinterpret_cast<const unsigned short*>(msg.data().data());
            dev->mInfoValidCnt++;
        }
        break;

      case svcBuildDate:
        if (dev)
        {
            dev->mBuildDate = string(msg.data().data(), msg.data().size());
            dev->mInfoValidCnt++;
        }
        break;

      case svcCpuInfo:
        if (dev)
        {
            dev->mCpuInfo = string(msg.data().data(), msg.data().size());
            dev->mInfoValidCnt++;
        }
        break;

      case svcBurnCount:
        if (dev)
        {
            dev->mBurnCount = *reinterpret_cast<const unsigned long*>(msg.data().data());
            dev->mInfoValidCnt++;
        }
        break;

      case svcObjectCount:
        if (dev)
        {
            dev->mObjectCount = msg.data()[0];
            if (dev->mBusType == BusSwonb)
            {
                ByteArray oba;
                oba.resize(1);
                for (int i=0; i<dev->mObjectCount; i++)
                {
                    oba[0] = i;
                    sendServiceMessage(netaddr, svcObjectInfo, oba);
                }
            }
        }
        break;
        
      case svcBusType:
        if (dev)
        {
            dev->mBusType = (BusType)msg.data().data()[0];
            if (dev->mBusType == BusSwonb)
            {
                if (!dev->isInfoValid())
                {
                    // request all info manually:
                    sendServiceMessage(netaddr, svcClass);
                    sendServiceMessage(netaddr, svcName);
                    sendServiceMessage(netaddr, svcFullName);
                    sendServiceMessage(netaddr, svcSerial);
                    sendServiceMessage(netaddr, svcVersion);
                    sendServiceMessage(netaddr, svcBuildDate);
                    sendServiceMessage(netaddr, svcCpuInfo);
                    sendServiceMessage(netaddr, svcBurnCount);
                    sendServiceMessage(netaddr, svcObjectCount);
                    //sendServiceMessage(netaddr, svcBusType); // but why?
                }
            }
        }
        break;

      case svcObjectInfo:
        if (dev)
        {
            dev->prepareObject(msg.data());
        }
        break;

      case svcTimedObject:
        if (dev)
        {
            dev->receiveTimedObject(msg.data());
        }
        break;
        
      case svcGroupedObject:
        if (dev)
        {
            dev->receiveGroupedObject(msg.data());
        }
        break;

      case svcAutoRequest:
      case svcTimedRequest:
        if (dev)
        {
            int period = *reinterpret_cast<int*>(msg.data().data());
            unsigned char oid = msg.data()[4];
            if (oid < dev->mObjects.size())
            {
                #ifdef QT_CORE_LIB
                emit dev->autoRequestAccepted(dev->mObjects[oid]->name(), period);
                #endif
            }
        }

      default:; // warning elimination
    }

    if (oid < svcEcho)
    {
        #ifdef QT_CORE_LIB
        emit serviceMessageAccepted(netaddr, oid, msg.data());
        #endif
    }
}
//---------------------------------------------------------------------------

//void ObjnetMaster::sendRemoteMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba)
//{
//    ObjnetCommonNode::sendRemoteMessage(receiver, oid, ba, route(receiver));
//}
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
    {
        dev->receiveObject(oid, msg.data());
    }
    else
    {
        #ifdef QT_CORE_LIB
        //qDebug() << "object received from unknown device!!";
        #endif
    }
}
//---------------------------------------------------------------------------

unsigned char ObjnetMaster::createNetAddress(unsigned char mac)
{
    if (mRouteTable.size() >= 127) // сразу избегаем бесконечного цикла
        return 0x7F;

    while (mRouteTable.count(mAssignNetAddress)) // если в таблице маршрутизации данный адрес занят, ищем дальше
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

ObjnetDevice *ObjnetMaster::deviceBySerial(unsigned long serial)
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

void ObjnetMaster::addDevice(unsigned char mac, ObjnetDevice *dev)
{
    dev->mNetAddress = createNetAddress(mac);   // создаём объект с новым адресом
    dev->mAutoDelete = false;                   // автоматически не удаляется, т.к. создан внешним объектом
    mDevices[mac] = dev;                        // запоминаем для поиска по маку
    #ifdef QT_CORE_LIB
    emit devAdded(dev->mNetAddress, ByteArray().append(mac));
    #endif
}
//---------------------------------------------------------------------------

void ObjnetMaster::requestObject(unsigned char netAddress, unsigned char oid)
{
    sendMessage(netAddress, oid);
//    unsigned char mac = route(netAddress);
//    if (mDevices[mac])
//    {
//        
//    }
}

void ObjnetMaster::sendObject(unsigned char netAddress, unsigned char oid, const ByteArray &ba)
{
    sendMessage(netAddress, oid, ba);
}
//---------------------------------------------------------------------------
