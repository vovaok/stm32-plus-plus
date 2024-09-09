#include "objnetmsg.h"

using namespace Objnet;

CommonMessageBuffer::CommonMessageBuffer() : 
    CommonMessage(),
    mParts(0),
    mPartsMask(0xFFFF),
    mHealthPoints(100)
{
}

void CommonMessageBuffer::addPart(const ByteArray &ba, int maxsize)
{
    if (!ba.size())
        return;
    FragmentSignature signature;
    signature.byte = ba[0];
    int offset = signature.fragmentNumber * (maxsize-1);
    int size = ba.size() - 1;
    int newsize = offset + size;
    if (newsize > mBa.size())
        mBa.resize(newsize);
    for (int i=0; i<size; i++)
        mBa[offset+i] = ba[i+1];
    mParts |= (1 << signature.fragmentNumber);
    if (signature.lastFragment)
        mPartsMask = (1 << (signature.fragmentNumber + 1)) - 1;
    mHealthPoints = 100;
}

unsigned char CommonMessageBuffer::damage(int points)
{
    mHealthPoints -= mHealthPoints > points? points: mHealthPoints;
    return mHealthPoints;
}


CommonMessageBuffer &CommonMessageBufferList::operator[](uint32_t key)
{
    CommonMessageBufferList *b;
    for (b=this; b->m_next; b=b->m_next)
    {
        if (b->m_next->m_key == key)
            return *b->m_next;
    }
    
    CommonMessageBufferList *tmp;
#ifdef __ICCARM__
    __istate_t interrupt_state = __get_interrupt_state();
    __disable_interrupt();
    tmp = new CommonMessageBufferList;
    __set_interrupt_state(interrupt_state);
#else
    tmp = new CommonMessageBufferList;
#endif
    tmp->m_key = key;
    b->m_next = tmp;    
    return *tmp;
}

void CommonMessageBufferList::erase(uint32_t key)
{
    CommonMessageBufferList *b;
    for (b=this; b->m_next; b=b->m_next)
    {
        if (b->m_next->m_key == key)
            eraseNext(b);
    }
}

void CommonMessageBufferList::damage(int pts)
{ 
    CommonMessageBufferList *b;
    for (b=this; b->m_next; b=b->m_next)
    {
        if (0 == b->m_next->CommonMessageBuffer::damage(pts))
            eraseNext(b);
    }
}

void CommonMessageBufferList::eraseNext(CommonMessageBufferList *b)
{
    if (!b)
        return;
    CommonMessageBufferList *tmp = b->m_next;
    b->m_next = b->m_next->m_next;
#ifdef __ICCARM__
    __istate_t interrupt_state = __get_interrupt_state();
    __disable_interrupt();
    delete tmp;
    __set_interrupt_state(interrupt_state);
#else
    delete tmp;
#endif
}

int CommonMessageBufferList::count() const
{
    int cnt = 0;
    for (const CommonMessageBufferList *b = this; b->m_next; b = b->m_next)
        cnt++;
    return cnt;
}
