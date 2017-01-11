#ifndef ___AVMODULE_H___
#define ___AVMODULE_H___

#include "Common/RefCount.h"
#include "Common/Utils.h"
#include "Events/EventBus.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

enum AVModuleType
{
    AV_MODULE_SOURCE        = 0x0100,
    AV_MODULE_TRANSFORMER   = 0x0200,
    AV_MODULE_RENDERER      = 0x0400
};

class AVModule : public EventRecipient,
                 public EventSender,
                 public RefCount
{
public:

    AVModule(AVModuleType type, const char * name, int evt_masks) : m_module_type(type)
    {
        strncpy(m_module_name, name, sizeof(m_module_name) - 1);

        EventBus::instance()->register_module(this, evt_masks);
    }

    virtual ~AVModule(){ EventBus::instance()->deregister_module(this); }

    void on_event(Event * event){ /* RP_LOG_D("module -- %s received event: %s", m_module_name, event->name.c_str()); */ }

    void on_event_dispatched(Event * event) { /* RP_LOG_D("module -- %s event: %s dispatched.", m_module_name, event->name.c_str()); */ }

    void on_event_response(EventResponse * response) { /* RP_LOG_D("module -- %s has been responsed.", m_module_name); */ }

    void post(EventType type, std::string name, EventData * data, bool need_response)
    {
        /* RP_LOG_D("module -- %s post event: %s", m_module_name, name.c_str()); */
        EventBus::instance()->post(this, WHERE_AM_I, type, name, data, need_response);
    }

    const AVModuleType module_type () const { return m_module_type; }

    const char * module_name() const { return m_module_name; }

private:

    AVModuleType        m_module_type;

    char                m_module_name[256];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVMODULE_H___
