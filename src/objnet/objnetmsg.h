#ifndef _OBJNETMSG_H
#define _OBJNETMSG_H

#include "objnetcommon.h"

namespace Objnet
{

/*! Common message handling class.
    It takes care of memory allocation for data buffer.
    Normally you should not create instance of it,
    use LocalMessage or GlobalMessage instead.
    Besides you can inherit your own message handling class based on Common message.
*/
class CommonMessage
{
protected:
    ByteArray mBa;
    unsigned long mId;

public:
    CommonMessage() :
        mId(0)
    {
    }

    CommonMessage(const ByteArray &ba) :
        mId(0)
    {
        mBa.append(ba);
    }

    virtual ~CommonMessage()
    {
    }

    unsigned long rawId() const {return mId;}
    void setId(unsigned long rawId) {mId = rawId;}
    LocalMsgId localId() {return reinterpret_cast<LocalMsgId&>(mId);}
    void setLocalId(LocalMsgId id) {mId = reinterpret_cast<int&>(id);}
    GlobalMsgId globalId() {return reinterpret_cast<GlobalMsgId&>(mId);}
    void setGlobalId(GlobalMsgId id) {mId = reinterpret_cast<int&>(id);}
    bool isGlobal() const {return !((LocalMsgId&)mId).local;}
    ByteArray &data() {return mBa;}
    void setData(const ByteArray &ba) {mBa = ba;}
};

class CommonMessageBuffer : public CommonMessage
{
public:
    typedef union
    {
        unsigned char byte;
        struct
        {
            unsigned char fragmentNumber: 4;
            unsigned char sequenceNumber: 3;
            unsigned char lastFragment: 1;
        };
    } FragmentSignature;

private:
    unsigned short mParts;
    unsigned short mPartsMask;
    unsigned char mHealthPoints;

public:
    CommonMessageBuffer() : CommonMessage(), mParts(0), mPartsMask(0xFFFF), mHealthPoints(100) {}

    void addPart(ByteArray &ba)
    {
        if (!ba.size())
            return;
        FragmentSignature signature;
        signature.byte = ba[0];
        int offset = signature.fragmentNumber * 7;
        int size = ba.size() - 1;
        int newsize = offset + size;
        if (newsize > mBa.size())
            mBa.resize(newsize);
        for (int i=0; i<size; i++)
            mBa[offset+i] = ba[i+1];
        mParts |= (1 << signature.fragmentNumber);
        if (signature.lastFragment)
            mPartsMask = (1 << (signature.fragmentNumber + 1)) - 1;
    }

    bool isReady() const {return (mParts & mPartsMask) == mPartsMask;}
    unsigned char damage(unsigned char points=1) {mHealthPoints -= mHealthPoints > points? points: mHealthPoints; return mHealthPoints;}
};
//---------------------------------------------------------------------------

}

#endif
