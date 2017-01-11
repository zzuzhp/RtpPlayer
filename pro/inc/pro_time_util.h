#if !defined(____PRO_TIME_UTIL_H____)
#define ____PRO_TIME_UTIL_H____

#include "pro_a.h"
#include "pro_extern_c.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <string>

PRO_EXTERN_C1

/////////////////////////////////////////////////////////////////////////////
////

struct PRO_TIME_SPAN
{
    PRO_TIME_SPAN()
    {
        startingTime = 0;
        span         = 0;
    }
    
    bool operator<(const PRO_TIME_SPAN& timeSpan) const
    {
        if (startingTime + span <= timeSpan.startingTime) //// <=!!!
        {
            return (true);
        }
        
        return (false);
    }
    
    PRO_INT64 startingTime;
    PRO_INT64 span;
};

/////////////////////////////////////////////////////////////////////////////
////

PRO_INT64
PRO_STDCALL
ProGetTickCount64();

void
PRO_STDCALL
ProSleep(unsigned long milliseconds);

void
PRO_STDCALL
ProGetTimeText(std::string& timeText);

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____PRO_TIME_UTIL_H____
