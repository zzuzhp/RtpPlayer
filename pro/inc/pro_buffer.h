#if !defined(____PRO_BUFFER_H____)
#define ____PRO_BUFFER_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"
#include "pro_z.h"

/////////////////////////////////////////////////////////////////////////////
////

class CProBuffer : public CProCopyDisabled
{
public:
    
    CProBuffer()
    {
        m_data     = NULL;
        m_dataSize = 0;
        m_preSize  = 0;
        m_postSize = 0;
    }
    
    virtual ~CProBuffer()
    {
        if (m_data != NULL)
        {
            char* const p = m_data - m_preSize;
            delete[] p;
            
            m_data     = NULL;
            m_dataSize = 0;
            m_preSize  = 0;
            m_postSize = 0;
        }
    }
    
    bool Resize(
        unsigned long dataSize,
        unsigned long preSize  = 0,
        unsigned long postSize = 0
        )
    {
        if (dataSize == 0)
        {
            return (false);
        }
        
        if (m_data != NULL)
        {
            char* const p = m_data - m_preSize;
            delete[] p;
            
            m_data     = NULL;
            m_dataSize = 0;
            m_preSize  = 0;
            m_postSize = 0;
        }
        
        char* const p = new char[dataSize + preSize + postSize];
        
        if (preSize > 0)
        {
            memset(p, 0, preSize);
        }
        
        if (postSize > 0)
        {
            memset(p + preSize + dataSize, 0, postSize);
        }
        
        m_data     = p + preSize;
        m_dataSize = dataSize;
        m_preSize  = preSize;
        m_postSize = postSize;
        
        return (true);
    }
    
    void* Data()
    {
        return (m_data);
    }
    
    const void* Data() const
    {
        return (m_data);
    }
    
    unsigned long DataSize() const
    {
        return (m_dataSize);
    }
    
    unsigned long PreSize() const
    {
        return (m_preSize);
    }
    
    unsigned long PostSize() const
    {
        return (m_postSize);
    }
    
private:
    
    char*         m_data;
    unsigned long m_dataSize;
    unsigned long m_preSize;
    unsigned long m_postSize;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_BUFFER_H____
