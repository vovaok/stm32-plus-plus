#include "qdebug.h"
#include <stdio.h>

Debug *Debug::mSelf = 0L;

Debug &qDebug()
{
    return *Debug::instance();
}

void _qDebug_outfn(const char *s)
{
    printf(s);
}

Debug::OutFn_t Debug::mOutFn = _qDebug_outfn;

void Debug::setOutputFunc(OutFn_t f)
{
    mOutFn = f;
}