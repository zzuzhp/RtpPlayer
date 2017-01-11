#include "EventQueue.h"

bool
EventQueue::get(Event ** event, int wait_ms)
{
    if (!m_events.empty())
    {
        *event = m_events.front();
        m_events.pop_front();
    }

    return true;
}

void
EventQueue::post(Event * event)
{
    m_events.push_back(event);
}
