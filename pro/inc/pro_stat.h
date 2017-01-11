#if !defined(____PRO_STAT_H___)
#define ____PRO_STAT_H___

#include "pro_a.h"
#include "pro_copy_disabled.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProStatBitRate : public CProCopyDisabled
{
public:
    
    CProStatBitRate();
    
    virtual ~CProStatBitRate();
    
    void SetTimeSpan(unsigned long timeSpanInSeconds = 5);
    
    void PushData(unsigned long dataBits);
    
    float GetBitRate() const;
    
    void Update();
    
    void Reset();
    
private:
    
    PRO_INT64 m_timeSpan;
    PRO_INT64 m_startTick;
    float     m_bits;
    float     m_bitRate;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProStatLossRate : public CProCopyDisabled
{
public:
    
    CProStatLossRate();
    
    virtual ~CProStatLossRate();
    
    void SetTimeSpan(unsigned long timeSpanInSeconds = 5);
    
    void SetMaxBrokenDuration(unsigned char maxBrokenDurationInSeconds = 10);
    
    void PushData(PRO_UINT16 dataSeq);
    
    float GetLossRate() const;
    
    float GetLossCount() const;
    
    void Reset();
    
private:
    
    void Update();
    
private:
    
    PRO_INT64     m_timeSpan;
    unsigned long m_maxBrokenDuration;
    PRO_INT64     m_startTick;
    PRO_INT64     m_lastValidTick;
    PRO_UINT16    m_nextSeq;
    float         m_count;
    float         m_lossCount;
    float         m_lossCountAll;
    float         m_lossRate;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProStatAvgValue : public CProCopyDisabled
{
public:
    
    CProStatAvgValue();
    
    virtual ~CProStatAvgValue();
    
    void SetTimeSpan(unsigned long timeSpanInSeconds = 5);
    
    void PushData(float dataValue);
    
    float GetAvgValue() const;
    
    void Reset();
    
private:
    
    void Update();
    
private:
    
    PRO_INT64 m_timeSpan;
    PRO_INT64 m_startTick;
    float     m_values;
    float     m_count;
    float     m_avgValue;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_STAT_H___
