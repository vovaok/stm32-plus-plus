#include "objnetCommonNode.h"

using namespace Objnet;

ObjnetCommonNode::ObjnetCommonNode(ObjnetInterface *iface) :
    mLocalFilter(-1),
    mGlobalFilter(-1),
    mInterface(iface),
//    hasSubnet(false),
    mAdjacentNode(0L),
    mBusAddress(0xFF),
    mNetAddress(0xFF),
    mConnected(false)
{
    #ifndef QT_CORE_LIB
    stmApp()->registerTaskEvent(EVENT(&ObjnetCommonNode::task));
    #else
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), SLOT(task()));
    timer->start(20);
    #endif
}

ObjnetCommonNode::~ObjnetCommonNode()
{
    #ifndef QT_CORE_LIB
    stmApp()->unregisterTaskEvent(EVENT(&ObjnetCommonNode::task));
    #endif
    delete mInterface;
}
//---------------------------------------------------------------------------

void ObjnetCommonNode::task()
{
    // не выполняем задачу, пока физический адрес неправильный
    if (mBusAddress == 0xFF)
        return;

    CommonMessage inMsg;
    while (mInterface->read(inMsg))
    {
        if (inMsg.isGlobal())
        {
            GlobalMsgId id = inMsg.globalId();
            if (id.aid & aidPropagationDown)
            {
//                if (mRetranslateEvent)
//                    mRetranslateEvent(inMsg);
                if (mAdjacentNode)
                    mAdjacentNode->mInterface->write(inMsg);
            }

            if (id.svc)
            {
                parseServiceMessage(inMsg);
            }
            else
            {
                #warning poka x3 s global messagami
                #ifndef QT_CORE_LIB
                if (mGlobalMessageEvent)
                    mGlobalMessageEvent(inMsg);
                #endif
                parseMessage(inMsg);
            }
        }
        else
        {
            LocalMsgId id = inMsg.localId();
            if (id.addr == mNetAddress || id.addr == 0x7F || mNetAddress == 0xFF) // logical address match OR universal address OR address is not assigned yet
            {
                if (id.frag) // if message is fragmented
                {
                    LocalMsgId key = id;
                    key.addr = inMsg.data()[0] & 0x70; // field "addr" of LocalId stores sequence number
                    CommonMessageBuffer &buf = mFragmentBuffer[key];
                    buf.setLocalId(id);
                    buf.addPart(inMsg.data(), mInterface->maxFrameSize());
                    if (buf.isReady())
                    {
                        if (id.svc)
                            parseServiceMessage(buf);
                        else
                            parseMessage(buf);
                        mFragmentBuffer.erase(key);
                    }
                }
                else
                {
                    if (id.svc)
                        parseServiceMessage(inMsg);
                    else
                        parseMessage(inMsg);
                }
            }
            else if (mAdjacentNode) // retranslate
            {
                if (id.mac) // not a master
                {
                    id.addr = mAdjacentNode->natRoute(id.addr);
                    id.mac = mAdjacentNode->route(id.addr);
                    id.sender++;
                }
                else // this is MAASTEEEERRRRR!!
                {
                    id.sender = mAdjacentNode->natRoute(id.sender);
                    --id.addr;
                }
                inMsg.setLocalId(id);
                mAdjacentNode->mInterface->write(inMsg);
            }

            std::list<unsigned long> toRemove;
            std::map<unsigned long, CommonMessageBuffer>::iterator it;
            for (it=mFragmentBuffer.begin(); it!=mFragmentBuffer.end(); it++)
                if (it->second.damage() == 0)
                    toRemove.push_back(it->first);

            for (std::list<unsigned long>::iterator it=toRemove.begin(); it!=toRemove.end(); it++)
                mFragmentBuffer.erase(*it);
            toRemove.clear();
        }
    }
}
//---------------------------------------------------------------------------

void ObjnetCommonNode::setBusAddress(unsigned char address)
{
    if (address > 0xF && address != 0xFF) // пресекаем попытку установки неправильного адреса
        return;

    if (mLocalFilter >= 0)
        mInterface->removeFilter(mLocalFilter);
    if (mGlobalFilter >= 0)
        mInterface->removeFilter(mGlobalFilter);

    mBusAddress = address;

    LocalMsgId lid, lmask;
    if (address != 0xFF)
    {
        lid.mac = mBusAddress;
        lmask.mac = 0xF;
    }
    mLocalFilter = mInterface->addFilter(lid, lmask);

    GlobalMsgId gid, gmask;
    gmask.local = 1;
    mGlobalFilter = mInterface->addFilter(gid, gmask);
}

#ifndef QT_CORE_LIB
void ObjnetCommonNode::setBusAddressFromPins(int bits, Gpio::PinName a0, ...)
{
    unsigned long address = 0;
    va_list vl;
    va_start(vl, a0);
    for (int i=0; i<bits; i++)
    {
        Gpio::PinName pinName = i? (Gpio::PinName)va_arg(vl, int): a0;
        Gpio pin(pinName, Gpio::Flags(Gpio::modeIn | Gpio::pullUp));
        unsigned long bit = pin.read()? 1: 0;
        address |= bit << i;
    }
    va_end(vl);
    setBusAddress(address);
}
#endif
//---------------------------------------------------------------------------

bool ObjnetCommonNode::sendCommonMessage(CommonMessage &msg)
{
    int maxsize = mInterface->maxFrameSize();
    if (msg.data().size() <= maxsize)
    {
        return mInterface->write(msg);
    }
    else
    {
        maxsize--;
        CommonMessage outMsg;
        LocalMsgId id = msg.localId();
        id.frag = 1;
        outMsg.setId(id);
        int fragments = ((msg.data().size() - 1) / maxsize) + 1;
        if (mInterface->availableWriteCount() < fragments)
            return false;
        for (int i=0; i<fragments; i++)
        {
            ByteArray ba;
            CommonMessageBuffer::FragmentSignature signature;
            signature.fragmentNumber = i;
            signature.sequenceNumber = mFragmentSequenceNumber;
            signature.lastFragment = (i == fragments-1);
            ba.append(signature.byte);
            int remainsize = msg.data().size() - i*maxsize;
            if (remainsize > maxsize)
                remainsize = maxsize;
            ba.append(msg.data().data() + i*maxsize, remainsize);
            outMsg.setData(ba);
            if (!mInterface->write(outMsg))
                return false;
        }
        mFragmentSequenceNumber++;
    }
    return true;
}
//---------------------------------------------------------------------------

//void ObjnetCommonNode::sendMessage(unsigned char oid, const ByteArray &ba, unsigned char mac)
//{
//    CommonMessage msg;
//    LocalMsgId id;
//    id.mac = mac==0xFF? mBusAddress: mac;
//    id.addr = 0x7F;
//    id.sender = mNetAddress;
//    id.oid = oid;
//    msg.setLocalId(id);
//    msg.setData(ba);
//    sendCommonMessage(msg);
//}

bool ObjnetCommonNode::sendMessage(unsigned char receiver, unsigned char oid, const ByteArray &ba)
{
    CommonMessage msg;
    LocalMsgId id;
    id.mac = route(receiver);
    id.addr = receiver;
    id.sender = mNetAddress;
    id.oid = oid;
    msg.setLocalId(id);
    msg.setData(ba);
    return sendCommonMessage(msg);
}

bool ObjnetCommonNode::sendServiceMessage(unsigned char receiver, SvcOID oid, const ByteArray &ba)
{
    CommonMessage msg;
    LocalMsgId id;
    id.mac = route(receiver);
    id.addr = receiver;
    id.svc = 1;
    id.sender = mNetAddress;
    id.oid = oid;
    msg.setLocalId(id);
    msg.setData(ba);
    return sendCommonMessage(msg);
}

bool ObjnetCommonNode::sendServiceMessage(SvcOID oid, const ByteArray &ba)
{
    CommonMessage msg;
    LocalMsgId id;
    id.mac = 0;
    id.addr = 0x7F;
    id.svc = 1;
    id.sender = mNetAddress;
    id.oid = oid;
    msg.setLocalId(id);
    msg.setData(ba);
    return sendCommonMessage(msg);
}

bool ObjnetCommonNode::sendGlobalMessage(unsigned char aid)
{
    CommonMessage msg;
    GlobalMsgId id;
    id.mac = mBusAddress;
    id.addr = mNetAddress;
    id.aid = aid;
    msg.setGlobalId(id);
    //msg.setData(ba);
    return mInterface->write(msg);
}

bool ObjnetCommonNode::sendGlobalServiceMessage(StdAID aid)
{
    CommonMessage msg;
    GlobalMsgId id;
    id.mac = mBusAddress;
    id.svc = 1;
    id.addr = mNetAddress;
    id.aid = aid;
    msg.setGlobalId(id);
    //msg.setData(ba);
    return mInterface->write(msg);
}
//---------------------------------------------------------------------------

void ObjnetCommonNode::addNatPair(unsigned char supernetAddr, unsigned char subnetAddr)
{
    if (mAdjacentNode && supernetAddr < 0x80 && subnetAddr < 0x80)
    {
        mNatTable[supernetAddr] = subnetAddr;
        mAdjacentNode->mNatTable[subnetAddr] = supernetAddr;
    }
}

void ObjnetCommonNode::removeNatPair(unsigned char supernetAddr, unsigned char subnetAddr)
{
    if (mAdjacentNode)
    {
        if (mNatTable.count(supernetAddr))
        {
            unsigned char addr = mNatTable[supernetAddr];
            mNatTable.erase(supernetAddr);
            mAdjacentNode->mNatTable.erase(addr);
        }
        if (mAdjacentNode->mNatTable.count(subnetAddr))
        {
            unsigned char addr = mAdjacentNode->mNatTable[subnetAddr];
            mAdjacentNode->mNatTable.erase(subnetAddr);
            mNatTable.erase(addr);
        }
    }
}
//---------------------------------------------------------------------------

//void ObjnetCommonNode::parseMessage(CommonMessage &msg)
//{
//}
//---------------------------------------------------------------------------

void ObjnetCommonNode::connect(ObjnetCommonNode *node)
{
    mAdjacentNode = node;
    node->mAdjacentNode = this;
}
//---------------------------------------------------------------------------
