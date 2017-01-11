#if !defined(____PRO_SHAPER_H____)
#define ____PRO_SHAPER_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProShaper : public CProCopyDisabled
{
public:
    
    CProShaper();
    
    virtual ~CProShaper();
    
    void SetMaxBitRate(float maxBitRate = 384000);
    
    float CalcGreenBits();
    
    void FlushGreenBits(float dataBits);
    
    void Reset();
    
private:
    
    float     m_maxBitRate;
    float     m_greenBits;
    PRO_INT64 m_startTick;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_SHAPER_H____
