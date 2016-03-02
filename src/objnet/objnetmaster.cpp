#include "objnetmaster.h"

using namespace Objnet;

//unsigned char ObjnetDeviceTreeNode::mAddressToAssign = 0;

ObjnetMaster::ObjnetMaster(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mAssignNetAddress(1),
    mAdjIfConnected(false)
{
    for (int i=0; i<16; i++)
        mLocalnetDevices[i] = 0L;
    setBusAddress(0);
    mNetAddress = 0x00;
    #ifdef __ICCARM__
    mTimer.setTimeoutEvent(EVENT(&ObjnetMaster::onTimer));
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
            sendGlobalServiceMessage(aidConnReset); // reset subnet state on adjacent node connection
        mAdjIfConnected = mAdjacentNode->isConnected(); // store previous value of connection state
    }

    if (mRouteTable.size() >= 127) // если вся таблица маршрутизации заполнена, значит что-то пошло не так, и...
    {
        mRouteTable.clear();                    // чистим таблицу маршрутизации
        mRouteTable[0x00] = 0;                  // сразу записываем, как достучаться до верхнего уровня
        sendGlobalServiceMessage(aidConnReset); // выполняем перенумерацию
    }
}

void ObjnetMaster::onTimer()
{
    sendGlobalServiceMessage(aidPollNodes);
    std::vector<unsigned char> macsToRemove;
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
                macsToRemove.push_back(it->first);
            }
            else
            {
                if (mAdjacentNode)
                {
                    ByteArray ba;
                    ba.append(dev->mNetAddress);
                    mAdjacentNode->acceptServiceMessage(0, svcDisconnected, &ba);
                }
                #ifndef __ICCARM__
                emit devDisconnected(dev->mNetAddress);
                #endif
            }
        }
    }
    // удаляем отсутствующие девайсы, если у них включено автоудаление
    for (std::vector<unsigned char>::iterator it=macsToRemove.begin(); it!=macsToRemove.end(); it++)
    {
        ObjnetDevice *dev = mDevices[*it];
        if (mAdjacentNode)
        {
            unsigned char supernetaddr = mAdjacentNode->natRoute(dev->mNetAddress);
            ByteArray ba;
            ba.append(supernetaddr);
            mAdjacentNode->acceptServiceMessage(0, svcKill, &ba);
            removeNatPair(supernetaddr, dev->mNetAddress);
        }
        #ifndef __ICCARM__
        emit devRemoved(dev->mNetAddress);
        #endif
        mRouteTable.erase(dev->mNetAddress);
        delete dev;
        mDevices.erase(*it);
    }
}
//---------------------------------------------------------------------------

void ObjnetMaster::acceptServiceMessage(unsigned char sender, SvcOID oid, ByteArray *ba)
{
//    #ifndef __ICCARM__
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
                mAdjacentNode->sendServiceMessage(sender, svcConnected, ba);
//            }
        }
        break;
      }

      default:; // warning elimination
    }
}

void ObjnetMaster::parseServiceMessage(CommonMessage &msg)
{
    SvcOID oid = (SvcOID)msg.localId().oid;

//    #ifndef __ICCARM__
//    qDebug() << "master" << QString::fromStdString(mName) << "parse" << oid;
//    #endif

    unsigned char netaddr = msg.localId().sender;
    ObjnetDevice *dev = mDevices.count(netaddr)? mDevices[netaddr]: 0L;
    switch (oid)
    {
      case svcEcho:
//        #ifndef __ICCARM__
//        qDebug() << "master" << QString::fromStdString(mName) << "parse echo from" << netaddr;
//        #endif
        if (!dev)
        {
            sendServiceMessage(netaddr, svcHello); // reset node's connection state
        }
        else
        {
            if (!dev->mPresent)
            {
                if (mAdjacentNode)
                {
                    unsigned char addr = mAdjacentNode->natRoute(dev->mNetAddress);
                    if (addr != 0x7F)
                    {
                        ByteArray ba;
                        ba.append(addr);
                        mAdjacentNode->acceptServiceMessage(netaddr, svcConnected, &ba);
                    }
                }
                //unsigned char mac = msg.localId().mac;
//                if (mLocalnetDevices[mac] == dev) // if device is in local network
                dev->mInfoValidCnt = 0;
                sendServiceMessage(netaddr, svcRequestAllInfo);
                sendServiceMessage(netaddr, svcRequestObjInfo);
                #ifndef __ICCARM__
                emit devConnected(dev->mNetAddress);
                #endif
            }
            dev->mPresent = true;
            dev->mTimeout = 5;
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
                mac = route(tempaddr);
            netaddr = createNetAddress(mac);             // создаём новый адрес
            dev = new ObjnetDevice(netaddr);             // создаём объект с новым адресом
            dev->mAutoDelete = true;                     // раз автоматически создали - автоматически и удалим)
            if (localnet)                                // если девайс в текцщей подсети...
                mLocalnetDevices[mac] = dev;             // ...запоминаем для поиска по маку
#warning NEED TO IMPLEMENT: kill mLocalnetDevices[mac] on svcKill. A mb i ne nado!!
            mDevices[netaddr] = dev;                     // запоминаем для поиска по адресу
            welcomeCmd = svcWelcome;                     // меняем команду на svcWelcome

            #ifndef __ICCARM__
            QObject::connect(dev, SIGNAL(requestObject(unsigned char,unsigned char)), SLOT(requestObject(unsigned char,unsigned char)));
            QObject::connect(dev, SIGNAL(sendObject(unsigned char,unsigned char,QByteArray)), SLOT(sendObject(unsigned char,unsigned char,QByteArray)));
            QObject::connect(dev, SIGNAL(serviceRequest(unsigned char,SvcOID,QByteArray)), SLOT(sendServiceRequest(unsigned char,SvcOID,QByteArray)));
            #endif
        }

        if (dev)
            netaddr = dev->netAddress();

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

        #ifndef __ICCARM__
        emit devAdded(netaddr, ba);                 // устройство добавлено
        #endif

        if (mAdjacentNode)                               // если мастер связан с узлом, то он не верхний
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
        #ifndef __ICCARM__
        emit devConnected(netaddr2);
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
        #ifndef __ICCARM__
        emit devDisconnected(netaddr);
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
        #ifndef __ICCARM__
        emit devRemoved(netaddr);
        #endif
        break;
      }

      case svcClass:
        if (dev)
            mDevices[netaddr]->setClassId(*reinterpret_cast<const unsigned long*>(msg.data().data()));

      case svcName:
        if (dev)
            mDevices[netaddr]->setName(string(msg.data().data(), msg.data().size()));
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
        dev->mObjectCount = msg.data()[0];
        //sendServiceMessage(netaddr, svcObjectInfo);
        break;

      case svcObjectInfo:
        if (dev)
        {
            dev->prepareObject(msg.data());
        }
        break;

      default:; // warning elimination
    }

    if (oid < svcEcho)
    {
        #ifndef __ICCARM__
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
        return;
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
        #ifndef __ICCARM__
        qDebug() << "object received from unknown device!!";
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

void ObjnetMaster::addDevice(unsigned char mac, ObjnetDevice *dev)
{
    dev->mNetAddress = createNetAddress(mac);   // создаём объект с новым адресом
    dev->mAutoDelete = false;                   // автоматически не удаляется, т.к. создан внешним объектом
    mDevices[mac] = dev;                        // запоминаем для поиска по маку
    #ifndef __ICCARM__
    emit devAdded(dev->mNetAddress, ByteArray().append(mac));
    #endif
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
