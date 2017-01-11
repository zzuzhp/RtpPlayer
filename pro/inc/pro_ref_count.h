#if !defined(____PRO_REF_COUNT_H____)
#define ____PRO_REF_COUNT_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_thread_mutex.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProRefCount : public CProCopyDisabled
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef();
    
    virtual unsigned long PRO_STDCALL Release();
    
protected:
    
    CProRefCount()
    {
        m_refCount = 1;
    }
    
    virtual ~CProRefCount()
    {
    }
    
private:
    
    unsigned long   m_refCount;
    
#if !defined(WIN32) && !defined(_WIN32_WCE)
    CProThreadMutex m_lock;
#endif
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_REF_COUNT_H____
