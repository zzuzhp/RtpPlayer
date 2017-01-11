#ifndef ___AVTIMER_H___
#define ___AVTIMER_H___

#include "Common/AVClock.h"
#include "pro_time_util.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////    a simple timer

/* get elapsed time (I) + (III)
 *       (running) (I)      (stopped)(II)   (running)(III)
 * |---------------------->|=============|------------------>|
 */
class AVTimer : public AVClock
{
public:

    /* constructs a timer
     * which is initialized as stopped unless \a start_on_create is true
     */
    AVTimer(bool start_on_create = false) : m_stopped(true),
                                            m_start_time(-1),
                                            m_end_time(-1)
    {
        if (start_on_create)
        {
            start(0);
        }
    }

    /* begins timing, optionally allowing a relative offset */
    void start(int offset_ms)
    {
        if (m_stopped)
        {
            m_start_time = ProGetTickCount64() - offset_ms;
            m_stopped = false;
        }
    }

    /* ends timing */
    void stop()
    {
        if (!m_stopped)
        {
            m_end_time = ProGetTickCount64();
            m_stopped = true;
        }
    }

    /* resumes timing without resetting the timer */
    void resume()
    {
        start(time_ms());
    }

    /* returns whether the timer is currently running */
    bool stopped() const { return m_stopped; }

    /* returns the elapsed time if the timer is running, or
     * the total time between calls to start() and stop() if it is stopped.
     */
    int time_ms()
    {
        if (m_stopped)
        {
            return m_end_time - m_start_time;
        }

        return ProGetTickCount64() - m_start_time;
    }

private:

    bool        m_stopped;

    PRO_INT64   m_start_time;

    PRO_INT64   m_end_time;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVTIMER_H___
