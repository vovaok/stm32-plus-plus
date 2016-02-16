#include "objnetold.h"

ObjnetOld::ObjnetOld(CanInterface *canInterface) :
    mCan(canInterface)
{
    ObjId id, mask;
    mask.dword = 0;
    mask.not_global = 1;
    id.dword = 0;
    id.not_global = 1;
    mCan->addFilter(id.dword, mask.dword);
    
    id.not_global = 0;
    mCan->addFilter(id.dword, mask.dword);
    
    stmApp()->registerTaskEvent(EVENT(&ObjnetOld::task));
}

void ObjnetOld::task()
{
    CommonMessage inMsg;
    while (mCan->read(inMsg))
    {
        ObjMessage objmsg;
        objmsg.id.dword = inMsg.rawId();
        objmsg.ba = inMsg.data();
        mMessageEvent(objmsg);
    }
}

void ObjnetOld::sendSvc(unsigned char address, unsigned char cmd, const ByteArray &ba)
{
    ObjId id;
    id.dword = 0;
    id.cmd = cmd;
    id.group = 15;
    id.address = address;
    id.version = 1;
    id.full = 1;
    id.not_global = 1;
    CommonMessage msg;
    msg.setId(id.dword);
    msg.setData(ba);
    mCan->write(msg);
}

void ObjnetOld::sendLocal(unsigned char address, unsigned char cmd, const ByteArray &ba)
{
    ObjId id;
    id.dword = 0;
    id.cmd = cmd;
    id.group = 0;
    id.address = address;
    id.version = 1;
    id.full = 1;
    id.not_global = 1;
    CommonMessage msg;
    msg.setId(id.dword);
    msg.setData(ba);
    mCan->write(msg);
}