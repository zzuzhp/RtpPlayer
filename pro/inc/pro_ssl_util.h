#if !defined(____PRO_SSL_UTIL_H____)
#define ____PRO_SSL_UTIL_H____

#include "pro_a.h"
#include "pro_extern_c.h"

PRO_EXTERN_C1

/////////////////////////////////////////////////////////////////////////////
////

void
PRO_STDCALL
ProMd5(const void*   buf,
       unsigned long size,
       char          hashValue[16]);

void
PRO_STDCALL
ProSha1(const void*   buf,
        unsigned long size,
        char          hashValue[20]);

void
PRO_STDCALL
ProBlowfishEncrypt(const void*   key,
                   unsigned long keyLength,
                   void*         buffer,
                   unsigned long bufferLength,
                   bool          xorOnly);

void
PRO_STDCALL
ProBlowfishDecrypt(const void*   key,
                   unsigned long keyLength,
                   void*         buffer,
                   unsigned long bufferLength,
                   bool          xorOnly);

void
PRO_STDCALL
ProCalcPasswordHash20(const char* password,
                      PRO_UINT32  token,
                      char        passwordHash[20]);

void
PRO_STDCALL
ProCalcPasswordHash40(const char* password,
                      PRO_UINT32  token,
                      char        passwordHash[40]);

bool
PRO_STDCALL
ProCheckPasswordHash20(const char* password,
                       PRO_UINT32  token,
                       const char  passwordHash[20]);

bool
PRO_STDCALL
ProCheckPasswordHash40(const char* password,
                       PRO_UINT32  token,
                       const char  passwordHash[40]);

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____PRO_SSL_UTIL_H____
