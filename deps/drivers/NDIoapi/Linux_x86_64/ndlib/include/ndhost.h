/***************************************************************
Name:            NDHOST.H

Description:
      Amalgamated version of hdhost file.  PLATFORM is used to trigger.
      Partially completed, combining ndsystem into ndhost.

****************************************************************/
#ifndef INC_NDHOST_H
#define INC_NDHOST_H

#ifdef CC_TRACE
#pragma message ("include "__FILE__)
#endif


#ifdef __cplusplus
#define CPLUSPLUS( x ) x
#else
#define CPLUSPLUS( x )
#endif
#ifdef __cplusplus
#define CPLUSPLUS_START extern "C" {
#define CPLUSPLUS_END }
#else
#define CPLUSPLUS_START
#define CPLUSPLUS_END
#endif

#undef HUGE   // old code keeps HUGE for SUN.  linux defines HUGE, overridden.
#undef FAR
#undef NEAR
#undef near
#undef far
#undef huge


// these defines are included for legacy only.  They should not be used.
#define IBMPC           0
#define APOLLO          0
#define AMIGA           0
#define MACINTOSH       0
#define BSD386          0
#define SGI             0
#define SUN             0
#define WIN16           0
#define HOST_WIN32      0
#define VC              0
#define LINUX           0


//---------------------------------------------------------------
// linux
//---------------------------------------------------------------
#ifdef PLATFORM_LINUX
#define HUGE
#define FAR
#define NDI_DECL1
#define NDI_DECL2

#undef LINUX
#define LINUX			1
#define NDI_LITTLE_ENDIAN
#define IEEE_FLOATS
#define BITFIELD_LTOM
#define DATA_ALIGNMENT 4  /* structures etc. align on mod 'n' addresses */

#define HOSTDEFINED


//---------------------------------------------------------------
// x86
//---------------------------------------------------------------
#elif defined (PLATFORM_X86WIN32) || defined (PLATFORM_X86CONSOLE) || defined (PLATFORM_X86)

#undef HOST_WIN32
#define HOST_WIN32      1

#define NDI_LITTLE_ENDIAN
#define IEEE_FLOATS
#define BITFIELD_LTOM
#define BYTE_DEFINED

#define DATA_ALIGNMENT 8  /* structures etc. align on mod 'n' addresses (/Zp affects) */

// #define HUGE
#define FAR
#define NEAR
#define huge
#define far
#define near

#define DIR_SEPARATOR   '\\'

//--borland------------------------------------
#ifdef __BORLANDC__
#include <borfix.h>
#if defined( _NO_STDCALL )
#define NDI_DECL2
#else
#define NDI_DECL2       __stdcall
#endif  /* _NO_STDCALL */

//--microsoft----------------------------------
#elif _MSC_VER
/* DECL1_EXPORT is added to CFLAGS_PRJ in the prjenv of any project wishing to use DLL exports */
#ifdef DECL1_EXPORT
#define NDI_DECL1       __declspec( dllexport )
#else
#define NDI_DECL1
#endif

#if defined( _NO_STDCALL )
#define NDI_DECL2
#else
#define NDI_DECL2       __stdcall
#endif  /* _NO_STDCALL */
#define inp             _inp
#define outp            _outp

//--other--------------------------------------
#else
#error unknown X86 compiler
#endif

#define HOSTDEFINED


#else

#ifdef NDI_INTERNAL_BUILD
#include <ndhost_int.h>
#else
#error NDI Error - Host not defined. Options: PLATFORM_X86, PLATFORM_LINUX
#endif

#endif  /*PLATFORM_type*/


// set the dir separator generically, if it was not specifically set above.
#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR '/'
#endif




//---------------------------------------------------------------

#endif  /*INC_NDHOST_H*/


