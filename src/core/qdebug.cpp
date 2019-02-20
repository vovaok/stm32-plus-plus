#include "qdebug.h"

Debug *Debug::mSelf = 0L;

Debug &qDebug()
{
    return *Debug::instance();
}