#if !defined(____PRO_Z_H____)
#define ____PRO_Z_H____

#include "pro_a.h"
#include "pro_extern_c.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <cctype>
#if !defined(_WIN32_WCE)
#include <cerrno>
#endif
#include <cmath>
#if !defined(_WIN32_WCE)
#include <csignal>
#endif
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>

#if defined(WIN32) || defined(_WIN32_WCE)
#if !defined(_WIN32_WCE)
#include <conio.h>
#endif
#include <tchar.h>
#endif

#if defined(__TI_COMPILER_VERSION__)
using namespace std; //// !!!
#endif

PRO_EXTERN_C1

/////////////////////////////////////////////////////////////////////////////
////

#if defined(WIN32) || defined(_WIN32_WCE)

#define strlwr   _strlwr
#define strupr   _strupr
#define stricmp  _stricmp
#define strnicmp _strnicmp
#define snprintf _snprintf
#if defined(_WIN32_WCE)
#define time     _time64
#endif

#else //// WIN32, _WIN32_WCE

#if !defined(__TI_COMPILER_VERSION__)
#define stricmp  strcasecmp
#define strnicmp strncasecmp
#endif

#endif //// WIN32, _WIN32_WCE

/////////////////////////////////////////////////////////////////////////////
////

#if !defined(WIN32) && !defined(_WIN32_WCE)

static
char*
PRO_STDCALL
strlwr(char* str)
{
    if (str != NULL)
    {
        for (int i = 0; str[i] != '\0'; ++i)
        {
            str[i] = (char)tolower(str[i]);
        }
    }
    
    return (str);
}

static
char*
PRO_STDCALL
strupr(char* str)
{
    if (str != NULL)
    {
        for (int i = 0; str[i] != '\0'; ++i)
        {
            str[i] = (char)toupper(str[i]);
        }
    }
    
    return (str);
}

#if defined(__TI_COMPILER_VERSION__)

static
int
PRO_STDCALL
strnicmp(const char* str1, const char* str2, int n)
{
    if (str1 == str2 || n <= 0)
    {
        return (0);
    }
    
    if (str1 == NULL)
    {
        if (str2[0] == '\0')
        {
            return (0);
        }
        else
        {
            return (-1);
        }
    }
    
    if (str2 == NULL)
    {
        if (str1[0] == '\0')
        {
            return (0);
        }
        else
        {
            return (1);
        }
    }
    
    char c1 = '\0';
    char c2 = '\0';
    
    for (int i = 0; i < n; ++i)
    {
        c1 = (char)tolower(str1[i]);
        c2 = (char)tolower(str2[i]);
        
        if (c1 != c2 || c1 == '\0' || c2 == '\0')
        {
            break;
        }
    }
    
    if (c1 < c2)
    {
        return (-1);
    }
    else if (c1 == c2)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}

static
int
PRO_STDCALL
stricmp(const char* str1, const char* str2)
{
    const int ret = strnicmp(str1, str2, 0x7FFFFFFF);
    
    return (ret);
}

#endif //// __TI_COMPILER_VERSION__

#endif //// WIN32, _WIN32_WCE

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____PRO_Z_H____
