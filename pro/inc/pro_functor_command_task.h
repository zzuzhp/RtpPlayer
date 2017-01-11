#if !defined(____PRO_FUNCTOR_COMMAND_TASK_H____)
#define ____PRO_FUNCTOR_COMMAND_TASK_H____

#include "pro_a.h"
#include "pro_thread.h"
#include "pro_thread_mutex.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <list>
#include <set>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
////

class IProFunctorCommand;

/////////////////////////////////////////////////////////////////////////////
////

class CProFunctorCommandTask : public CProThreadBase
{
public:
    
    CProFunctorCommandTask();
    
    virtual ~CProFunctorCommandTask();
    
    bool Start(bool realtime = false, unsigned long threadCount = 1);
    
    void Stop();
    
    bool Put(IProFunctorCommand* command, bool blocking = false);
    
    unsigned long GetSize() const;
    
    const std::vector<unsigned long> GetThreadIds() const;
    
private:
    
    void StopMe();
    
    virtual void Svc();
    
private:
    
    unsigned long                  m_threadCountSum;
    unsigned long                  m_threadCountNow;
    bool                           m_wantExit;
    std::set<unsigned long>        m_threadIds;
    std::list<IProFunctorCommand*> m_commands;
    CProThreadMutexCondition       m_countCond;
    CProThreadMutexCondition       m_commandCond;
    mutable CProThreadMutex        m_lock;
    CProThreadMutex                m_lockInit;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_FUNCTOR_COMMAND_TASK_H____
