#if !defined(____PRO_TIMER_FACTORY_H____)
#define ____PRO_TIMER_FACTORY_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_thread_mutex.h"
#include "pro_z.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <map>
#include <set>

/////////////////////////////////////////////////////////////////////////////
////

class CProFunctorCommandTask;
class IProOnTimer;

struct PRO_TIMER_NODE
{
    PRO_TIMER_NODE()
    {
        expireTick = 0;
        timerId    = 0;
        
        onTimer    = NULL;
        timeSpan   = 0;
        recurring  = false;
    }
    
    bool operator<(const PRO_TIMER_NODE& node) const
    {
        if (expireTick < node.expireTick)
        {
            return (true);
        }
        
        if (expireTick > node.expireTick)
        {
            return (false);
        }
        
        if (timerId < node.timerId)
        {
            return (true);
        }
        
        return (false);
    }
    
    PRO_INT64     expireTick;
    unsigned long timerId;
    
    IProOnTimer*  onTimer;
    PRO_INT64     timeSpan;
    bool          recurring;
};

/////////////////////////////////////////////////////////////////////////////
////

class IProOnTimer
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    virtual void PRO_STDCALL OnTimer(unsigned long timerId) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProTimerFactory : public CProCopyDisabled
{
public:
    
    CProTimerFactory();
    
    virtual ~CProTimerFactory();
    
    bool Start(bool mmTimer);
    
    void Stop();
    
    unsigned long ScheduleTimer(
        IProOnTimer* onTimer,
        PRO_UINT64   timeSpan,
        bool         recurring
        );
    
    void CancelTimer(unsigned long timerId);
    
private:
    
    void WorkerRun(long* args);
    
private:
    
    CProFunctorCommandTask*            m_task;
    bool                               m_wantExit;
    bool                               m_mmTimer;
    unsigned long                      m_mmResolution;
    std::set<PRO_TIMER_NODE>           m_timers;
    std::map<unsigned long, PRO_INT64> m_timerId2ExpireTick;
    CProThreadMutexCondition           m_cond;
    CProThreadMutex                    m_lock;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_TIMER_FACTORY_H____
