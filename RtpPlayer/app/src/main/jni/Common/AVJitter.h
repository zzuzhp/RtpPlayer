#ifndef ___AVJITTER_H___
#define ___AVJITTER_H___

#include "Common/Timer.h"

#include <list>
#include <algorithm>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////////////////
////

struct Sum
{
    Sum() : m_sum(0) {}

    void operator() (int64_t i)
    {
        m_sum += i;
    }

    int64_t m_sum;
};

struct Variance
{
    Variance(double mean = 0) : m_mean(mean), m_var(0.), m_num(0) {}

    void operator() (int64_t i)
    {
        m_var += (i - m_mean) * (i - m_mean);
        ++m_num;
    }

    double std_dev() const { return sqrt(m_var / m_num); }

private:

    double  m_var;
    double  m_mean;
    int     m_num;
};

class AVJitter
{
public:

    AVJitter(int clock) : m_clock(clock),
                          m_send(0),
                          m_arrival(0),
                          m_jitter(0.),
                          m_first(true),
                          m_history_max_size(90)
    {
    }

    ~AVJitter()
    {
    }

    void push(uint32_t ts)
    {
        int64_t arrival = Timer::now() * m_clock / 1000;

        if (!m_first)
        {
            int64_t d = (arrival - m_arrival) - (ts - m_send);
            if (d < 0)
            {
                d = -d;
            }

            m_history.push_back(d * 1000 / m_clock);

            if (m_history.size() > m_history_max_size)
            {
                m_history.pop_front();

                Sum sum = std::for_each(m_history.begin(), m_history.end(), Sum());
                m_mean  = sum.m_sum * 1.0 / m_history.size();
                m_var   = std::for_each(m_history.begin(), m_history.end(), Variance(m_mean));
            }

            m_jitter += (1./16.) * ((double)d - m_jitter); ///< RTP's way of calculating jitter
        }

        m_send      = ts;
        m_arrival   = arrival;
        m_first     = false;
    }

    uint32_t jitter_ms()
    {
        return (uint32_t)(m_jitter * 1000 / m_clock);
    }

    double std_dev() const
    {
        return m_var.std_dev();
    }

    double mean() const
    {
        return m_mean;
    }

private:

    const int           m_history_max_size;

    std::list<int64_t>  m_history;

    double              m_mean;

    Variance            m_var;

    int64_t             m_arrival;

    uint32_t            m_send;

    double              m_jitter;

    int                 m_clock;

    bool                m_first;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___AVJITTER_H___
