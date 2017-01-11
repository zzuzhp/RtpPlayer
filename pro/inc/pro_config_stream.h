#if !defined(____PRO_CONFIG_STREAM_H____)
#define ____PRO_CONFIG_STREAM_H____

#include "pro_a.h"
#include "pro_config_file.h"
#include "pro_copy_disabled.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <map>
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
////

class CProConfigStream : public CProCopyDisabled
{
public:
    
    static bool StringToConfigs(
        std::string                   theString,
        std::vector<PRO_CONFIG_ITEM>& theConfigs
        );
    
    static void ConfigsToString(
        std::vector<PRO_CONFIG_ITEM> theConfigs,
        std::string&                 theString
        );
    
    CProConfigStream();
    
    virtual ~CProConfigStream();
    
    void Add(
        std::string configName,
        std::string configValue
        );
    
    void Add(PRO_CONFIG_ITEM config);
    
    void Add(std::vector<PRO_CONFIG_ITEM> configs);
    
    void AddInt(
        std::string configName,
        int         configValue
        );
    
    void AddUint(
        std::string  configName,
        unsigned int configValue
        );
    
    void AddInt64(
        std::string configName,
        PRO_INT64   configValue
        );
    
    void AddUint64(
        std::string configName,
        PRO_UINT64  configValue
        );
    
    void Remove(std::string configName);
    
    void Clear();
    
    void Get(
        std::string  configName,
        std::string& configValue
        ) const;
    
    void Get(
        std::string                   configName,
        std::vector<PRO_CONFIG_ITEM>& configs
        ) const;
    
    void Get(std::vector<PRO_CONFIG_ITEM>& configs) const;
    
    void GetInt(
        std::string configName,
        int&        configValue
        ) const;
    
    void GetUint(
        std::string   configName,
        unsigned int& configValue
        ) const;
    
    void GetInt64(
        std::string configName,
        PRO_INT64&  configValue
        ) const;
    
    void GetUint64(
        std::string configName,
        PRO_UINT64& configValue
        ) const;
    
    void ToString(std::string& theString) const;
    
private:
    
    std::map<std::string, std::vector<PRO_CONFIG_ITEM> > m_name2Configs;
    std::map<std::string, unsigned long>                 m_name2Index;
    unsigned long                                        m_nextIndex;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_CONFIG_STREAM_H____
