#ifndef ___EVENTQUEUE_H___
#define ___EVENTQUEUE_H___

#include "Event.h"

#include <list>
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class EventQueue
{
public:

    EventQueue(){}

    ~EventQueue(){}

    bool get(Event ** event, int wait_ms);

    void post(Event * event);

    void post_delayed(DelayedEvent * delayed_event);

private:

    std::list<Event *>     m_events;

    std::priority_queue<DelayedEvent *> m_delayed_events;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___EVENTQUEUE_H___
