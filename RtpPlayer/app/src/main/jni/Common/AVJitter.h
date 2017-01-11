#ifndef ___AVJITTER_H___
#define ___AVJITTER_H___

#include "common.h"
#include "pro_time_util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class AVJitter
{
public:

    AVJitter(int clock) : m_clock(clock),
                          m_send(0),
                          m_arrival(0),
                          m_jitter(0.),
                          m_first(true)
    {

    }

    ~AVJitter()
    {

    }

    uint32_t jitter_ms(uint32_t ts)
    {
        PRO_INT64 arrival = ProGetTickCount64() * m_clock / 1000;

        if (!m_first)
        {
            PRO_INT64 d = (arrival - m_arrival) - (ts - m_send);
            if (d < 0)
            {
                d = -d;
            }

            m_jitter += (1./16.) * ((double)d - m_jitter);
        }

        m_send      = ts;
        m_arrival   = arrival;
        m_first     = false;

        return (uint32_t)(m_jitter * 1000 / m_clock);
    }

private:

    PRO_INT64       m_arrival;

    uint32_t        m_send;

    double          m_jitter;

    int             m_clock;

    bool            m_first;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVJITTER_H___
