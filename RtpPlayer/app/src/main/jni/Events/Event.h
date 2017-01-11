#ifndef ___EVENT_H___
#define ___EVENT_H___

#include "Common/Utils.h"

#include <stdint.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct Event;
class AVModule;

enum EventType
{
    EVENT_STATS     = 0x0001,
    EVENT_QOS       = 0x0002,
    EVENT_ALL       = 0xffff
};

class EventData
{
public:

    EventData(){}

    virtual ~EventData(){}
};

class EventResponse
{
public:

    EventResponse(){}

    virtual ~EventResponse(){}
};

class EventSender
{
public:

    virtual ~EventSender(){};

    virtual void on_event_dispatched(Event * event) = 0;

    virtual void on_event_response(EventResponse * response) = 0;

    virtual void post(EventType type, std::string name, EventData * data, bool need_response) = 0;

protected:

    EventSender() {}
};

class EventRecipient
{
public:

    virtual ~EventRecipient(){};

    virtual void on_event(Event * event) = 0;

protected:

    EventRecipient() {}
};

struct Event
{
    EventSender     * from;

    std::string       location;
    int64_t           send_time;
    int64_t           done_time;

    EventType         type;
    EventData       * data;
    std::string       name;

    bool              need_response;
};

struct DelayedEvent
{
    bool operator < (const DelayedEvent & event) const
    {
        return (event.trigger < trigger) ||
               (event.trigger == trigger && event.num < num);
    }

    int64_t           delay;
    int64_t           trigger;
    uint32_t          num;
    Event             event;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___EVENT_H___
