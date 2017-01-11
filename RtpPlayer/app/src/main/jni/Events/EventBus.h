#ifndef ___EVENTBUS_H___
#define ___EVENTBUS_H___

#include "Common/Utils.h"
#include "Events/EventQueue.h"
#include "pro_thread.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVModule;
class EventBus : public Singleton<EventBus>,
                 public CProThreadBase
{
    friend class Singleton<EventBus>;

public:

    void register_module(AVModule * module, int evt_masks);

    void deregister_module(AVModule * module);

    void post(EventSender      * sender,
              std::string        location,
              EventType          type,
              std::string        name,
              EventData        * data,
              bool               need_response);

private:

    EventBus();

    ~EventBus();

    void Svc();

private:

    typedef struct EvtModule
    {
        AVModule      * module;
        int             evt_mask;
    } EvtModule;

    volatile bool               m_exit;

    std::list<EvtModule *>      m_modules;

    EventQueue                * m_events;

    CProThreadMutexCondition    m_cond;

    CProThreadMutex             m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___EVENTBUS_H___
