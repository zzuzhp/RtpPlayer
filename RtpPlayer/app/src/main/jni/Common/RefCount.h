#ifndef ___REFCOUNT_H___
#define ___REFCOUNT_H___

#include "pro_ref_count.h"
#include "Common/common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class RefCount : public CProRefCount
{
public:

    virtual ~RefCount(){}

    unsigned long AddRef()
    {
        unsigned long ulref = CProRefCount::AddRef();
        return ulref;
    }

    unsigned long Release()
    {
        unsigned long ulref = CProRefCount::Release();
        return ulref;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___REFCOUNT_H___
