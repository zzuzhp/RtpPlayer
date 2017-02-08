#ifndef ___TIMER_H___
#define ___TIMER_H___

#include "pro_time_util.h"

#include <string>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////    timer utilities

class Timer
{
public:

    /* current time in ms */
    static int64_t now()
    {
        PRO_INT64 time = ProGetTickCount64();
        return (int64_t)time;
    }

    static std::string now_s()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        time_t curtime = tv.tv_sec;

        struct tm * timeinfo = localtime(&curtime);

        char time_string[64] = {0};

        sprintf(time_string, "%.2d:%.2d:%.2d:%03d",
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec,
                tv.tv_usec / 1000);

        return time_string;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___TIMER_H___
