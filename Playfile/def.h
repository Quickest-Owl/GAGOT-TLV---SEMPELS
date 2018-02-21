/**********************************************************************\
 Library     :  Utils / Common
 Filename    :  def.h
 Purpose     :  Common definitions header
 Author      :  (c) RedSoft, Sergey Krasnitsky
 Created     :  1.3.1999
 Platform    :  Windows/Linux/Cygwin.
\**********************************************************************/

#ifndef _DEF_H
#define _DEF_H


/***********************************************************************\
                          Current PLATFORM
\***********************************************************************/

/* Supported PLATFORMs */
#define PLATFORM_WIN			0   // Windows
#define PLATFORM_LIN			1   // Linux/Cygwin


/* The PLATFORM define may be set in an external makefile. */
#ifndef PLATFORM
  #error PLATFORM is undefined (must be defined in project file)
#endif



/***********************************************************************\
                        Current PLATCOMPL
\***********************************************************************/

/* Supported PLATCOMPLs */
#define PLATCOMPL_MS			0   // Microsoft
#define PLATCOMPL_GNU			1   // GNU Compiler


/* The PLATFORM define may be set in an external makefile. */
#ifndef PLATCOMPL
  #error PLATCOMPL is undefined (must be defined in project file)
#endif


/* The ENDIANNESS define may be set in an external makefile: 0-Little Endian, 1-Big Endian */
#ifndef ENDIANNESS
  #define ENDIANNESS   0
#endif

/***********************************************************************\
                        Common includes
\***********************************************************************/
#include <stdint.h>     // C99 new types (uint16_t, uint32_t, ...)
#include <stdlib.h>     // malloc
#include <stdio.h>      // For MAC it is mandatory for sprintf
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h> 	// malloc (it's needed to be included because we want to redefine malloc/free)
#endif
#include <string.h>     // strcpy, strlen, strcat & memset
#include <stdarg.h>     // va_list                        
#include <stdio.h>      // sprintf & vsprintf
#include <limits.h>		// INT_MIN, INT_MAX, UINT_MAX, ...
#include <ctype.h>		// isalpha, ...
#include <signal.h>		// signal handling
#include <math.h>       // mathematical funcs
#ifdef __cplusplus
#include <new>
#endif

/***********************************************************************\
                          Basic types and constants
\***********************************************************************/

typedef unsigned long long  uint64 ;
typedef long long           int64  ;
typedef unsigned            uint32 ;
typedef unsigned short      uint16 ;
typedef unsigned char       uint8  ;
typedef int                 int32  ;
typedef signed short        int16  ;
typedef signed char         int8   ;
typedef int                 boolint;

#if PLATFORM == PLATFORM_WIN
typedef uint64				TIMESTAMP;
#else
#include <sys/time.h>
typedef struct timeval		TIMESTAMP;
#endif

typedef const char cchar;


// CPU-word common type used to support 32- and 64-bit architectures
// Sometimes pvoid is not enough because it may be needed to cast to integer
typedef void* pvoid;
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__LP64__) || (__WORDSIZE == 64)
#define cpuword			uint64
#define pvoidformat		"ll"
#else
#define cpuword			uint32
#define pvoidformat		""
#endif


#ifndef __cplusplus
  #ifndef true
  #define true    1
  #endif

  #ifndef false
  #define false   0
  #endif
#endif /* __cplusplus */

// values for boolint type
#define bool_true    1
#define bool_false   0
#define bool_none   -1


/***********************************************************************\
                    OS-dependent file path and name
\***********************************************************************/

#if PLATFORM == PLATFORM_WIN
#define PLATFORM_PATH_SEPARATOR '\\'
#else
#define PLATFORM_PATH_SEPARATOR '/'
#endif



/***********************************************************************\
	Bytes-to-Word
\***********************************************************************/
#define Bytes2Word32(x3,x2,x1,x0)		(((x3)<<24 | (x2)<<16 | (x1)<<8 | (x0)))
#define Byte0(x32)						((uint8)x32)
#define Byte1(x32)						((uint8)(x32 >> 8))
#define Byte2(x32)						((uint8)(x32 >> 16))
#define Byte3(x32)						((uint8)(x32 >> 24))



/***********************************************************************\
        ntohl(), ntohs(), htonl(), htons(), IP r/w as a macro
         - ntohl_(x) is identical to x = ntohl(x),
         - ntohl__ is identical to ntohl(), and so on...
 Note: in Utils/DS libs we are keeping IP addresses always in 
	   network format. For this purpose IpAddr macro must be used.
	   No needing to call htonl__/htonl_ for IP address 32b words.
\***********************************************************************/

#if ENDIANNESS == 0
    /* Little endian */
    #define ntohll_(x)  x = Swap8(x)
    #define ntohl_(x)   x = Swap4(x)
    #define ntohs_(x)   x = Swap2(x)
    #define htonll_(x)  x = Swap8(x)
    #define htonl_(x)   x = Swap4(x)
    #define htons_(x)   x = Swap2(x)

    #define ntohll__(x)		Swap8(x)
    #define ntohl__(x)		Swap4(x)
    #define ntohs__(x)		Swap2(x)
    #define htonll__(x)		Swap8(x)
    #define htonl__(x)		Swap4(x)
    #define htons__(x)		Swap2(x)

    #define IpAddr(x0,x1,x2,x3)		Bytes2Word32(x3,x2,x1,x0)

    #define IpByte0(x32)			Byte0(x32)
    #define IpByte1(x32)			Byte1(x32)
    #define IpByte2(x32)			Byte2(x32)
    #define IpByte3(x32)			Byte3(x32)
#else
    /* Big endian */
    #define ntohll_(x)
    #define ntohl_(x)
    #define ntohs_(x)
    #define htonll_(x)
    #define htonl_(x)
    #define htons_(x)

    #define ntohll__(x) x
    #define ntohl__(x)  x
    #define ntohs__(x)  x
    #define htonll__(x) x
    #define htonl__(x)  x
    #define htons__(x)  x

    #define IpAddr(x0,x1,x2,x3)		Bytes2Word32(x0,x1,x2,x3)

	#define IpByte3(x32)			Byte0(x32)
	#define IpByte2(x32)			Byte1(x32)
	#define IpByte1(x32)			Byte2(x32)
	#define IpByte0(x32)			Byte3(x32)
#endif



/***********************************************************************\
                             Packing attribute
\***********************************************************************/

#if PLATCOMPL == PLATCOMPL_MS
  #define packed_data
#elif PLATCOMPL == PLATCOMPL_GNU
  #define packed_data	__attribute__((packed))
#endif  /* PLATCOMPL */



/***********************************************************************\
                        Some short standard functions
\***********************************************************************/

#if PLATCOMPL == PLATCOMPL_MS
  #define vsnprintf _vsnprintf
  #define snprintf  _snprintf

  #ifdef __cplusplus
    /* Support abs function with unsigned argument */
    inline int abs(unsigned x) { return abs(int(x)); }
  #endif
#endif

#ifdef __APPLE__
  #ifdef __cplusplus
	inline int64 abs(int64 x) { return (x < 0) ? -x : x; }
  #endif
#endif

#if PLATCOMPL == PLATCOMPL_GNU && PLATFORM != PLATFORM_WIN
  #ifdef __cplusplus
    /* Support absent itoa function */
    inline char* itoa  (int val, char *str, int radix)			{ sprintf(str,"%d",val); return str; }
    inline void  ultoa (unsigned val, char *str, int radix)		{ sprintf(str,"%u",val); }
  #endif
#endif
#ifdef __cplusplus
    inline void lltoa (uint64 val, char *str, int radix)		{ sprintf(str,"%llu",val); }	// for Windows may be used _ui64toa
#endif



/***********************************************************************\
                 UNALIGNED ACCESS TO 32- & 16-BIT VARIABLES
 Note:
  Because of the __packed specifier in the ARM compiler has no affect 
  when doing a simple cast, a local intermediate variable is required. 
\***********************************************************************/
#if 0
inline uint32 UINT32_GET(void* ptr)             { uint32 __packed__ *p = (uint32*) ptr; return *p; }
inline uint16 UINT16_GET(void* ptr)             { uint16 __packed__ *p = (uint16*) ptr; return *p; }
inline int32  INT32_GET (void* ptr)             { int32  __packed__ *p = (int32*)  ptr; return *p; }
inline int16  INT16_GET (void* ptr)             { int16  __packed__ *p = (int16*)  ptr; return *p; }

inline void   UINT32_SET(void* ptr, uint32 x)   { uint32 __packed__ *p = (uint32*) ptr;  *p = x;   }
inline void   UINT16_SET(void* ptr, uint16 x)   { uint16 __packed__ *p = (uint16*) ptr;  *p = x;   }
inline void   INT32_SET (void* ptr, int32 x)    { int32  __packed__ *p = (int32*)  ptr;  *p = x;   }
inline void   INT16_SET (void* ptr, int16 x)    { int16  __packed__ *p = (int16*)  ptr;  *p = x;   }
#endif



/***********************************************************************\
                          Other common macro
\***********************************************************************/

#ifndef MAX
#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)        (((a) < (b)) ? (a) : (b))
#endif


#define ABS(x)          (((x)>=0) ? (x) : -(x))


#ifndef OFFSETOF
#define OFFSETOF(s,m)   (int)&(((s *)0)->m)
#endif

#ifndef TABLESIZE
#define TABLESIZE(table) (sizeof(table)/sizeof((table)[0]))
#endif

#define WORDALIGN(x)    ( (x + sizeof(uint32) - 1) / sizeof(uint32) * sizeof(uint32) )

#define ByteX(i,pval)   (((uint8*) pval)[i])
#define WordX(i,pval)   (((uint16*)pval)[i])

#define Swap8(x64)		( ((x64 & 0x00000000000000FFULL) << 56) | ((x64 & 0x000000000000FF00ULL) << 40) | ((x64 & 0x0000000000FF0000ULL) << 24) | ((x64 & 0x00000000FF000000ULL) <<  8) | ((x64 & 0x000000FF00000000ULL) >>  8) | ((x64 & 0x0000FF0000000000ULL) >> 24) | ((x64 & 0x00FF000000000000ULL) >> 40) | ((x64 & 0xFF00000000000000ULL) >> 56) )
#define Swap4(x32)      ( ((uint8)(x32) << 24) | ((x32 & 0x0000FF00) << 8) | ((x32 & 0x00FF0000) >> 8) | ((uint8)((x32) >> 24)) )
#define Swap2(x16)      ( ((uint8)(x16) << 8)  | ((uint8)((x16) >> 8)) )

#define BITMASK(nbytes) ((1<<(nbytes*8)-1)

#define ISODD(x)        ((uint32)(x) << 31)
#define ISEVEN(x)       (!(ISODD(x))

/*
 ******************************************************************
 Cyclic adding, subtraction
 Note: a caller must check that x1<base & x2<=base
 ******************************************************************
*/
#define CYCLIC_ADD(res,x1,x2,base)  { res = (x1+(x2)); if (res>=base) res-=base; }
#define CYCLIC_SUB(res,x1,x2,base)  { res = (x1-(x2)); if (res<0)	  res+=base; }

/*
 ******************************************************************
 Cyclic increment and decrement x with respect to base
 ******************************************************************
*/
#define CYCLIC_INC(x,base)      ((x)<(base-1)) ? (x+1) : 0
#define CYCLIC_DEC(x,base)      (x)            ? (x-1) : (base-1)


/***********************************************************************\
        Values for the compilation flags DSCFG
\***********************************************************************/

#define DSCFG_DISABLED          0
#define DSCFG_SIMPLE            1
#define DSCFG_HALF           	2
#define DSCFG_ENABLED           3


/***********************************************************************\
         Options.h (DS & Application specific definitions)
\***********************************************************************/
#include "options.h"


/***********************************************************************\
    Sometimes in low-level basic modules the ASSERT must be used,
	but may be not defined because of some complex h-dependencies.
	There is basic crash-only producing LOW_ASSERT for this cases.
\***********************************************************************/
#if DSASSERT == 1
#define LOW_ASSERTCRASH		(*((volatile uint32*)0) = 0)
#define LOW_ASSERT(x)		{ if (!(x)) { LOW_ASSERTCRASH; } }
#else
#define LOW_ASSERT(x)
#endif


#endif /* _DEF_H */
