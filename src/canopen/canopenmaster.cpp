#include "canopen.h"

//CanOpenMaster::CanOpenMaster(CanInterface *device, uint8_t address, QObject *parent) :
//    QObject(parent),
//    mCan(device),
//    mAddress(address & 0x7F)
//{
//    connect(device, SIGNAL(stdPacketReceived(unsigned short,QByteArray)), SLOT(receiveStdPacket(unsigned short,QByteArray)));
//}
//
//void CanOpenMaster::receiveStdPacket(unsigned short id, const QByteArray &ba)
//{
//    unsigned short cobid = id & 0x780;
//    unsigned char addr = id & 0x7F;
//    if (addr != mAddress)
//        return;
////    if (cobid == SDO_Response)
//    {
//        const SDO &sdo = *reinterpret_cast<const SDO*>(ba.data());
//        ErrorCode err = static_cast<ErrorCode>(ba[6]);
//
//        switch (sdo.cmd & 0xE3)
//        {
//          case cmdReadParamResponse:
//            paramReceived(sdo.id, sdo.subid, sdo.value);
//            break;
//
//          case cmdWriteParamResponse:
//            paramWritten(sdo.id, sdo.subid);
//            break;
//
//          case cmdError:
//            error(err);
//            break;
//        }
//    }
//}
//
//void CanOpenMaster::readParam(unsigned short id, unsigned char subid)
//{
//    SDO sdo;
//    sdo.cmd = cmdReadParam;
//    sdo.id = id;
//    sdo.subid = subid;
//    sdo.value = 0;
//    ByteArray ba = ByteArray(reinterpret_cast<const char*>(&sdo), 8);
//    mCan->sendStdPacket(SDO_Request | mAddress, ba);
//}
//
//void CanOpenMaster::writeParam(unsigned short id, unsigned char subid, unsigned long value)
//{
//    SDO sdo;
//    sdo.cmd = cmdWriteParam;
//    sdo.n = 4 - sizeof(value);
//    sdo.id = id;
//    sdo.subid = subid;
//    sdo.value = value;
//    QByteArray ba = QByteArray(reinterpret_cast<const char*>(&sdo), 8);
//    mCan->sendStdPacket(SDO_Request | mAddress, ba);
//}
