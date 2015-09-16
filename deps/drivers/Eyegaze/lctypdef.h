/*****************************************************************************

File Name:       LCTYPDEF.H
                 Commonly used data type abbreviations

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

*****************************************************************************/
#ifndef LCTYPDEF_H_INCLUDED
#define LCTYPDEF_H_INCLUDED
/****************************************************************************/

#include <windows.h>
#include <tchar.h>

/****************************************************************************/
/* Variable Type Definitions:                                               */

#define  VOID    void
#define  CHAR    TCHAR
#define  INT     int
#define  BOOL    int
#define  LONG    long
#define  FLOAT   float
#define  DOUBLE  double
#define  BYTE    unsigned char
#define  UCHAR   _TUCHAR
#define  UINT    unsigned int
#define  USHORT  unsigned short  // defined in windef.h  DixonC
#define  ULONG   unsigned long   // defined in windef.h  DixonC
#define  STRUCT  struct

#define  SVOID   static void
#define  SCHAR   static TCHAR
#define  SINT    static int
#define  SBOOL   static int
#define  SFLOAT  static float
#define  SBYTE   static unsigned char
#define  SUCHAR  static _TUCHAR
#define  SUINT   static unsigned int
#define  SSTRUCT static struct

/****************************************************************************/
#endif   // LCTYPDEF_H_INCLUDED
/****************************************************************************/
