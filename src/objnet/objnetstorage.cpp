#include "objnetstorage.h"

using namespace Objnet;

ObjnetStorage* ObjnetStorage::mSelf = 0L;

ObjnetStorage::ObjnetStorage()
{
    mSelf = this;
}