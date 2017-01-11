#if !defined(____PRO_MEMORY_POOL_H____)
#define ____PRO_MEMORY_POOL_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_extern_c.h"
#include "pro_thread_mutex.h"
#include "pro_z.h"

/////////////////////////////////////////////////////////////////////////////
////

class CFixedAlloc;

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C1

CFixedAlloc*
PRO_STDCALL
ProCreateFixedAlloc_i(unsigned long objSize,
                      unsigned long objPerChunk);

void*
PRO_STDCALL
ProFixedAllocate_i(CFixedAlloc* fixedAlloc);

void
PRO_STDCALL
ProFixedDeallocate_i(CFixedAlloc* fixedAlloc,
                     void*        p);

void
PRO_STDCALL
ProDeleteFixedAlloc_i(CFixedAlloc* fixedAlloc);

#if defined(_MSC_VER)

void*
PRO_STDCALL
ProAllocate_i(unsigned long size);

void
PRO_STDCALL
ProDeallocate_i(void*         p,
                unsigned long size);

#endif //// _MSC_VER

PRO_EXTERN_C2

/////////////////////////////////////////////////////////////////////////////
////

class IProMemoryPool
{
public:
    
    virtual void* PRO_STDCALL Alloc() = 0;
    
    virtual void  PRO_STDCALL Free(void* p) = 0;
    
    virtual ~IProMemoryPool() //// delete this!!!
    {
    }
};

/////////////////////////////////////////////////////////////////////////////
////

template<unsigned long objSize, unsigned long objPerChunk = 64, typename MUTEX = CProThreadMutex>
class CProMemoryPool : public IProMemoryPool, public CProCopyDisabled
{
public:
    
    CProMemoryPool()
    {
        m_fixedAlloc = ProCreateFixedAlloc_i(objSize, objPerChunk);
    }
    
    virtual ~CProMemoryPool()
    {
        ProDeleteFixedAlloc_i(m_fixedAlloc);
        m_fixedAlloc = NULL;
    }
    
    virtual void* PRO_STDCALL Alloc()
    {
        m_lock.Lock();
        void* const p = ProFixedAllocate_i(m_fixedAlloc);
        m_lock.Unlock();
        
        return (p);
    }
    
    virtual void PRO_STDCALL Free(void* p)
    {
        m_lock.Lock();
        ProFixedDeallocate_i(m_fixedAlloc, p);
        m_lock.Unlock();
    }
    
private:
    
    CFixedAlloc* m_fixedAlloc;
    MUTEX        m_lock;
};

/////////////////////////////////////////////////////////////////////////////
////

#if defined(_MSC_VER)
_STD_BEGIN

template<typename _Ty>
class allocator_sgi : public allocator<_Ty>
{
public:
    
    pointer allocate(size_type _N, const void*)
    {
        pointer const p = (pointer)ProAllocate_i(sizeof(_Ty) * _N);
        
        return (p);
    }
    
    char _FARQ* _Charalloc(size_type _N)
    {
        char* const p = (char*)ProAllocate_i(sizeof(char) * _N);
        
        return (p);
    }
    
    void deallocate(void _FARQ* _P, size_type _N)
    {
        ProDeallocate_i(_P, _N);
    }
};

_STD_END
#endif //// _MSC_VER

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_MEMORY_POOL_H____
