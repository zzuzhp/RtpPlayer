#include "EventBus.h"
#include "Common/AVModule.h"
#include "pro_time_util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

EventBus::EventBus() : m_exit(false)
{
    m_events = new EventQueue();
    Spawn(false);
}

EventBus::~EventBus()
{
    m_exit = true;
    Wait();
}

void
EventBus::register_module(AVModule * module, int evt_mask)
{
    CProThreadMutexGuard mon(m_lock);

    EvtModule * evt_module = NULL;

    std::list<EvtModule *>::const_iterator citr = m_modules.begin();
    for (; citr != m_modules.end(); ++citr)
    {
        if ((*citr)->module == module)
        {
            break;
        }
    }

    if (citr != m_modules.end())
    {
        return;
    }

    evt_module = new EvtModule;

    evt_module->module   = module;
    evt_module->evt_mask = evt_mask;

    m_modules.push_back(evt_module);
}

void
EventBus::deregister_module(AVModule * module)
{
    CProThreadMutexGuard mon(m_lock);

    std::list<EvtModule *>::iterator itr = m_modules.begin();
    for (; itr != m_modules.end(); ++itr)
    {
        if ((*itr)->module == module)
        {
            delete *itr;
            m_modules.erase(itr);

            break;
        }
    }
}

void
EventBus::post(EventSender      * sender,
               std::string        location,
               EventType          type,
               std::string        name,
               EventData        * data,
               bool               need_response)
{
    CProThreadMutexGuard mon(m_lock);

    Event * event = new Event;

    event->from             = sender;
    event->location         = location;
    event->send_time        = ProGetTickCount64();
    event->done_time        = 0;
    event->name             = name;
    event->type             = type;
    event->data             = data;
    event->need_response    = need_response;

    m_events->post(event);

    m_cond.Signal();
}

void
EventBus::Svc()
{
    while (true)
    {
        Event * event = NULL;
        std::list<AVModule *> modules;

        {
            CProThreadMutexGuard mon(m_lock);

            while (!m_exit)
            {
                m_events->get(&event, 0);

                if (event)
                {
                    break;
                }

                m_cond.Wait(&m_lock);
            }

            if (m_exit)
            {
                break;
            }

            std::list<EvtModule *>::const_iterator citr = m_modules.begin();
            for (; citr != m_modules.end(); ++citr)
            {
                if ((*citr)->evt_mask & event->type)
                {
                    (*citr)->module->AddRef();
                    modules.push_back((*citr)->module);
                }
            }
        }

        /* dispatch */
        std::list<AVModule *>::const_iterator citr = modules.begin();
        for (; citr != modules.end(); ++citr)
        {
            dynamic_cast<EventRecipient *>(*citr)->on_event(event);
            dynamic_cast<EventSender *>(*citr)->on_event_dispatched(event);

            (*citr)->Release();
        }

        delete event;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
