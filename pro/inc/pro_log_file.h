#if !defined(____PRO_LOG_FILE_H____)
#define ____PRO_LOG_FILE_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_thread_mutex.h"
#include "pro_z.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProLogFile : public CProCopyDisabled
{
public:
    
    CProLogFile();
    
    virtual ~CProLogFile();
    
    void Init(const char* fileName);
    
    void SetGreenLevel(long level);
    
    long GetGreenLevel() const;
    
    void Log(const char* text, long level = 0, bool showTime = true);
    
    long GetPos() const;
    
    void Rewind();
    
private:
    
    FILE*                   m_file;
    long                    m_greenLevel;
    mutable CProThreadMutex m_lock;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_LOG_FILE_H____
