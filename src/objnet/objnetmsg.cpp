#include "objnetmsg.h"

using namespace Objnet;

CommonMessageBuffer::CommonMessageBuffer() : 
    CommonMessage(),
    mParts(0),
    mPartsMask(0xFFFF),
    mHealthPoints(100)
{
}

void CommonMessageBuffer::addPart(ByteArray &ba, int maxsize)
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
}

unsigned char CommonMessageBuffer::damage(unsigned char points)
{
    mHealthPoints -= mHealthPoints > points? points: mHealthPoints;
    return mHealthPoints;
}
