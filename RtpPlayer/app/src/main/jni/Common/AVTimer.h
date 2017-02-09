#ifndef ___AVTIMER_H___
#define ___AVTIMER_H___

#include "Common/AVClock.h"
#include "Common/Timer.h"

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
            start_clock(0);
        }
    }

    /* begins timing, optionally allowing a relative offset */
    void start_clock(int offset_ms)
    {
        m_start_time = Timer::now() - offset_ms;
        m_stopped = false;
    }

    /* ends timing */
    void stop_clock()
    {
        if (!m_stopped)
        {
            m_end_time = Timer::now();
            m_stopped = true;
        }
    }

    /* resumes timing without resetting the timer */
    void resume_clock()
    {
        start_clock(clock_ms());
    }

    /* returns whether the timer is currently running */
    bool stopped() const { return m_stopped; }

    /* returns the elapsed time if the timer is running, or
     * the total time between calls to start() and stop() if it is stopped.
     */
    int clock_ms()
    {
        if (m_stopped)
        {
            return m_end_time - m_start_time;
        }

        return Timer::now() - m_start_time;
    }

private:

    bool        m_stopped;

    int64_t     m_start_time;

    int64_t     m_end_time;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVTIMER_H___
