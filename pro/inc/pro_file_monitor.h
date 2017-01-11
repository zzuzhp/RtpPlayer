#if !defined(____PRO_FILE_MONITOR_H____)
#define ____PRO_FILE_MONITOR_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_thread_mutex.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <string>

/////////////////////////////////////////////////////////////////////////////
////

class CProFileMonitor : public CProCopyDisabled
{
public:
    
    CProFileMonitor();
    
    virtual ~CProFileMonitor();
    
    void Init(const char* fileName);
    
    void UpdateFileExist();
    
    bool QueryFileExist() const;
    
private:
    
    std::string             m_fileName;
    PRO_INT64               m_updateTick;
    bool                    m_exist;
    mutable CProThreadMutex m_lock;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_FILE_MONITOR_H____
