#if !defined(____PRO_A_H____)
#define ____PRO_A_H____

/////////////////////////////////////////////////////////////////////////////
////

#define PRO_INT16_VC    short
#define PRO_INT32_VC    int
#define PRO_INT64_VC    __int64
#define PRO_UINT16_VC   unsigned short
#define PRO_UINT32_VC   unsigned int
#define PRO_UINT64_VC   unsigned __int64
#define PRO_CDECL_VC    __cdecl
#define PRO_STDCALL_VC  __stdcall
#define PRO_EXPORT_VC   __declspec(dllexport)
#define PRO_IMPORT_VC   __declspec(dllimport)

#define PRO_INT16_GCC   short
#define PRO_INT32_GCC   int
#define PRO_INT64_GCC   long long
#define PRO_UINT16_GCC  unsigned short
#define PRO_UINT32_GCC  unsigned int
#define PRO_UINT64_GCC  unsigned long long
#define PRO_CDECL_GCC   __attribute__((__cdecl__))
#define PRO_STDCALL_GCC __attribute__((__stdcall__))
#define PRO_EXPORT_GCC  __attribute__((visibility("default")))
#define PRO_IMPORT_GCC

#if defined(_MSC_VER)

#if !defined(PRO_INT16)
#define PRO_INT16   PRO_INT16_VC
#endif
#if !defined(PRO_INT32)
#define PRO_INT32   PRO_INT32_VC
#endif
#if !defined(PRO_INT64)
#define PRO_INT64   PRO_INT64_VC
#endif
#if !defined(PRO_UINT16)
#define PRO_UINT16  PRO_UINT16_VC
#endif
#if !defined(PRO_UINT32)
#define PRO_UINT32  PRO_UINT32_VC
#endif
#if !defined(PRO_UINT64)
#define PRO_UINT64  PRO_UINT64_VC
#endif
#if !defined(PRO_CDECL)
#define PRO_CDECL   PRO_CDECL_VC
#endif
#if !defined(PRO_STDCALL)
#define PRO_STDCALL PRO_STDCALL_VC
#endif
#if !defined(PRO_EXPORT)
#define PRO_EXPORT  PRO_EXPORT_VC
#endif
#if !defined(PRO_IMPORT)
#define PRO_IMPORT  PRO_IMPORT_VC
#endif

#else //// _MSC_VER

#if !defined(PRO_INT16)
#define PRO_INT16   PRO_INT16_GCC
#endif
#if !defined(PRO_INT32)
#define PRO_INT32   PRO_INT32_GCC
#endif
#if !defined(PRO_INT64)
#define PRO_INT64   PRO_INT64_GCC
#endif
#if !defined(PRO_UINT16)
#define PRO_UINT16  PRO_UINT16_GCC
#endif
#if !defined(PRO_UINT32)
#define PRO_UINT32  PRO_UINT32_GCC
#endif
#if !defined(PRO_UINT64)
#define PRO_UINT64  PRO_UINT64_GCC
#endif
#if !defined(PRO_CDECL)
#define PRO_CDECL
#endif
#if !defined(PRO_STDCALL)
#define PRO_STDCALL
#endif
#if !defined(PRO_EXPORT)
#define PRO_EXPORT  PRO_EXPORT_GCC
#endif
#if !defined(PRO_IMPORT)
#define PRO_IMPORT  PRO_IMPORT_GCC
#endif

#endif //// _MSC_VER

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_A_H____
