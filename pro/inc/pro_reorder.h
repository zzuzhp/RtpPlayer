#if !defined(____PRO_REORDER_H____)
#define ____PRO_REORDER_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <map>

/////////////////////////////////////////////////////////////////////////////
////

class IRtpPacket;

/////////////////////////////////////////////////////////////////////////////
////

class CProReorder : public CProCopyDisabled
{
public:
    
    CProReorder();
    
    virtual ~CProReorder();
    
    void SetMaxPacketCount(unsigned char maxPacketCount = 5);
    
    void SetMaxWaitingDuration(unsigned char maxWaitingDurationInSeconds = 1);
    
    void SetMaxBrokenDuration(unsigned char maxBrokenDurationInSeconds = 10);
    
    void PushBack(IRtpPacket* packet);
    
    IRtpPacket* PopFront();
    
    void Reset();
    
private:
    
    unsigned long                    m_maxPacketCount;
    unsigned long                    m_maxWaitingDuration;
    unsigned long                    m_maxBrokenDuration;
    PRO_INT64                        m_minSeq64;
    PRO_INT64                        m_lastValidTick;
    std::map<PRO_INT64, IRtpPacket*> m_seq64ToPacket;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_REORDER_H____
