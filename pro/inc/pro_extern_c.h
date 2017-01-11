#if !defined(____PRO_EXTERN_C_H____)
#define ____PRO_EXTERN_C_H____

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)

#define PRO_EXTERN_C1 extern "C" {
#define PRO_EXTERN_C2 }

#else //// __cplusplus

#define PRO_EXTERN_C1
#define PRO_EXTERN_C2

#endif //// __cplusplus

/////////////////////////////////////////////////////////////////////////////
////

#endif //// ____PRO_EXTERN_C_H____
