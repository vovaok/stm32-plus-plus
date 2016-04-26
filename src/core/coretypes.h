#include "core/closure.h"
#include "core/bytearray.h"

typedef Closure<void(void)> TaskEvent;
typedef Closure<void(int)> TickEvent;
typedef Closure<void(void)> NotifyEvent;
typedef Closure<void(ByteArray &)> DataEvent;
typedef Closure<void(const ByteArray &)> ConstDataEvent;