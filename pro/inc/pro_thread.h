#if !defined(____PRO_THREAD_H____)
#define ____PRO_THREAD_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_thread_mutex.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#if defined(__TI_COMPILER_VERSION__)
#include <ti/sysbios/knl/Task.h>
#endif

#include <vector>

/////////////////////////////////////////////////////////////////////////////
////

class CProThreadBase : public CProCopyDisabled
{
protected:
    
    CProThreadBase();
    
    virtual ~CProThreadBase();
    
    bool Spawn(bool realtime);
    
    void Wait();
    
    virtual void Svc() = 0;
    
private:
    
#if defined(_WIN32_WCE)
    static unsigned long PRO_STDCALL SvcRun(void* arg);
#elif defined(WIN32)
    static unsigned int PRO_STDCALL SvcRun(void* arg);
#elif defined(__TI_COMPILER_VERSION__)
    static void SvcRun(unsigned int arg, unsigned int);
#else
    static void* SvcRun(void* arg);
#endif
    
private:
    
    unsigned long            m_threadCount;
    bool                     m_realtime;
    CProThreadMutexCondition m_cond;
    CProThreadMutex          m_lock;
    
#if defined(__TI_COMPILER_VERSION__)
    std::vector<Task_Handle> m_taskHandles;
#endif
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_THREAD_H____
