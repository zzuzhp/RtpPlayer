#ifndef ___REFCOUNT_H___
#define ___REFCOUNT_H___

#include "pro_thread_mutex.h"
#include "Common/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class RefCount
{
public:

    unsigned long add_ref()
    {
        m_lock.Lock();
        const unsigned long ref_count = ++m_ref_count;
        m_lock.Unlock();

        return ref_count;
    }

    unsigned long release()
    {
        m_lock.Lock();
        const unsigned long ref_count = --m_ref_count;
        m_lock.Unlock();

        if (ref_count == 0)
        {
            delete this;
        }

        return ref_count;
    }

protected:

    RefCount()
    {
        m_ref_count = 1;
    }

    virtual ~RefCount(){}

private:

    unsigned long   m_ref_count;

    CProThreadMutex m_lock;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___REFCOUNT_H___
