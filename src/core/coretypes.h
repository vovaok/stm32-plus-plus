#include <stdint.h>

#include "core/closure.h"
#include "core/bytearray.h"
#include "core/qvector3d.h"

typedef Closure<void(void)> TaskEvent;
typedef Closure<void(int)> TickEvent;
typedef Closure<void(void)> NotifyEvent;
typedef Closure<void(ByteArray &)> DataEvent;
typedef Closure<void(const ByteArray &)> ConstDataEvent;

//class NotifyEventList
//{
//public:
//    NotifyEventList();
//    NotifyEventList &operator <<(const NotifyEvent &e)
//    {
//        for (NotifyEventList *list = this; list->m_next; list = list->m_next);
//            list->m_next = NotifyEvent(other);
//    }
//    void operator()();
//    
//private:
//    NotifyEvent m_event;
//    NotifyEventList *m_next = nullptr;
//};