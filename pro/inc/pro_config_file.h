#if !defined(____PRO_CONFIG_FILE_H____)
#define ____PRO_CONFIG_FILE_H____

#include "pro_a.h"
#include "pro_copy_disabled.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
////

struct PRO_CONFIG_ITEM
{
    PRO_CONFIG_ITEM()
    {
        configName  = "";
        configValue = "";
    }
    
    std::string configName;
    std::string configValue;
};

/////////////////////////////////////////////////////////////////////////////
////

class CProConfigFile : public CProCopyDisabled
{
public:
    
    CProConfigFile();
    
    virtual ~CProConfigFile();
    
    void Init(const char* fileName);
    
    bool Read(std::vector<PRO_CONFIG_ITEM>& configs) const;
    
    bool Write(std::vector<PRO_CONFIG_ITEM> configs);
    
private:
    
    std::string m_fileName;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_CONFIG_FILE_H____
