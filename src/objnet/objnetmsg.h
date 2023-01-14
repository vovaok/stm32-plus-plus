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
    uint32_t mId;

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

//    virtual ~CommonMessage()
//    {
//    }

    uint32_t rawId() const {return mId;}
    void setId(uint32_t rawId) {mId = rawId;}
    LocalMsgId localId() const {return reinterpret_cast<const LocalMsgId&>(mId);}
    void setLocalId(LocalMsgId id) {mId = reinterpret_cast<int&>(id);}
    GlobalMsgId globalId() const {return reinterpret_cast<const GlobalMsgId&>(mId);}
    void setGlobalId(GlobalMsgId id) {mId = reinterpret_cast<int&>(id);}
    bool isLocal() const {return ((LocalMsgId&)mId).local;}
    bool isGlobal() const {return !((LocalMsgId&)mId).local;}
    ByteArray &data() {return mBa;}
    const ByteArray &data() const {return mBa;}
    void setData(const ByteArray &ba) {mBa = ba;}
    int size() const {return mBa.size();}
};
//---------------------------------------------------------------------------

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
    CommonMessageBuffer();

    void addPart(ByteArray &ba, int maxsize);
    unsigned char damage(unsigned char points=1);

    inline bool isReady() const {return (mParts & mPartsMask) == mPartsMask;}
};
//---------------------------------------------------------------------------

}

#endif
