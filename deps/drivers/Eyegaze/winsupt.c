/****************************************************************************

File Name:		  WINSUPT.C
Program Name:	  Stub functions for Win32 support

Company: 		  LC Technologies, Inc.
					  10363 Democracy Lane
                 Fairfax, VA 22030
					  (703) 385-7133

Date Created:	  06/12/98

*****************************************************************************


*****************************************************************************/
#include <windows.h>
#include <winbase.h>
#include <windowsx.h>

#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

#include <memory.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <sys\timeb.h>

#define	  RXLEN			 4096 		 /* length of recieve buffer				*/
#define	  TXLEN			 2048 		 /* length of transmit buffer 			*/

/****************************************************************************/

#include <lctypdef.h>			/* Commonly used data type abbreviations		 */
#include <lctutil.h> 			/* LCT's general utility functions            */
#include <lctsupt.h>          // NOTE: NOT winsupt.h (that's a special version) */
#include <lctfont.h>
#include <ikeys.h>
#include <lctcolor.h>
#include <igconfig.h>
#include <egfileio.h>

/****************************************************************************/
/* Required Function, Variable and Constant Definitions:                    */

static int WriteGraphicsAreaWidth;
static int WriteGraphicsAreaHeight;
static HBITMAP  hbm; 		 /* Used by read/write graphics area functions   */
static HDC		 hdcMem; 	 /*  "                                           */

/* Array of font handles.                                                   */
static HFONT ahFontHandle[32];

void lct_SelectFont(HFONT hFontHandle);

static HFONT EzCreateFont(HDC hdc, _TCHAR * szFaceName, int iDeciPtHeight,
								  int iDeciPtWidth, int iAttributes, BOOL fLogRes);

void DumpTheScreen(int iHorzDim, int iVertDim);

/****************************************************************************/
static BOOL bLocalXORMode = FALSE;

_TCHAR *pchFontName;

HWND hwndEyegaze = NULL;
HDC  hdcEyegaze  = NULL;             /* Device context of offscreen bitmap  */
HDC  hdcWindow   = NULL;             /* Device context for screen window    */
_TCHAR szAppName[50];  // passed in from init call, used in CreateWindow calls.

long hFindHandle; 	// used in the findfirst/findnext functions

static BOOL bScreenUpdatesActive = TRUE;
RECT	 rectUpdate;

/*------------------------ EZFONT.H header file  ---------------------------*/
#define EZ_ATTR_BOLD 			 1
#define EZ_ATTR_ITALIC			 2
#define EZ_ATTR_UNDERLINE		 4
#define EZ_ATTR_STRIKEOUT		 8
#define EZ_ATTR_FIXEDPITCH 	16

/****************************************************************************/
/****************************************************************************/
/* FONT FUNCTIONS:																			 */
/****************************************************************************/
/****************************************************************************/
int lct_fontinit(void)
{
/* Use transparent backgrounds under font characters.                       */
	SetBkMode(hdcEyegaze, TRANSPARENT);
	return 0;
}
/****************************************************************************/
int lctFontLoad(int iFontNumber)

/* We currently use four fonts.	0 is an EGA font, 1 is a VGA font, 2 is    */
/* a 'roman' font (the large font we use for page titles, etc.) and 3 is a  */
/* 'SBO' font used for button labels, etc (medium size font).               */
{
	int	iDeciPtHeight;
	int	iFontAttributes;
	_stEgFonts stEgFonts;

	if ((iFontNumber < 0) || (iFontNumber > 9))
	{
		_tprintf(_TEXT("Invalid font number defined in font_load\n"));
		lct_sound(500,4);
		lct_sound(1000,4);
	}

/* If we have already loaded this font number, there's nothing to do.       */
   if (ahFontHandle[iFontNumber] != 0) return 0;

   vReadFontsSettings(&stEgFonts, LANG_DEFAULT);

	iDeciPtHeight	 = stEgFonts.astFonts[iFontNumber].iDecipointHeight;
	iFontAttributes = stEgFonts.astFonts[iFontNumber].stFlags;

	ahFontHandle[iFontNumber] =
		EzCreateFont(hdcEyegaze, stEgFonts.astFonts[iFontNumber].pszFaceName, iDeciPtHeight, 0,
						 iFontAttributes, TRUE);
	if (ahFontHandle[iFontNumber] == NULL)
	{
		_tprintf(_TEXT("Error loading font number %i\n"), iFontNumber);
		return 1;
	}
	return 0;
}
/****************************************************************************/
int lctFontLoadSpecific(int iFontNumber, _TCHAR *pchFontName,
								int iDeciPtHeight, int iFontAttributes)

/* This function allows a program to bypass the four LCT-selected fonts     */
/* and select any font on the system into a font numbered 0-9.              */
{
/* If we have already loaded this font number, unload it first.             */
	if (ahFontHandle[iFontNumber] != 0)
	{
		lct_font_unload(iFontNumber);
	}

	if ((iFontNumber < 0) || (iFontNumber > 9))
	{
		_tprintf(_TEXT("Invalid font number defined in font_load\n"));
		lct_sound(500,4);
		lct_sound(1000,4);
      return 1;
	}

	ahFontHandle[iFontNumber] =
		EzCreateFont(hdcEyegaze, pchFontName, iDeciPtHeight, 0,
						 iFontAttributes, TRUE);
	if (ahFontHandle[iFontNumber] == NULL)
	{
		_tprintf(_TEXT("Error loading font number %i\n"), iFontNumber);
		return 1;
	}
	return 0;
}
/****************************************************************************/
void lct_font_unload(int iFontNumber)
{
	int i;

/* If we pass in a -1, unload all the fonts.  Otherwise unload one font.    */
	if (iFontNumber == -1)
	{
		for (i = 0; i < 4; i++)
		{
			if (ahFontHandle[i] != 0) DeleteObject(ahFontHandle[i]);
		}

/* 	Zero out all the font handles.                                        */
		for (i = 0; i < 4; i++) ahFontHandle[i] = 0;
	}
/* Don't attempt to unload a font that we haven't loaded yet.               */
	else if (ahFontHandle[iFontNumber] != 0)
	{
		DeleteObject(ahFontHandle[iFontNumber]);
		ahFontHandle[iFontNumber] = 0;
	}
}
/****************************************************************************/
int lct_fontpixn(int iFontNumber, _TCHAR *pchText, int dir, int len)
{
	SIZE lpSize;

/* Select the font to use for this string.                                  */
	lct_SelectFont(ahFontHandle[iFontNumber]);

/* Many (most?) of our programs pass a length of zero.  This seems to have  */
/* been a bug in Essential Graphics that allowed this to work.              */
	if (len <= 0) len = _tcslen(pchText);

	GetTextExtentPoint32(hdcEyegaze, pchText, len, &lpSize);
	return lpSize.cx;
}
/****************************************************************************/
int lct_fontheight(int iFontNumber, _TCHAR *pchText)
{
	TEXTMETRIC tm;

/* Select the font to use for this string.                                  */
	lct_SelectFont(ahFontHandle[iFontNumber]);

	GetTextMetrics(hdcEyegaze, &tm);

	return tm.tmHeight;
}
/****************************************************************************/
void lct_FontInfo(int iFontNumber, void *p)
{
	struct lctFontInfo *pstX;
	static TEXTMETRIC tm;

	pstX = (struct lctFontInfo *)p;

/* Select the font to use for this string.                                  */
	lct_SelectFont(ahFontHandle[iFontNumber]);

	if (GetTextMetrics(hdcEyegaze, &tm) == 0)
	{
#if defined _DEBUG
		FILE *hf;
		_TCHAR achErrorString[100];
		_TCHAR achDate[20];
		_TCHAR achTime[20];

		swprintf_s(achErrorString, _countof(achErrorString), _TEXT("ZERO return from GetTextMetrics()"));
		_wfopen_s(&hf, _T("font.err"), _T("a"));
		if (hf != NULL)
		{
/*       Write the date and time to the log file.                           */
			lct_sysdate(achDate, 20, 7);
			_ftprintf(hf, _TEXT("%s  "), achDate);
			lct_systime(achTime, 20, 9);
			_ftprintf(hf, _TEXT("%s  "), achTime);

			_ftprintf(hf, _TEXT("%s\n"), achErrorString);
			fclose(hf);
		}
#endif
		// ugly hack: (avoid divide by zero)
		pstX->iWidth	  = 10;
		pstX->iHeight	  = 18;
		pstX->iDescender = 4;
	}
	else
	{
		pstX->iWidth	  = tm.tmMaxCharWidth;
		pstX->iHeight	  = tm.tmHeight;
		pstX->iDescender = tm.tmDescent;
	}

/* UGLY KLUDGE: Windows is returning widths of over 100 pixels on some      */
/* fonts.                                                                   */
	if (tm.tmMaxCharWidth > tm.tmHeight*2)
	{
		int iW;

		iW = lct_fontpixn(iFontNumber, _TEXT("W"),0,1);
		pstX->iWidth = (int)(iW * 1.1);
	}
}
/****************************************************************************/
int lct_fontch(int iFontNumber, _TUCHAR ch, int x, int y,
					 int iColor, int rot)
{
	TEXTMETRIC tm;

/* NOTE: this function is almost identical to lct_fontsprt().               */

/* Select the font to use for this string.                                  */
	lct_SelectFont(ahFontHandle[iFontNumber]);

	GetTextMetrics(hdcEyegaze, &tm);

	SetTextColor(hdcEyegaze, iColor);
	TextOut(hdcEyegaze, x, y-tm.tmAscent, (_TCHAR *)&ch, 1);
	lct_RedrawScreen(x, y+40, x+50, max(y-50,0));

	return 0;
}
/****************************************************************************/
int lct_fontsprt(int iFontNumber, _TCHAR *pchText, int x, int y,
                 int iDirection, int iColor, int iRotation)
{
	TEXTMETRIC tm;

/* Select the font to use for this string.                                  */
	lct_SelectFont(ahFontHandle[iFontNumber]);

	GetTextMetrics(hdcEyegaze, &tm);

	SetTextColor(hdcEyegaze, iColor);
	TextOut(hdcEyegaze, x, y-tm.tmAscent, pchText, _tcslen(pchText));
	lct_RedrawScreen(x, y-tm.tmAscent, x+lct_fontpixn(iFontNumber, pchText, iDirection, 0), y+tm.tmDescent);

/* Quiet the compiler */
	if (iDirection || iRotation);

	return 0;
}
/****************************************************************************/
void lct_SelectFont(HFONT hFontHandle)
{
/* Select the font to use for this string.                                  */

	LOGFONT lf;

	if (GetObject(hFontHandle, sizeof(LOGFONT), &lf) == 0);

	if (SelectObject(hdcEyegaze, hFontHandle) == NULL);

	return;
}
/****************************************************************************/
/****************************************************************************/
/* Graphics Primitives: 																	 */
/****************************************************************************/
/****************************************************************************/
void lctSetWindowHandle(HWND hwnd, HDC memdc, HDC hdc, _TCHAR *sz, int iAppNameStringLen)
{
	hwndEyegaze  = hwnd;
	hdcEyegaze	 = memdc;
	hdcWindow	 = hdc;
	wcscpy_s(szAppName, 50, sz);
}
/****************************************************************************/
void lctSetColor(int iColor)
{
	static HPEN hpen;

/* Delete the old pen if it exists.                                         */
	if (hpen != 0) DeleteObject(hpen);

	hpen = CreatePen(PS_SOLID, 1, iColor);
	SelectPen(hdcEyegaze, hpen);
}
/****************************************************************************/
void lctSetBrush(int iColor)
{
	static HBRUSH hbrush;

/* Delete the old brush if it exists.                                       */
	if (hbrush != 0) DeleteObject(hbrush);

	hbrush = CreateSolidBrush(iColor);
	SelectObject(hdcEyegaze, hbrush);
}
/****************************************************************************/
void lct_RedrawScreen(int x1, int y1, int x2, int y2)
{
	RECT stRect;
	BOOL bSuccess;
	int  iRetryCount;
	PAINTSTRUCT ps;

/* Invalidate the region that we need updated.                              */
	stRect.left   = min(x1,x2);
	stRect.right  = max(x1,x2)+1;
	stRect.top	  = min(y1,y2);
	stRect.bottom = max(y1,y2)+1;

	if (bScreenUpdatesActive == TRUE)
	{
iRetryCount = 0;
retry:
		BeginPaint(hwndEyegaze, &ps);
		bSuccess = BitBlt(hdcWindow,
					  stRect.left, stRect.top,
					  stRect.right  - stRect.left,
					  stRect.bottom - stRect.top,
					  hdcEyegaze,
					  stRect.left, stRect.top,
					  SRCCOPY);
// AWJ - 5/20/06 - 3Dat stereo calibration
      #ifdef STEREO_CALIBRATION
	   	BitBlt(hdcWindow,
					  stRect.left+1024, stRect.top,
					  stRect.right  - stRect.left,
					  stRect.bottom - stRect.top,
					  hdcEyegaze,
					  stRect.left, stRect.top,
					  SRCCOPY);
      #endif
		EndPaint(hwndEyegaze, &ps);

/*    If the call to BitBlt fails, record the error and retry the call      */
/*    up to four more times, yielding the CPU time slice each time.         */
		if (bSuccess == FALSE)
		{
			if (iRetryCount < 4)
			{
				iRetryCount++;
				Sleep(0);
				goto retry;
			}
		}
	}
	else
	{
/*    Accumulate screen updates.                                            */
		if (rectUpdate.left != -999)
		{
			rectUpdate.left   = min(rectUpdate.left  ,stRect.left);
			rectUpdate.right  = max(rectUpdate.right ,stRect.right);
			rectUpdate.top    = min(rectUpdate.top   ,stRect.top);
			rectUpdate.bottom = max(rectUpdate.bottom,stRect.bottom);
		}
		else
		{
			rectUpdate = stRect;
		}
	}
}
/****************************************************************************/
void EyegazeScreenUpdatesActive(BOOL bActive)
{
	BOOL bSuccess;
	int  iRetryCount;
	PAINTSTRUCT ps;

	bScreenUpdatesActive = bActive;

/* If we're re-enabling screen updates, update the accumulated rectangle.   */
	if (bActive == TRUE)
	{
		if (rectUpdate.left != -999)
		{
iRetryCount = 0;
retry:
			BeginPaint(hwndEyegaze, &ps);
			bSuccess = BitBlt(hdcWindow,
					 rectUpdate.left, rectUpdate.top,
					 rectUpdate.right  - rectUpdate.left,
					 rectUpdate.bottom - rectUpdate.top,
					 hdcEyegaze,
					 rectUpdate.left, rectUpdate.top,
					 SRCCOPY);
			EndPaint(hwndEyegaze, &ps);

/* 		If the call to BitBlt fails, record the error and retry the call	 */
/* 		up to four more times, yielding the CPU time slice each time.		 */
			if (bSuccess == FALSE)
			{
				if (iRetryCount < 4)
				{
					iRetryCount++;
					Sleep(0);
					goto retry;
				}
			}

		}
		rectUpdate.left = -999;
	}
}
/****************************************************************************/
void lct_dot(int x, int y, int iColor)
{
	/* hack for trace drawing mode: */
	if (bLocalXORMode == 3) iColor = 0x808080;
	SetPixel(hdcEyegaze, x, y, iColor);
	lct_RedrawScreen(x,y,x,y);
}
/****************************************************************************/
int lct_readdot(int x, int y)
{
	return GetPixel(hdcEyegaze, x, y);
}
/****************************************************************************/
void lct_line(int x1, int y1, int x2, int y2, int iColor)
{
	/* hack for trace drawing mode: */
	if (bLocalXORMode == 3) iColor = 0x808080;
	lctSetColor(iColor);
	MoveToEx(hdcEyegaze, x1, y1, NULL);

/* NOTE: Under Windows, the LineTo() function draws a line from the current */
/* 		pen position up to BUT NOT INCLUDING the line end point. 			 */
	LineTo(hdcEyegaze, x2, y2);
	SetPixel(hdcEyegaze, x2, y2, iColor);

	lct_RedrawScreen(x1, y1, x2, y2);
}
/****************************************************************************/
void lct_dashed_line(int x1, int y1, int x2, int y2,
							int iColor, int iLineStyle)
{
	/* hack for trace drawing mode: */
	if (bLocalXORMode == 3) iColor = 0x808080;
	lctSetColor(iColor);
	MoveToEx(hdcEyegaze, x1, y1, NULL);
	LineTo(hdcEyegaze, x2, y2);
	SetPixel(hdcEyegaze, x2, y2, iColor);

	lct_RedrawScreen(x1, y1, x2, y2);
}
/****************************************************************************/
int lct_rectangle(int x0,int y0,int x1,int y1,int iColor,int fill,int hatch)
{
	lctSetColor(iColor);

/* Draw a filled rectangle.																 */
	if (fill == 1)
	{
		lctSetBrush(iColor);
		Rectangle(hdcEyegaze, x0, y0, x1, y1);
		// NOTE: Rectangle() excludes the bottom and right edges
		lctSetBrush(BLACK);
	}
	else
/* Draw a hollow rectangle.																 */
	{
		lct_line(x0, y0, x1, y0, iColor);
		lct_line(x1, y0, x1, y1, iColor);
		lct_line(x1, y1, x0, y1, iColor);
		lct_line(x0, y1, x0, y0, iColor);
	}

/* Quiet the compiler */
	if (hatch);

	lct_RedrawScreen(x0, y0, x1, y1);

	return 0;
}
/****************************************************************************/
int lct_circle(int x, int y, int radius, int iColor, int fill)
{
	/* hack for trace drawing mode: */
	if (bLocalXORMode == 3) iColor = 0x808080;
	lctSetColor(iColor);

	Arc(hdcEyegaze, x-radius, y-radius, x+radius, y+radius,
						 x, y-radius, x, y-radius);

/* Quiet the compiler */
	if (fill);

	lct_RedrawScreen(x-radius, y-radius, x+radius, y+radius);

	return 0;
}
/****************************************************************************/
int lct_polygon(int *poly_x, int *poly_y, int sides, int close,
					  int iBorderColor, int bFill, int FillColor, int type)
{
	POINT *pt;
	int i;

/* Allocate memory and proceed if successful.                               */
	pt = malloc(sizeof(POINT) * (sides+1));
   if (pt != NULL)
   {
	   for (i = 0; i < sides; i++)
	   {
		   pt[i].x = poly_x[i];
		   pt[i].y = poly_y[i];
	   }
	   pt[sides].x = poly_x[0];
	   pt[sides].y = poly_y[0];

	   lctSetColor(iBorderColor);

	   MoveToEx(hdcEyegaze, pt[0].x, pt[0].y, NULL);

	   if (bFill == FALSE)
	   {
		   PolylineTo(hdcEyegaze, pt, sides+1);
	   }
	   else
	   {
		   PolylineTo(hdcEyegaze, pt, sides+1);
	   }

/*    Quiet the compiler */
	   if (close || FillColor || type);

	   lct_RedrawScreen(0, 0, lctScreenWidthPix(), lctScreenHeightPix());

	   free(pt);
      pt = NULL;
   }

	return 0;
}
/****************************************************************************/
void lct_ReadGraphicsArea(int iLeftX,	int iUpperY,
								  int iRightX, int iLowerY, char *array)
{
/* This function and the Write Graphics Area function are limited to a		 */
/* single read/write pair. 																 */

	WriteGraphicsAreaWidth	= iRightX - iLeftX  + 1;
	WriteGraphicsAreaHeight = iLowerY - iUpperY + 1;
	hdcMem = CreateCompatibleDC(hdcEyegaze);
	hbm = CreateCompatibleBitmap(hdcEyegaze,
										  WriteGraphicsAreaWidth,
										  WriteGraphicsAreaHeight);
	SelectObject(hdcMem, hbm);

	BitBlt(hdcMem, 0, 0, WriteGraphicsAreaWidth, WriteGraphicsAreaHeight,
			hdcEyegaze, iLeftX, iUpperY, SRCCOPY);
}
/****************************************************************************/
void lct_WriteGraphicsArea(int iLeftX, int iUpperY, char *array, int type)
{
	BitBlt(hdcEyegaze, iLeftX, iUpperY,
			 WriteGraphicsAreaWidth, WriteGraphicsAreaHeight,
			 hdcMem, 0, 0, SRCCOPY);

	lct_RedrawScreen(0, 0, lctScreenWidthPix(), lctScreenHeightPix());

/* Delete the bitmap and the device context. 										 */
	DeleteObject(hbm);
	DeleteDC(hdcMem);

/* Quiet the compiler */
	if (type);
}
/****************************************************************************/
// AWJ 5/20/06 - draws a duplicate of the screen for the 3Dat stereo display
void LCTDrawDuplicate()
{
// should be roughly a copy of lct_RedrawScreen(0, 0, iScreenWidthPix, lctScreenHeightPix());
   PAINTSTRUCT ps;

	BeginPaint(hwndEyegaze, &ps);
   BitBlt(hdcWindow,
				  0, 0,
				  lctScreenWidthPix(),
				  lctScreenHeightPix(),
				  hdcEyegaze,
				  lctScreenWidthPix(), 0,
				  SRCCOPY);
   TextOut(hdcWindow,  1500,  10, _T("w1500:10"), 8);
	EndPaint(hwndEyegaze, &ps);
}
/****************************************************************************/
/****************************************************************************/
/* Graphics Mode Functions:																 */
/****************************************************************************/
/****************************************************************************/
void lct_typedot(int i)
{
/* This function is passed a 1 to XOR everything and a 0 to not XOR. 		 */
/* or a 2 to draw in MASKPEN mode for TRACE and 3 to erase using MERGEPEN	 */
	bLocalXORMode = i;

	if (bLocalXORMode == 1)
	{
		SetROP2(hdcEyegaze,R2_NOT /*R2_XORPEN*/);
	}
	else if (bLocalXORMode == 2) // Draw High Bits (trace program)
	{
		SetROP2(hdcEyegaze,R2_MASKPEN);
	}
	else if (bLocalXORMode == 3) // Erase High Bits (trace program)
	{
		SetROP2(hdcEyegaze,R2_MERGEPEN);
	}
	else // all else is 'normal' (this is where '0' ends up)
	{
		SetROP2(hdcEyegaze,R2_COPYPEN);
	}
}
/****************************************************************************/
BOOL lct_GetXORMode(void)
{
	return bLocalXORMode;
}
/****************************************************************************/
/****************************************************************************/
/* Misc Functions:																			 */
/****************************************************************************/
/****************************************************************************/
void lct_sound(int frequency, unsigned int duration)
{
	int ms;

/* Note: Frequency must be in the range 37-32767 in Windows.					 */
	if (frequency < 37) frequency = 37;
	if (frequency >32767) frequency = 32767;

/* Note: since we're using 'clock ticks' from DOS here, I'm shortening      */
/* 55ms beeps to 10 ms. 																	 */
	ms = duration * 55;
	if (ms == 55) ms = 10;
	Beep(frequency, ms); 				// Input is in 55 ms clock ticks,
												// Windows Beep is in ms.
}
/****************************************************************************/
int lct_syspause(int hours, int min, int sec, int hsec)
{
	Sleep(hours* 60 * 60 * 1000 +
			min		 * 60 * 1000 +
			sec				* 1000 +
			hsec				*	 10);
	return 0;
}
/****************************************************************************/
int lct_lprtstat(int status)
{
	DWORD dwNeeded;
	DWORD dwReturned;
	PRINTER_INFO_4 *pinfo4;

	EnumPrinters(PRINTER_ENUM_LOCAL,
					 NULL,
					 4,				// information level
					 NULL,			// printer information buffer
					 0,				// size of printer information buffer
					 &dwNeeded, 	// bytes received or required
					 &dwReturned); // number of printers enumerated

	pinfo4 = malloc(dwNeeded);
   if (pinfo4 != NULL)
   {
	   EnumPrinters(PRINTER_ENUM_LOCAL,
					    NULL,
					    4,				// information level
					    (PBYTE)pinfo4, // printer information buffer
					    dwNeeded,		// size of printer information buffer
					    &dwNeeded, 	// bytes received or required
					    &dwReturned); // number of printers enumerated

/*    Free the memory allocated above. 													 */
	   free(pinfo4);
      pinfo4 = NULL;
   }

	if (dwReturned > 0)
	{
		return 0x90;
	}
	else
	{
		return 0;
	}
}
/****************************************************************************/
int lct_sysdate(_TCHAR *achDate, int iMaxDateStrLen, int format)
{
	struct _timeb	tstruct;
	struct tm	   today;
	_TCHAR        *pchMonth;

	_ftime_s(&tstruct);
	localtime_s(&today, &tstruct. time);

	switch(today.tm_mon)
	{
		case	0: pchMonth = _TEXT("Jan"); break;
		case	1: pchMonth = _TEXT("Feb"); break;
		case	2: pchMonth = _TEXT("Mar"); break;
		case	3: pchMonth = _TEXT("Apr"); break;
		case	4: pchMonth = _TEXT("May"); break;
		case	5: pchMonth = _TEXT("Jun"); break;
		case	6: pchMonth = _TEXT("Jul"); break;
		case	7: pchMonth = _TEXT("Aug"); break;
		case	8: pchMonth = _TEXT("Sep"); break;
		case	9: pchMonth = _TEXT("Oct"); break;
		case 10: pchMonth = _TEXT("Nov"); break;
		case 11: pchMonth = _TEXT("Dec"); break;
      default: pchMonth = _TEXT("Unk"); break;
	}

	swprintf_s(achDate, iMaxDateStrLen, _TEXT("%s %i, %i"),
      pchMonth, today.tm_mday, today.tm_year+1900);
	return 0;
}
/****************************************************************************/
int lct_systime(_TCHAR *achTime, int iMaxTimeStrLen, int format)
{
	struct _timeb	tstruct;
	struct tm	   today;

	_ftime_s(&tstruct);
	localtime_s(&today, &tstruct. time);

	swprintf_s(achTime, iMaxTimeStrLen, _TEXT("%02i:%02i:%02i"),
			    today.tm_hour, today.tm_min, today.tm_sec);

   if (format == 19) // avoid colons for file names...
   {
	   swprintf_s(achTime, iMaxTimeStrLen, _TEXT("%02i.%02i.%02i"),
			        today.tm_hour, today.tm_min, today.tm_sec);
   }
	return 0;
}
/****************************************************************************/
int lct_sysitime(int *hours, int *min, int *sec, int *hsec)
{
	struct _timeb tstruct;
	struct tm today;

	_ftime_s(&tstruct);
	localtime_s(&today, &tstruct.time);

	*hours = today.tm_hour;
	*min	 = today.tm_min;
	*sec	 = today.tm_sec;
	*hsec  = tstruct.millitm / 10;

	return 0;
}
/****************************************************************************/
int lct_copyfile(_TCHAR *achSourceFile, _TCHAR *achTargetFile)
{
	return CopyFile(achSourceFile, achTargetFile, FALSE);
}
/****************************************************************************/
void lct_curget(int *x,int *y)
{

}
/****************************************************************************/
int lct_curlocat(int x,int y)
{
	static HANDLE hNewScreenBuffer = NULL;
	COORD coord;

	//_tprintf(_TEXT("%c[%i;%iH"), 27, x, y);

	if (hNewScreenBuffer == NULL)
	{
	hNewScreenBuffer = CreateConsoleScreenBuffer(
		 GENERIC_READ |			  // read/write access
		 GENERIC_WRITE,
		 0,							  // not shared
		 NULL,						  // no security attributes
		 CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
		 NULL);						  // reserved; must be NULL
	}

	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(hNewScreenBuffer, coord);

	return 0;
}
/****************************************************************************/
int lct_scrndump(int printer,int dir,int size,int horzspac,int vertspac,
					  int formfeed,int copies,int iColors)
{
/* Quiet the compiler */
	if (printer || dir || size || horzspac || vertspac ||
		 formfeed || copies || iColors);

	return 0;
}
/****************************************************************************/
int lct_screendump(int iHorzDim, int iVertDim, int bFlipBlackToWhite)
{
// This code is from Petzold, chap 13, popprnt.c:

	static DOCINFO  di = { sizeof (DOCINFO) };
	static PRINTDLG pd;
	BOOL				 bSuccess ;
	int				 iXRes, iYRes, iDestHorzSize, iDestVertSize;
	int				 iError;

// Invoke Print common dialog box

	pd.lStructSize 		  = sizeof (PRINTDLG);
	pd.hwndOwner			  = hwndEyegaze;
	pd.hDevMode 			  = NULL;
	pd.hDevNames			  = NULL;
	pd.hDC					  = NULL;
	pd.Flags 				  = PD_ALLPAGES | PD_COLLATE |
									 PD_RETURNDC | PD_NOSELECTION; // | PD_SELECTLANDSCAPE;
	// PD_SELECTLANDSCAPE (and PD_SELECTPORTRAIT) are described in the online help
	// but won't compile.  They're not defined in COMMDLG.H   PLN
	pd.nFromPage			  = 0;
	pd.nToPage				  = 0;
	pd.nMinPage 			  = 0;
	pd.nMaxPage 			  = 0;
	pd.nCopies				  = 1;
	pd.hInstance			  = NULL;
	pd.lCustData			  = 0L;
	pd.lpfnPrintHook		  = NULL;
	pd.lpfnSetupHook		  = NULL;
	pd.lpPrintTemplateName = NULL;
	pd.lpSetupTemplateName = NULL;
	pd.hPrintTemplate 	  = NULL;
	pd.hSetupTemplate 	  = NULL;

/* Pop up the standard print dialog to select printer, etc. 					 */
	if (!PrintDlg (&pd)) return TRUE;

/* Get the dimensions of the printer device. 										 */
	iXRes = GetDeviceCaps(pd.hDC, HORZRES);
	iYRes = GetDeviceCaps(pd.hDC, VERTRES);

	iDestHorzSize = iXRes;
	iDestVertSize = iXRes * lctScreenHeightPix() / lctScreenWidthPix();
	if (iDestVertSize > iYRes)
	{
		iDestVertSize = iYRes;
		iDestHorzSize = iYRes * lctScreenWidthPix() / lctScreenHeightPix();
	}

	StartDoc (pd.hDC, &di);

	StartPage(pd.hDC);

	bSuccess = StretchBlt(pd.hDC,
								 0, 0,
								 iDestHorzSize,
								 iDestVertSize,
								 hdcEyegaze,
								 0, 0,
								 iHorzDim,
								 iVertDim,
								 bFlipBlackToWhite ? SRCINVERT : SRCCOPY);

	if (bSuccess == 0)
	{
		iError = GetLastError();
	}

	EndPage(pd.hDC);

	EndDoc (pd.hDC);

	DeleteDC (pd.hDC);

	return bSuccess;
}
/****************************************************************************/
void lct_clearscreen(int iColor)
{
	lctSetBrush(iColor);
	PatBlt(hdcEyegaze, 0, 0, lctScreenWidthPix(), lctScreenHeightPix(), PATCOPY);

	lct_RedrawScreen(0, 0, lctScreenWidthPix(), lctScreenHeightPix());
}
/****************************************************************************/
long lct_timer(void)
{
	return 0;
}
/****************************************************************************/
long lct_MStimer(void)
{
/* This function returns the amount of time that has elapsed since the		 */
/* last call to this function.															 */

	static ULONG ulTimeMS;
	ULONG 		 ulTimeNowMS;
	ULONG 		 ulTimeElapsedMS;
	static BOOL  bFirstCall = TRUE;
	int			 iHours, iMinutes, iSeconds, iHSeconds;
	int			 iJulianDay = 0; /* NOTE!!! Julian Day not used !!! */

	lct_sysitime(&iHours, &iMinutes, &iSeconds, &iHSeconds);

	ulTimeNowMS = iJulianDay * 24L * 60L * 60L * 1000L +
					  iHours 			 * 60L * 60L * 1000L +
					  iMinutes					 * 60L * 1000L +
					  iSeconds							 * 1000L +
					  iHSeconds 						 * 10L;

	if (ulTimeNowMS == 0)
	{
		_tprintf(_TEXT("Error in lct_timer() -- ulTimeNowMS = 0\n"));
	}

	if (ulTimeMS == 0)
	{
		ulTimeMS = ulTimeNowMS;

/* 	Check to make sure that we're not thinking this is the first call     */
/* 	twice.																					 */
		if (bFirstCall == FALSE)
		{
			_tprintf(_TEXT("Error in lct_timer() -- two 'first' calls\n"));
		}

		bFirstCall = FALSE;
		return 0;
	}
	else
	{
		ulTimeElapsedMS = ulTimeNowMS - ulTimeMS;
		ulTimeMS = ulTimeNowMS;
		return ulTimeElapsedMS;
	}
}
/****************************************************************************/
int lct_FindFirstFileName(_TCHAR *wildcard, _TCHAR *achFileName, int iMaxFileNameLen)
{
	struct _tfinddata_t stFD;

	hFindHandle = _tfindfirst(wildcard, &stFD);

	if (hFindHandle != -1)
	{
/* 	Under Windows NT, we're seeing the "." and ".." file names.  Skip     */
/* 	these.																					 */
		if ((_tcscmp(stFD.name, _TEXT("."))  == 0) ||
          (_tcscmp(stFD.name, _TEXT("..")) == 0))
		{
			return lct_FindNextFileName(achFileName, iMaxFileNameLen);
		}
		else
		{
			wcscpy_s(achFileName, iMaxFileNameLen, stFD.name);
			return 0;
		}
	}
	else
	{
		return 1;
	}
}
/****************************************************************************/
int lct_FindNextFileName(_TCHAR *achFileName, int iMaxFileNameLen)
{
	struct _tfinddata_t stFD;

	if (_tfindnext(hFindHandle, &stFD) == 0)
	{
/* 	Under Windows NT, we're seeing the "." and ".." file names.  Skip     */
/* 	these.																					 */
		if ((_tcscmp(stFD.name, _TEXT("."))  == 0) ||
          (_tcscmp(stFD.name, _TEXT("..")) == 0))
		{
			return lct_FindNextFileName(achFileName, iMaxFileNameLen);
		}
		else
		{
			wcscpy_s(achFileName, iMaxFileNameLen, stFD.name);
			return 0;
		}
	}
	else
	{
		return 1;
	}
}
/****************************************************************************/
void lct_set_printer_shade(int iColor, int shade)
{
/* Quiet the compiler */
	if (iColor || shade);
}
/****************************************************************************/
/* Petzold's font support:                                                  */
/****************************************************************************/
/*---------------------------------------
	EZFONT.C -- Easy Font Creation
					(c) Charles Petzold, 1998
  ---------------------------------------*/

#include <math.h>

HFONT EzCreateFont(HDC hdc, _TCHAR * szFaceName, int iDeciPtHeight,
						 int iDeciPtWidth, int iAttributes, BOOL fLogRes)
{
	FLOAT 	  cxDpi, cyDpi;
	HFONT 	  hFont;
	LOGFONT	  lf;
	POINT 	  pt;
	TEXTMETRIC tm;

	SaveDC(hdc);

	SetGraphicsMode(hdc, GM_ADVANCED);
	ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
	SetViewportOrgEx(hdc, 0, 0, NULL);
	SetWindowOrgEx(hdc, 0, 0, NULL);

	if (fLogRes)
	{
		cxDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSX);
		cyDpi = (FLOAT)GetDeviceCaps(hdc, LOGPIXELSY);
	}
	else
	{
		cxDpi = (FLOAT)(25.4 * GetDeviceCaps(hdc, HORZRES) /
									  GetDeviceCaps(hdc, HORZSIZE));

		cyDpi = (FLOAT)(25.4 * GetDeviceCaps(hdc, VERTRES) /
									  GetDeviceCaps(hdc, VERTSIZE));
	}

	pt.x = (int)(iDeciPtWidth	* cxDpi / 72);
	pt.y = (int)(iDeciPtHeight * cyDpi / 72);

	DPtoLP(hdc, &pt, 1);

	lf.lfHeight 		  = -(int)(fabs(pt.y) / 10.0 + 0.5);
	lf.lfWidth			  = 0;
	lf.lfEscapement	  = 0;
	lf.lfOrientation	  = 0;
	lf.lfWeight 		  = iAttributes & EZ_ATTR_BOLD		? 700 : 0;
	lf.lfItalic 		  = iAttributes & EZ_ATTR_ITALIC 	?	 1 : 0;
	lf.lfUnderline 	  = iAttributes & EZ_ATTR_UNDERLINE ?	 1 : 0;
	lf.lfStrikeOut 	  = iAttributes & EZ_ATTR_STRIKEOUT ?	 1 : 0;
	lf.lfCharSet		  = DEFAULT_CHARSET;
	lf.lfOutPrecision   = 0;
	lf.lfClipPrecision  = 0;
	lf.lfQuality		  = 0;
	lf.lfPitchAndFamily = iAttributes & EZ_ATTR_FIXEDPITCH ?  1 : 0;

	lstrcpy(lf.lfFaceName, szFaceName);

	hFont = CreateFontIndirect(&lf);

	if (iDeciPtWidth != 0)
	{
		hFont = (HFONT)SelectObject(hdc, hFont);

		GetTextMetrics(hdc, &tm);

		DeleteObject(SelectObject(hdc, hFont));

		lf.lfWidth = (int)(tm.tmAveCharWidth *
								 fabs(pt.x) / fabs(pt.y) + 0.5);

		hFont = CreateFontIndirect(&lf);
	}

	RestoreDC(hdc, -1);
	return hFont;
}
/****************************************************************************/
/* End Petzold font support.																 */
/****************************************************************************/
/****************************************************************************/
int lctNVisionSystems(void)
{
	_stEgHardware stEgHardware;

/* Read in the Eyegaze hardware configuration to get iNVisionSystems.	    */
	vReadHardwareSettings(&stEgHardware, LANG_DEFAULT);

	return stEgHardware.iNVisionSystems;
}
/****************************************************************************/
