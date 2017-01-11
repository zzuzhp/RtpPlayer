#if !defined(____PRO_THREAD_MUTEX_H____)
#define ____PRO_THREAD_MUTEX_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProThreadMutexCondition;
class CProThreadMutexConditionImpl;
class CProThreadMutexImpl;

/////////////////////////////////////////////////////////////////////////////
////

class CProThreadMutex : public CProCopyDisabled
{
public:
    
    CProThreadMutex();
    
    virtual ~CProThreadMutex();
    
    void Lock();
    
    void Unlock();
    
private:
    
    CProThreadMutexImpl* m_impl;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProRecursiveThreadMutex : public CProCopyDisabled
{
public:
    
    CProRecursiveThreadMutex();
    
    virtual ~CProRecursiveThreadMutex();
    
    void Lock();
    
    void Unlock();
    
private:
    
    unsigned long             m_ownerNestingLevel;
    unsigned long             m_ownerThreadId;
    unsigned long             m_waiters;
    CProThreadMutexCondition* m_cond;
    CProThreadMutex*          m_mutex;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProThreadMutexGuard : public CProCopyDisabled
{
public:
    
    CProThreadMutexGuard(CProThreadMutex& mutex);
    
    CProThreadMutexGuard(CProRecursiveThreadMutex& rmutex);
    
    virtual ~CProThreadMutexGuard();
    
private:
    
    CProThreadMutex*          m_mutex;
    CProRecursiveThreadMutex* m_rmutex;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProThreadMutexCondition : public CProCopyDisabled
{
public:
    
    CProThreadMutexCondition();
    
    virtual ~CProThreadMutexCondition();
    
    ////
    //// mutex可以为NULL
    ////
    void Wait(CProThreadMutex* mutex);
    
    ////
    //// rmutex可以为NULL
    ////
    void Waitr(CProRecursiveThreadMutex* rmutex);
    
    void Signal();
    
private:
    
    CProThreadMutexConditionImpl* m_impl;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_THREAD_MUTEX_H____
