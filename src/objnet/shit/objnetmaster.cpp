#include "objnetmaster.h"

using namespace Objnet;

//unsigned char ObjnetDeviceTreeNode::mAddressToAssign = 0;

ObjnetMaster::ObjnetMaster(ObjnetInterface *iface) :
    ObjnetCommonNode(iface),
    mAssignNetAddress(1),
    mAdjIfConnected(false)
{
    setBusAddress(0xFF);
    mNetAddress = 0;
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
    mRouteTable[0x00] = 0; // сразу записываем, как достучатьс€ до верхнего уровн€
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
    
    if (mRouteTable.size() >= 127) // если вс€ таблица маршрутизации заполнена, значит что-то пошло не так, и...
    {
        mRouteTable.clear();                    // чистим таблицу маршрутизации
        mRouteTable[0x00] = 0;                  // сразу записываем, как достучатьс€ до верхнего уровн€
        sendGlobalServiceMessage(aidConnReset); // выполн€ем перенумерацию
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
                    mAdjacentNode->acceptServiceMessage(svcDisconnected, &ba);
                }
                #ifndef __ICCARM__
                emit devDisconnected(dev->mNetAddress);
                #endif
            }
        }
    }
    // удал€ем отсутствующие девайсы, если у них включено автоудаление
    for (std::vector<unsigned char>::iterator it=macsToRemove.begin(); it!=macsToRemove.end(); it++)
    {
        ObjnetDevice *dev = mDevices[*it];
        if (mAdjacentNode)
        {
            ByteArray ba;
            ba.append(dev->mNetAddress);
            mAdjacentNode->acceptServiceMessage(svcKill, &ba);
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

void ObjnetMaster::acceptServiceMessage(SvcOID oid, ByteArray *ba)
{
//    switch (oid)
//    {
//      case svcWelcome:
//      {
//        unsigned char netaddr = ba->data()[0];
//        unsigned char mac = ba->data()[ba->size()-1];
//        ObjnetDevice *dev = mDevices.count(mac)? mDevices[mac]: 0L;
//        if (dev)
//        {
//            dev->mNetAddress = netaddr;
//            mRouteTable[netaddr] = mac;
//        }
//        ByteArray ba2(ba->data(), ba->size()-1);
//        sendServiceMessage(oid, mac, ba2);
//        break;
//      }
//
//      default:; // warning elimination
//    }
}

void ObjnetMaster::parseServiceMessage(CommonMessage &msg)
{
    SvcOID oid = (SvcOID)msg.localId().oid;
    unsigned char mac = msg.localId().mac;
    ObjnetDevice *dev = mDevices.count(mac)? mDevices[mac]: 0L;
    switch (oid)
    {
#warning #error tut ne universal
      case svcEcho:
        if (!dev)
            sendServiceMessage(netAddrUniversal, svcHello); // reset node's connection state
        else
        {
            if (!dev->mPresent)
            {
                if (mAdjacentNode)
                {
                    ByteArray ba;
                    ba.append(dev->mNetAddress);
                    mAdjacentNode->acceptServiceMessage(svcConnected, &ba);
                }
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
        if (!dev)                                        // сначала добавл€ем девайс с маком, который в id-шнике, если он ещЄ не добавлен
        {
            dev = new ObjnetDevice(createNetAddress(mac)); // создаЄм объект с новым адресом  
            dev->mAutoDelete = true;                     // раз автоматически создали - автоматически и удалим)
            mDevices[mac] = dev;                         // запоминаем дл€ поиска по маку
            welcomeCmd = svcWelcome;                     // мен€ем команду на svcWelcome
            #ifndef __ICCARM__
            emit devAdded(dev->mNetAddress, ByteArray().append(mac));
            #endif
        }
        
        if (mAdjacentNode)                               // если мастер св€зан с узлом, то он не верхний
        {
            if (mAdjacentNode->isConnected() && !dev->mPresent) // и если смежный узел подключЄн к своему мастеру и девайс ещЄ не получил свой адрес
                mAdjacentNode->acceptServiceMessage(svcHello, &msg.data()); // отдаЄм ему сообщение на обработку
        }
        else if (msg.data().size() > 1)                  // иначе мастер €вл€етс€ верхним устройством в цепочке (тут может быть друга€ логика, но пока так)
        {                                                // если размер данных > 1, значит, это ретранслированный запрос
            ByteArray ba;
            unsigned char netaddr = createNetAddress(mac);
            ba.append(netaddr);                          // присваиваем новый сетевой адрес тому далЄкому узлу
            ba.append(msg.data().data(), msg.data().size()-1); // добавл€ем остатки сообщени€, отн€в ненужную инфу (мак-адрес текущего получател€)
            sendServiceMessage(netaddr, svcWelcome, ba);     // отправл€ем сообщение с присвоенным адресом
            #ifndef __ICCARM__
            emit devAdded(netaddr, msg.data());
            #endif
        }
        else
        {
            unsigned char netaddr = dev->mNetAddress;    // просто берЄм сетевой адрес из объекта
            ByteArray ba;
            ba.append(netaddr);
            sendServiceMessage(netaddr, welcomeCmd, ba);     // отправл€ем сообщение с присвоенным адресом
        }
        
        break;
      }
        
      case svcConnected:  
      {
        unsigned char netaddr = msg.data()[0];
        if (mAdjacentNode)
            mAdjacentNode->acceptServiceMessage(svcConnected, &msg.data());
        #ifndef __ICCARM__
        emit devConnected(netaddr);
        #endif
        break;
      }
        
      case svcDisconnected:
      {
        unsigned char netaddr = msg.data()[0];
        if (mAdjacentNode)
            mAdjacentNode->acceptServiceMessage(svcDisconnected, &msg.data());
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
            mAdjacentNode->acceptServiceMessage(svcKill, &msg.data());
        #ifndef __ICCARM__
        emit devRemoved(netaddr);
        #endif
        break;
      }
      
      case svcClass:
        if (dev)
            mDevices[mac]->setClassId(*reinterpret_cast<const unsigned long*>(msg.data().data()));

      case svcName:
        if (dev)
            mDevices[mac]->setName(string(msg.data().data(), msg.data().size()));
        break;

      case svcFullName:
        if (dev)
            mDevices[mac]->mFullName = string(msg.data().data(), msg.data().size());
        break;

      default:; // warning elimination
    }
    
    if (oid < svcEcho)
    {
        #ifndef __ICCARM__
        emit serviceMessageAccepted(msg.localId().sender, oid, msg.data());
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
  
}
//---------------------------------------------------------------------------

unsigned char ObjnetMaster::createNetAddress(unsigned char mac)
{
    if (mRouteTable.size() >= 127) // сразу избегаем бесконечного цикла
        return 0x7F;
    while (mRouteTable.count(mAssignNetAddress)) // если в таблице маршрутизации данный адрес зан€т, ищем дальше
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
    dev->mNetAddress = createNetAddress(mac);   // создаЄм объект с новым адресом  
    dev->mAutoDelete = false;                   // автоматически не удал€етс€, т.к. создан внешним объектом
    mDevices[mac] = dev;                        // запоминаем дл€ поиска по маку
    #ifndef __ICCARM__
    emit devAdded(dev->mNetAddress, ByteArray().append(mac));
    #endif
}
//---------------------------------------------------------------------------
