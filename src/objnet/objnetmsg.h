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
    
    CommonMessage(const CommonMessage &other) :
        mBa(other.mBa),
        mId(other.mId)
    {
    }
    
    CommonMessage &operator=(const CommonMessage &other)
    {
        mBa = other.mBa;
        mId = other.mId;
        return *this;
    }
    
    CommonMessage(CommonMessage &&other)
    {
        mId = other.mId;
        mBa = std::move(other.mBa);
    }
    
    CommonMessage &operator=(CommonMessage &&other)
    {
        mId = other.mId;
        mBa = std::move(other.mBa);
        return *this;
    }

//    CommonMessage(const ByteArray &ba) :
//        mId(0)
//    {
//        mBa.append(ba);
//    }
    
//    CommonMessage(uint32_t id, const ByteArray &ba) :
//        mId(id), mBa(ba)
//    {
//    }
    
    CommonMessage(uint32_t id, ByteArray &&ba) :
        mBa(std::move(ba)), mId(id)
    {
    }
    
    CommonMessage(const ByteArray &ba) :
//        mBa(ByteArray::fromRawData(ba.data() + 4, ba.size() - 4)),
        mBa(ByteArray(ba.data() + 4, ba.size() - 4)),
        mId(*reinterpret_cast<const uint32_t*>(ba.data()))
    {
    }

//    virtual ~CommonMessage()
//    {
//    }

    uint32_t rawId() const {return mId;}
    void setId(uint32_t rawId) {mId = rawId;}
    LocalMsgId localId() const {return static_cast<const LocalMsgId>(mId);}
    void setLocalId(LocalMsgId id) {mId = static_cast<int>(id);}
    GlobalMsgId globalId() const {return static_cast<const GlobalMsgId>(mId);}
    void setGlobalId(GlobalMsgId id) {mId = reinterpret_cast<int&>(id);}
    inline bool isLocal() const {return localId().local;}
    inline bool isGlobal() const {return !isLocal();}
    ByteArray &data() {return mBa;}
    const ByteArray &data() const {return mBa;}
//    void setData(const ByteArray &ba) {mBa = ba;}
    void copyData(const ByteArray &ba) {mBa = ba;}
    void setData(ByteArray &&ba) {mBa = std::move(ba);}
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
    friend class CommonMessageBufferList;

public:
    CommonMessageBuffer();

    void addPart(const ByteArray &ba, int maxsize);
    unsigned char damage(int points=1);

    inline bool isReady() const {return (mParts & mPartsMask) == mPartsMask;}
};

class CommonMessageBufferList : public CommonMessageBuffer
{
public:
    CommonMessageBuffer &operator[](uint32_t key);
    void erase(uint32_t key);
    void damage(int pts=1);
    int count() const;
    
private:
    uint32_t m_key = 0;
    CommonMessageBufferList *m_next = nullptr;
    void eraseNext(CommonMessageBufferList *b);
};
//---------------------------------------------------------------------------

}

#endif
