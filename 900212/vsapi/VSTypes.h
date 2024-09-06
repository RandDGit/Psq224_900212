/*
 * Type:        Header File
 * 
 * Id:          VSTypes.h
 *
 * Function:    Contains visualSTATE predefined types.
 *
 * Portability: ANSI-C Compiler.
 *
 * Copyright (c) 1999 IAR Systems. All rights reserved.
 */

/*
 * Version   ! Date   ! Status ! Changes
 *----------------------------------------------------------------------------
 * 4.0.1.0   ! 101199 ! Closed ! -
 */

#ifndef __VSTYPES_H
#define __VSTYPES_H



#include <limits.h>



#ifdef SE_EMBED
#define VS_FILE_TYPE void
#else
#define VS_FILE_TYPE FILE
#endif



#ifndef SEM_VS_TYPES_AS_MACROS
#error Do not include this file directly, include SEMTypes.h or SEMBDef.h/SEMEDef.h.
#else
#if (SEM_VS_TYPES_AS_MACROS == 1)


#define VS_BOOL            unsigned char      
#define VS_UCHAR           unsigned char      
#define VS_SCHAR           signed char        
#define VS_UINT            unsigned int       
#define VS_INT             signed int         
#define VS_FLOAT           float              
#define VS_DOUBLE          double             
#define VS_VOID            void               
#define VS_VOIDPTR         void*              

#if (UCHAR_MAX >= 0x0ff)
#define VS_UINT8           unsigned char      
#define VS_INT8            signed char        
#elif (USHRT_MAX >= 0x0ff)
#define VS_UINT8           unsigned short     
#define VS_INT8            signed short       
#elif (UINT_MAX >= 0x0ff)
#define VS_UINT8           unsigned int       
#define VS_INT8            signed int         
#elif (ULONG_MAX >= 0x0ff)
#define VS_UINT8           unsigned long      
#define VS_INT8            signed long        
#else
#define VS_UINT8           (unsupported data type)
#define VS_INT8            (unsupported data type)
#endif

#define VS_UINT16_VAARG    VS_INT
#define VS_INT16_VAARG     VS_INT
#if (UCHAR_MAX >= 0x0ffff)
#define VS_UINT16          unsigned char      
#define VS_INT16           signed char        
#elif (USHRT_MAX >= 0x0ffff)
#define VS_UINT16          unsigned short     
#define VS_INT16           signed short       
#elif (UINT_MAX >= 0x0ffff)
#define VS_UINT16          unsigned int       
#define VS_INT16           signed int         
#elif (ULONG_MAX >= 0x0ffff)
#define VS_UINT16          unsigned long      
#define VS_INT16           signed long        
#undef VS_UINT16_VAARG
#undef VS_INT16_VAARG
#define VS_UINT16_VAARG    VS_INT16
#define VS_INT16_VAARG     VS_INT16
#else
#define VS_UINT16          (unsupported data type)
#define VS_INT16           (unsupported data type)
#endif

#define VS_UINT32_VAARG    VS_INT
#define VS_INT32_VAARG     VS_INT
#if (UCHAR_MAX >= 0x0ffffffffUL)
#define VS_UINT32          unsigned char      
#define VS_INT32           signed char        
#elif (USHRT_MAX >= 0x0ffffffffUL)
#define VS_UINT32          unsigned short     
#define VS_INT32           signed short       
#elif (UINT_MAX >= 0x0ffffffffUL)
#define VS_UINT32          unsigned int       
#define VS_INT32           signed int         
#elif (ULONG_MAX >= 0x0ffffffffUL)
#define VS_UINT32          unsigned long      
#define VS_INT32           signed long        
#undef VS_UINT32_VAARG
#undef VS_INT32_VAARG
#define VS_UINT32_VAARG    VS_INT32
#define VS_INT32_VAARG     VS_INT32
#else
#define VS_UINT32          (unsupported data type)
#define VS_INT32           (unsupported data type)
#endif



#else



typedef unsigned char      VS_BOOL;
typedef unsigned char      VS_UCHAR;
typedef signed char        VS_SCHAR;
typedef unsigned int       VS_UINT;
typedef signed int         VS_INT;
typedef float              VS_FLOAT;
typedef double             VS_DOUBLE;
#define VS_VOID            void               
typedef void*              VS_VOIDPTR;

#if (UCHAR_MAX >= 0x0ff)
typedef unsigned char      VS_UINT8;
typedef signed char        VS_INT8;
#elif (USHRT_MAX >= 0x0ff)
typedef unsigned short     VS_UINT8;
typedef signed short       VS_INT8;
#elif (UINT_MAX >= 0x0ff)
typedef unsigned int       VS_UINT8;
typedef signed int         VS_INT8;
#elif (ULONG_MAX >= 0x0ff)
typedef unsigned long      VS_UINT8;
typedef signed long        VS_INT8;
#else
#define VS_UINT8           (unsupported data type)
#define VS_INT8            (unsupported data type)
#endif

#define VS_UINT16_VAARG    VS_INT
#define VS_INT16_VAARG     VS_INT
#if (UCHAR_MAX >= 0x0ffff)
typedef unsigned char      VS_UINT16;
typedef signed char        VS_INT16;
#elif (USHRT_MAX >= 0x0ffff)
typedef unsigned short     VS_UINT16;
typedef signed short       VS_INT16;
#elif (UINT_MAX >= 0x0ffff)
typedef unsigned int       VS_UINT16;
typedef signed int         VS_INT16;
#elif (ULONG_MAX >= 0x0ffff)
typedef unsigned long      VS_UINT16;
typedef signed long        VS_INT16;
#undef VS_UINT16_VAARG
#undef VS_INT16_VAARG
#define VS_UINT16_VAARG    VS_INT16
#define VS_INT16_VAARG     VS_INT16
#else
#define VS_UINT16          (unsupported data type)
#define VS_INT16           (unsupported data type)
#endif

#define VS_UINT32_VAARG    VS_INT
#define VS_INT32_VAARG     VS_INT
#if (UCHAR_MAX >= 0x0ffffffffUL)
typedef unsigned char      VS_UINT32;
typedef signed char        VS_INT32;
#elif (USHRT_MAX >= 0x0ffffffffUL)
typedef unsigned short     VS_UINT32;
typedef signed short       VS_INT32;
#elif (UINT_MAX >= 0x0ffffffffUL)
typedef unsigned int       VS_UINT32;
typedef signed int         VS_INT32;
#elif (ULONG_MAX >= 0x0ffffffffUL)
typedef unsigned long      VS_UINT32;
typedef signed long        VS_INT32;
#undef VS_UINT32_VAARG
#undef VS_INT32_VAARG
#define VS_UINT32_VAARG    VS_INT32
#define VS_INT32_VAARG     VS_INT32
#else
#define VS_UINT32          (unsupported data type)
#define VS_INT32           (unsupported data type)
#endif



#endif
#endif


#endif
