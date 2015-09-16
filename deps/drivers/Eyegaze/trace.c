/****************************************************************************

File Name:		  TRACE.C
Program Name:	  Capture and redisplay a trace of the user's gazepoint

Company: 		  LC Technologies, Inc.
					  10363 Democracy Lane
                 Fairfax, VA 22030
					  (703) 385-7133

Date Created:	  07/18/88
Converted to Windows 01/2000

*****************************************************************************

	  The TRACE program displays a scene on the computer monitor, tracks and
stores the user's gazepoint for a given period of time while the user looks
at the screen, and then redisplays the user's gazepoint by superimposing the
gazepoint trace on the original display.	The purpose of this program is to
demonstrate the real-time capture and storage of gazepoint data for later
retrieval and analysis.  The code may be used as a starting point for
developers who wish to write their own eyegaze data collection and analysis
programs.
	In this example program, a brief menu displays the various data collection
and analysis options.  During the data collection phase, either a graphics
scene or a paragraph of text may be displayed.	As the user looks at the
screen, the Eyegaze System continually tracks the user's eye and stores the
gazepoint data for up to 20 seconds.  The length of the data collection
period can be terminated by pressing the Escape key on the keyboard.
	During the analysis phase, the program redisplays the scene and plots
the gazepoint data on the screen.  The gazepoint plots include both a) a
super-position of the gazepoint trace on the display scene and b)
position-versus-time graphs of the gazepoint x and y components and of the
pupil diameter.  The entire trace may be displayed statically or the trace
may be played back dynamically using a replay display which moves a "snake"
along showing the last half-second's worth of gazepoint data.  The snake
traces may be played back at various speeds.

*****************************************************************************/
#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <commdlg.h>

#undef DELETE
#include <stdio.h>
#include <conio.h>

#include <string.h>		 /* string manipulation functions						 */
#include <math.h> 		 /* high level math functions 							 */
#include <stdlib.h>		 /* C run-time functions									 */
#include <process.h> 	 /* computer process control functions 				 */
#include <malloc.h>		 /* Memory allocation function declarations			 */
#include <time.h> 		 /* time and date functions								 */

#include <lctfont.h> 	 /* Eyegaze font number definitions 					 */

#define EYEGAZE_STRUCTURES_DEFINED
#include <egwin.h>       /* Eyegaze functions and variables                 */
#include <lctsupt.h> 	 /* Eyegaze support functions 							 */
#include <igutil.h>		 /* Eyegaze System utility function definitions 	 */
#include <lctcolor.h>	 /* color constants for ESG calls						 */
#include <lctcalib.h>
#include <fixfunc.h> 	 /* Eyegaze fixation detection function				 */
#include <lctwin.h>

#include "Trace.h"

/* This program can be built with a special set of assumptions for the Mill */
/* Creek School District.	To turn on these assumptions, we define a 		 */
/* preprocessor constant here.  Ordinarily this should not be defined.		 */
#define NO_MILL_CREEK
#define MAX_FILE_NAME_LEN  260

/****************************************************************************/
/* Function Prototypes: 																	 */
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT uiMessage,
									 WPARAM wParam, LPARAM lParam);
DWORD WINAPI CreateDimBitmap(void *pVoid);
void			 InitTrace(void);
void			 lct_VisionDisplay(int bDisplayActive);
PBITMAPINFO  CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
void			 CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
									HBITMAP hBMP, HDC hDC);
void         errhandler(_TCHAR *ach, HWND hwnd);
BOOL CALLBACK SetReplaySpeedDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SetCollectionTimeDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GetUserNameDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

DWORD WINAPI CollectData(void *pVoid);
DWORD WINAPI DisplayFullTrace(void *pVoid);
DWORD WINAPI DisplayMovingTrace(void *pVoid);

DWORD WINAPI CallCalibrate(void *pVoid);
DWORD WINAPI DisplayEyeImages(void *pVoid);

void			 CollectGazeTrace(void);
void         AddFixation(int *iLastFixCollected, int iFixStartSample,
                         float fXFix, float fYFix,
                         int iSaccadeDuration, int iFixDuration);

/* Graphics functions																		 */

void	Draw_Text(void);
void	DrawBitmap(BOOL bBright, BOOL bInvertBitmap);
void	DrawRawTrace(void);
void	DrawRawTimePlots(void);
void	DrawTimeplotAxes(void);
void	PlotRawTimePoint(int,int);
void	ReplayMovingRawTrace(void);
void	DrawFixationTrace(void);
void	DrawScene(int iDisplayType, BOOL bInvertBitmap);
void	SelectText(HWND hwnd);
void	SelectBitmapLCT(HWND hwnd);
HBITMAP LoadBitmapLCT(_TCHAR *achFileName);
void	TraceClearScreen(void);
void	lctSaveEyeImage(void *hEyegaze);

void  SaveScreenBitmap(HWND hwnd);
BOOL  BitmapSaveNameLCT(HWND hwnd, _TCHAR *pFileName);
BOOL  ReplayFileNameLCT(HWND hwnd, _TCHAR *pFileName);

/* Utility Functions 																		 */

void	delay_msec(int);
void  WriteTraceDataFile(_TCHAR *pchFileName);
void  ReadTraceDataFile(_TCHAR *pchFileName);
int	nint(float x); 								 /* nearest integer				 */
int   iGetFileSequenceNumber(_TCHAR *achBaseName, int iStartingPosition);
void	EgWriteSavedImages(struct _stEgControl *pstEgControl);
void  UpdateTitleBarText(BOOL bAutoSaveActive, _TCHAR *pszUserName,
                         _TCHAR *pszFileDisplayed, _TCHAR *pszDataFileName);

/****************************************************************************/
/* External Variables:																		 */
HWND hwndEyegaze;
_TCHAR szAppName[] = _TEXT("Trace");

HDC		memdc;						/* Stores the virtual device handle 		 */
HBITMAP	hbit; 						/* Stores the virtual bitmap					 */
HBRUSH	hbrush;						/* Stores the brush handle 					 */
HDC      memdcBitmap;            /* Stores the virtual device handle        */
HBITMAP	hbitDesktop;				/* Stores the virtual bitmap					 */
HDC      memdcBitmapDim;         /* Stores the virtual device handle        */
HBITMAP	hbitDesktopDim;			/* Stores the virtual bitmap					 */

HDC		hdc;
HPALETTE hpal;
UINT		cxClient, cyClient;
RECT		stWindowRect;
int		iWindowWidthPix;
int		iWindowHeightPix;
int		iWindowHorzOffset;
int		iWindowVertOffset;
int		iScreenWidthPix;
int		iScreenHeightPix;
static _TCHAR achTextFileName[MAX_FILE_NAME_LEN+1];
static _TCHAR achBitmapFileName[MAX_FILE_NAME_LEN+1];
static _TCHAR achDimBitmapFileName[MAX_FILE_NAME_LEN+1];
_TCHAR   *pszDisplayedFile;

_TCHAR   achUserName[100];
_TCHAR   achTitleBarText[300];
_TCHAR   achDataFileName[MAX_FILE_NAME_LEN+1];

BOOL		bRawTraceOnscreen 	  = FALSE;
BOOL		bTimePlotOnscreen 	  = FALSE;
BOOL		bFixationTraceOnscreen = FALSE;
BOOL		bBitmapOnscreen		  = FALSE;
BOOL     bDisplayDataFileNameInWindow = TRUE;

void		*hEyegaze;
int		iManualKeyPressed = 0;
int		iNVisionSystems;

#if defined MILL_CREEK
	BOOL		bAutoSaveData = TRUE;
#else
	BOOL		bAutoSaveData = FALSE;
#endif

/****************************************************************************/
/* GENERAL CONSTANTS 																		 */

#define	EVER					;; 	/* Used in infinite for(EVER) loops 		 */

#define	MAX_GAZEPOINTS 36000 	/* maximum number of trace points that 	 */
											/*  that this program can accommodate		 */
#define	MAX_FIXATIONS	 3600 	/* maximum number of fixations that can	 */
											/*   be recorded									 */
#define	NO_SCENE 			 0 	/* values for scene_type						 */
#define	TEXT_SCENE			 2 	/* 													 */
#define	BITMAP_SCENE		 3 	/* 													 */
#define	SNAKE_LENGTH		10 	/* length of the replay snake 				 */
#define	FORWARD				 0 	/* direction of replay motion 				 */
#define	BACKWARD 			 1 	/* 													 */

#define	X_GRAPH_BOT 		50 	/* pixel locations for the time history	 */
#define	Y_GRAPH_BOT 	  105 	/*   plots											 */
#define	D_GRAPH_BOT 	  160 	/* 													 */
#define	GRAPH_LEFT			40 	/* horizontal coordinate of the first time */
											/*   point on the time plots (pixel)		 */
#define	GRAPH_HEIGHT		50 	/* height of the time plots (plots) 		 */

/*--------------------------------------------------------------------------*/

#define	RAW_GAZEPOINT_COLOR			 TR_RED
#define	GAZEPOINT_CONNECT_COLOR 	 TR_RED
#define	GAZEPOINT_DISCONNECT_COLOR  TR_MAGENTA
#define	FIXATION_COLOR 				 TR_BLUE
#define	FIXATION_CONNECT_COLOR		 TR_BLUE
#define	FIXATION_DISCONNECT_COLOR	 TR_CYAN
#define	TIME_HIGHLIGHT_COLOR 		 TR_GREEN
#define	TIME_AXIS_COLOR				 TR_BLUE

#define	SOLID_LINE	 0xFFFF
#define	DASHED_LINE  0xF0F0

/* iDisplayType constants: 																 */
#define	ORIGINAL_STIMULUS 1
#define	REPLAY_DATA 		2

/****************************************************************************/
/* Trace color definitions:	  B G R													 */
#define TR_BLACK	 0x7f7f7f  // 0 0 0 used to draw
#define TR_RED 	 0x7f7fff  // 0 0 1		 "
#define TR_GREEN	 0x7fff7f  // 0 1 0		 "
#define TR_BLUE	 0xff7f7f  // 1 0 0		 "
#define TR_CYAN	 0xffff7f  // 1 1 0		 "
#define TR_MAGENTA 0xff7fff  // 1 0 1		 "
#define TR_BROWN	 0x7fffff  // 0 1 1		 "
#define TR_WHITE	 0x808080  // 1 1 1 used to erase.

#define ROP_MODE_NORMAL 		 0  // raster operation
#define ROP_MODE_XOR 			 1
#define ROP_MODE_TRACE_DRAW	 2
#define ROP_MODE_TRACE_ERASE	 3

/****************************************************************************/
/* Structure Templates: 																	 */

struct _stRawGazepoint        /* data for a single raw gazepoint, i.e. the  */
										/*   gazepoint for a single camera sample 	 */
{
	int	bGazeTracked;			/* gaze-tracked flag (true or false)			 */
	int	iXGazeWindowPix;		/* gaze coordinate within window (pixels) 	 */
	int	iYGazeWindowPix;		/*  "                                         */
   float fPupilDiamMm;        /* pupil diameter (mm)                        */
   float fXEyeballMm;         /* x and y eyeball offsets with respect to    */
   float fYEyeballMm;         /*   the camera axis (mm)                     */
										/*   x positive: eye moved to user's right    */
										/*   y positive: eye moved up 					 */
   float fFocusOffsetMm;      /* distance of eye from focus plane (mm)      */
										/*   positive: eye farther from camera 		 */
   float fFocusRangeMm;       /* range from the camera-sensor plane to      */
										/*   the focus plane (mm)							 */
   int   iFixIndex;           /* index of the fixation that this point is   */
										/*   in, -1 if not part of a fixation			 */
};

struct _stFixPoint            /* data for a single gaze fixation point      */
{
   int iXFixPix;              /* x fixation coordinate (screen pixels)      */
   int iYFixPix;              /* y fixation coordinate (screen pixels)      */
   int iSaccadeDurCnt;        /* duration of the saccade preceeding the     */
										/*   fixation (camera samples)					 */
   int iFixDurCnt;            /* duration of the fixation (camera samples)  */
   int iFixStartSamp;         /* camera sample at which the fixation        */
										/*   started											 */
};
/****************************************************************************/
/* GLOBAL VARIABLES																			 */

struct _stRawGazepoint stRawGazepoint[MAX_GAZEPOINTS];
											/* array of raw gazepoint structures		 */
struct _stFixPoint stFixPoint[MAX_FIXATIONS];
											/* array of raw gazepoint structures		 */
int	iLastFixCollected;			/* index of the last fixation point 		 */
											/*   collected 									 */
int	iSceneType = NO_SCENE;		/* type of scene displayed 					 */
BOOL	bDataCollectedFlag=FALSE;	/* flag indicating whether data has been	 */
											/*   collected and is ready for replay 	 */
float fReplaySpeed = (float)1.0; /* relative speed of the trace replay		 */
											/*   with respect to real time				 */
											/*   (unitless)									 */
int	iReplayIntervalMs;			/* time delay between points in replay 	 */
											/*   (milliseconds)								 */
float fCollectionTimeSeconds=60.0F; /* length of time to collect gazepoint  */
											/*   data (seconds)								 */
int	iSamplesToCollect;			/* maximum number of gazepoint data 		 */
											/*   samples to collect 						 */
int	iLastSampleCollected;		/* index of the last gazepoint data sample */
											/*   actually collected during the test	 */
											/*   run 											 */

/*- - - - Control Parameters for Detecting Fixations - - - - - - - - - - - -*/

float fMinFixMs;						/* minimum time that gaze must be still	 */
											/*   to be considered a fixation 			 */
											/*   (milliseconds)								 */
float fGazeDeviationThreshMm; 	/* distance that a gazepoint may 			 */
											/*   vary from the average fixation 		 */
											/*   point and still be considered			 */
											/*   part of the fixation						 */
											/*   (millimeters)								 */

/*- - - - Plot Variables - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int	iPixPerSample; 				/* time plot scale factor (pix/gzpt-samp)  */
float fXGazeScaleFactor;			/* amplitude scale factors for plotting x  */
float fYGazeScaleFactor;			/*   and y gaze values on the time plots	 */
float fPupilDiameterScaleFactor; /* amplitude scale factor for plotting 	 */
											/*   pupil diameter								 */
float fMinPupilDiameterPlot;		/* minimum plot value for the pupil 		 */
											/*   diameter (mm)								 */

#ifndef NO_EYEGAZE
#define BUFFER_LEN 1
struct _stEgControl	 stEgControl;
#endif

BOOL	  bStopDataCollection;		/* Boolean flag used to interrupt data 	 */
											/*   collection.									 */
HMENU hMenu;							/* Handle to the program's menu            */
static BOOL bDisplayState = TRUE;
_TCHAR achCurrentDirectory[260];
struct _stEyeImageInfo stEyeImageInfo;
BOOL   bDisplayEyeImages = TRUE;

/****************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpszCmdParam, int nWinMode)
{
   #define MAX_CAPTION_LEN  200
	HWND			hwnd;
	MSG			msg;
	WNDCLASSEX	wndclass;
	HANDLE		hAccel;	// handle to the keyboard accelerators
   _TCHAR      achCaption[MAX_CAPTION_LEN+1];

/* Initialize window data, register window class and create main window.	 */
	if (!hPrevInstance)
	{
		wndclass.cbSize		  = sizeof(WNDCLASSEX);
		wndclass.style 		  = 0;
		wndclass.lpfnWndProc   = WindowFunc;
		wndclass.cbClsExtra	  = 0;
		wndclass.cbWndExtra	  = 0;
		wndclass.hInstance	  = hInstance;
		wndclass.hIcon 		  = LoadIcon(NULL,IDI_APPLICATION);
		wndclass.hIconSm		  = LoadIcon(NULL,IDI_APPLICATION);
		wndclass.hCursor		  = LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
      wndclass.lpszMenuName  = szAppName;
		wndclass.lpszClassName = szAppName;

/* 	Register the class																	 */
		if (!RegisterClassEx(&wndclass)) return FALSE;
	}

/* Get screen coordinates. 																 */
	iScreenWidthPix  = GetSystemMetrics(SM_CXSCREEN);
	iScreenHeightPix = GetSystemMetrics(SM_CYSCREEN);

	if (bAutoSaveData == TRUE)
      wcscpy_s(achCaption, MAX_CAPTION_LEN,
         _TEXT("Eyegaze Trace Demonstration - LC Technologies, Inc.  --  Auto Save Active"));
	else
      wcscpy_s(achCaption, MAX_CAPTION_LEN,
         _TEXT("Eyegaze Trace Demonstration - LC Technologies, Inc.  --  Auto Save Inactive"));


	hwnd = CreateWindow(szAppName, // window class name
      _TEXT("Eyegaze Trace Demonstration - LC Technologies, Inc.  --  Auto Save Inactive"),  // window caption
		WS_OVERLAPPEDWINDOW, 		 // window style
		0,//CW_USEDEFAULT,			 // initial x position
		0,//CW_USEDEFAULT,			 // initial y position
		iScreenWidthPix, //CW_USEDEFAULT, // initial x size
		iScreenHeightPix, //CW_USEDEFAULT,// initial y size
		NULL, 							 // parent window handle
		NULL, 							 // window menu handle
		hInstance,						 // program instance handle
		NULL);							 // creation parameters

/* Display the window																		 */
	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

   hAccel = LoadAccelerators(hInstance, TEXT("TRACE"));

/* Get and dispatch messages until a WM_QUIT message is received. 			 */
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);   /* Translates virtual key codes			 */
			DispatchMessage(&msg);	  /* Dispatches message to window			 */
		}
	}

	return msg.wParam;			  /* Returns the value from PostQuitMessage	 */
}
/****************************************************************************/
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT uiMessage,
									 WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static HINSTANCE hInstance;
	POINT 		point;
	HCURSOR		m_hWaitCursor;
	HCURSOR		m_hOldCursor;
	static BOOL bControlKeyPressed = FALSE;

	switch (uiMessage)
	{
		case WM_CREATE:
/* 		Save the instance handle for the dialog box calls below. 			 */
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

/* 		Determine the upper-left corner of the client area in screen		 */
/* 		coordinates.																		 */
			point.x = 0;
			point.y = 0;
			ClientToScreen(hwnd,&point);
			GetClientRect(hwnd, &stWindowRect);

			iWindowWidthPix	= stWindowRect.right - stWindowRect.left+1;
			iWindowHeightPix	= stWindowRect.bottom - stWindowRect.top+1;
			iWindowHorzOffset = point.x;
			iWindowVertOffset = point.y;

/* 		Create the virtual window. 													 */
			hdc	 = GetDC(hwnd);
			memdc  = CreateCompatibleDC(hdc);
			hbit	 = CreateCompatibleBitmap(hdc, iScreenWidthPix, iScreenHeightPix);
			SelectObject(memdc, hbit);

/* 		These lines create bitmaps for the "look at bitmap" functions.     */
			memdcBitmap 	= CreateCompatibleDC(hdc);
			hbitDesktop 	= CreateCompatibleBitmap(hdc, iWindowWidthPix, iWindowHeightPix);
			SelectObject(memdcBitmap, hbitDesktop);
			memdcBitmapDim = CreateCompatibleDC(hdc);
			hbitDesktopDim = CreateCompatibleBitmap(hdc, iWindowWidthPix, iWindowHeightPix);
			SelectObject(memdcBitmapDim, hbitDesktopDim);

			hbrush = GetStockObject(WHITE_BRUSH);
			SelectObject(memdc, hbrush);
			PatBlt(memdc, 0, 0, iScreenWidthPix, iScreenHeightPix, PATCOPY);

			hwndEyegaze = hwnd;
			lctSetWindowHandle(hwnd, memdc, hdc, szAppName, wcslen(szAppName));

/* 		Save the startup directory.													 */
			GetCurrentDirectory(260, achCurrentDirectory);

			{
				// make sure that the title bar and menu item reflect the state of
				// bAutoSave
				hMenu = GetMenu(hwnd);
				UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
				if (bAutoSaveData == TRUE)
				{
					CheckMenuItem(hMenu, ID_FILE_AUTOSAVEDATA, MF_CHECKED);
				}
				else
				{
					CheckMenuItem(hMenu, ID_FILE_AUTOSAVEDATA, MF_UNCHECKED);
/* 				Null-out the user name so we don't save bad data.         */
					achUserName[0] = '\0';
				}
			}

         achDataFileName[0] = '\0';

/* 		General Initialization code that used to be in the top of Trace:	 */
			InitTrace();
		   CreateThread(NULL, 0, DisplayEyeImages, 0, 0, NULL);
			break;

		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			BitBlt(hdc,
					 ps.rcPaint.left, ps.rcPaint.top,
					 ps.rcPaint.right  - ps.rcPaint.left,
					 ps.rcPaint.bottom - ps.rcPaint.top,
					 memdc,
					 ps.rcPaint.left, ps.rcPaint.top,
					 SRCCOPY);
			EndPaint(hwnd, &ps);
			break;

/* 	If the user sizes or moves the window, obtain the new size and          */
/* 	offsets from screen origin.													      */
		case WM_MOVE:
			point.x = 0;
			point.y = 0;
			ClientToScreen(hwnd,&point);
			iWindowHorzOffset = point.x;
			iWindowVertOffset = point.y;
		 break;

		case WM_SIZE:
			iWindowWidthPix  = LOWORD(lParam);
			iWindowHeightPix = HIWORD(lParam);
			break;

		case WM_SETFOCUS:
			InvalidateRect(hwnd,NULL,TRUE);
			return 0;

/* 	This code catches any attempts by child windows (push buttons) to 	 */
/* 	obtain the input focus and takes it back again.  (Petzold, 371)		 */
		case WM_KILLFOCUS:
			if (hwnd == GetParent((HWND)wParam)) SetFocus(hwnd);
			return 0;

		case WM_CHAR:
			if (wParam == 27) SendKeyCommandToCalibrate(CAL_KEY_COMMAND_ESCAPE);
			if (wParam == 32) SendKeyCommandToCalibrate(CAL_KEY_COMMAND_SPACE);
			iManualKeyPressed = wParam;
			SendKeystrokeToEyegaze(wParam);	// also send keystroke to cntrlig
			break;

/* 	We want to catch function keys here and pass them to cntrlig			 */
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 17: // Ctrl key
					bControlKeyPressed = TRUE;
					break;
				case 112:  // F1
					SendKeyCommandToCalibrate(CAL_KEY_COMMAND_RESTART);
					break;
				case 113:  // F2
					SendKeyCommandToCalibrate(CAL_KEY_COMMAND_SKIP);
					break;
				case 114:  // F3
					SendKeyCommandToCalibrate(CAL_KEY_COMMAND_ACCEPT);
					break;
				case 115:  // F4
					SendKeyCommandToCalibrate(CAL_KEY_COMMAND_RETRIEVE);
					break;

				case 119:  // F8 - Save video history ('movie')
// !!!!!!
					if (bControlKeyPressed == TRUE)
					{
						EgWriteSavedImages(&stEgControl);
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
					break;

				case 122:  // F11
					lctSaveEyeImage(stEgControl.hEyegaze);
					break;

/* 			Use the "F12" key to switch the video display on and off.       */
				case 123:
               if (bControlKeyPressed == TRUE)  // CTRL_F12 turns images off
               {
				      bDisplayState = !bDisplayState;
					   if (bDisplayState == FALSE)
					   {
						   InvalidateRect(hwndEyegaze, NULL, TRUE);
                  }
					}
					break;

				default:
					iManualKeyPressed = wParam;
					break;
			}
			break;

		case WM_KEYUP:
			switch(wParam)
			{
				case 17: // Ctrl key
					bControlKeyPressed = FALSE;
					break;
			}
			break;

		case WM_COMMAND:
			UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
			hMenu = GetMenu(hwnd);
			switch(LOWORD (wParam))
			{
				case ID_FILE_SELECTTEXT:
					SelectText(hwnd);
					iSceneType = TEXT_SCENE;
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,		MF_CHECKED);
					CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_UNCHECKED);
					break;
				case ID_FILE_SELECTBITMAP:
					SelectBitmapLCT(hwnd);
					iSceneType = BITMAP_SCENE;
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,		MF_UNCHECKED);
					CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_CHECKED);
					break;
				case ID_FILE_SETREPLAYSPEED:
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_SET_REPLAY_SPEED), hwnd, SetReplaySpeedDialogProc);
					break;
				case ID_FILE_SETCOLLECTIONTIME:
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_SET_COLLECTION_TIME), hwnd, SetCollectionTimeDialogProc);
					break;
				case ID_FILE_AUTOSAVEDATA:
					bAutoSaveData = !bAutoSaveData;
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					if (bAutoSaveData == TRUE)
					{
						CheckMenuItem(hMenu, ID_FILE_AUTOSAVEDATA, MF_CHECKED);
					}
					else
					{
						CheckMenuItem(hMenu, ID_FILE_AUTOSAVEDATA, MF_UNCHECKED);
/* 					Null-out the user name so we don't save bad data.         */
						achUserName[0] = '\0';
					}
					break;
				case ID_FILE_PRINTSCREEN:
					lct_screendump(iWindowWidthPix, iWindowHeightPix, FALSE);
					break;

				case ID_FILE_SAVEBITMAP:
					SaveScreenBitmap(hwnd);
					break;

				case ID_FILE_EXIT:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case ID_TRACKGAZE_CALIBRATE:
/* 				Disable all menu items while calibrate is running. 			 */
					EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_GRAYED);
					EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_GRAYED);

/* 				If Auto Save Data is turned on, prompt for a "user name"     */
/* 				before calibrating.														 */
					if (bAutoSaveData == TRUE)
					{
						DialogBox(hInstance, MAKEINTRESOURCE(IDD_GET_USER_ID), hwnd, GetUserNameDialogProc);
					}
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					bDisplayState = TRUE;

					CreateThread(NULL, 0, CallCalibrate, 0, 0, NULL);
					break;
				case ID_CALIBRATE_COMPLETE:
/* 				At the end of calibration, this message is posted. 			 */
					if (bAutoSaveData == TRUE)
					{
/* 					Use the same sequence number that will be used for the	 */
/* 					TraceData*.dat file. 												 */
						int iSeq;
                  iSeq = iGetFileSequenceNumber(_TEXT("TraceData*.DAT"), 9);

						EgWriteSavedImages(&stEgControl);
						InvalidateRect(hwndEyegaze, NULL, TRUE);
						UpdateWindow(hwndEyegaze);
					}
					break;
				case ID_TRACKGAZE_READTEXT:
					iSceneType = TEXT_SCENE;
/* 				Make sure we have a text file defined. 							 */
					if (achTextFileName[0] == '\0')
					{
						SelectText(hwnd);
						pszDisplayedFile = achTextFileName;
					}
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,		MF_CHECKED);
					CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_UNCHECKED);
					break;
				case ID_TRACKGAZE_LOOKATBITMAP:
					iSceneType = BITMAP_SCENE;
/* 				Make sure we have a bitmap file defined.							 */
					if (achBitmapFileName[0] == '\0')
					{
						SelectBitmapLCT(hwnd);
						pszDisplayedFile = achBitmapFileName;
					}
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,		MF_UNCHECKED);
					CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_CHECKED);
					break;
				case ID_TRACKGAZE_CLEARSCREEN:  // "W" accelerator
					TraceClearScreen();
					break;
            case ID_REPLAY_DISPLAYIMAGENAMEINIMAGE:
               bDisplayDataFileNameInWindow = !bDisplayDataFileNameInWindow;
               if (bDisplayDataFileNameInWindow == TRUE)
               {
                  CheckMenuItem(hMenu, ID_REPLAY_DISPLAYIMAGENAMEINIMAGE, MF_CHECKED);
                  lct_fontsprt(LCT_FONT_1, achDataFileName, 0, lct_fontheight(LCT_FONT_1, _TEXT("Mg")), 0, BLACK, 0);
               }
               else
               {
                  CheckMenuItem(hMenu, ID_REPLAY_DISPLAYIMAGENAMEINIMAGE, MF_UNCHECKED);
                  CreateThread(NULL, 0, DisplayFullTrace, 0, 0, NULL);
               }
               break;
				case ID_COLLECTDATA_START:
					UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);
					bStopDataCollection = FALSE;
					EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_GRAYED);
					EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_GRAYED);
					EnableMenuItem(hMenu, ID_COLLECTDATA_STOP,					 MF_ENABLED);
					CreateThread(NULL, 0, CollectData, 0, 0, NULL);
					break;
				case ID_COLLECTDATA_STOP:
					bStopDataCollection = TRUE;
					EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_ENABLED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_ENABLED);
					EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_ENABLED);
					EnableMenuItem(hMenu, ID_COLLECTDATA_STOP,					 MF_GRAYED);
					break;

				case ID_DISPLAY_DISPLAYCOMPLETERAWTRACE:
					CreateThread(NULL, 0, DisplayFullTrace, 0, 0, NULL);
					break;
				case ID_DISPLAY_DISPLAYMOVINGTRACE:
					EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_GRAYED);
					EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_GRAYED);
					EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_GRAYED);
					CreateThread(NULL, 0, DisplayMovingTrace, 0, 0, NULL);
					break;

				case ID_DISPLAY_TOGGLERAWTRACE:
					DrawRawTrace();
					break;

				case ID_DISPLAY_TOGGLEFIXATIONTRACE:
					DrawFixationTrace();
					break;
				case ID_DISPLAY_TOGGLETIMEPLOT:
					DrawRawTimePlots();
					break;
				case ID_DISPLAY_TOGGLESCREENDISPLAY:
					DrawScene(REPLAY_DATA, TRUE);
					break;

				case ID_REPLAY_LOADDATAFILE:
					ReplayFileNameLCT( hwnd, achDataFileName );
					m_hWaitCursor = LoadCursor( NULL, IDC_WAIT );
					m_hOldCursor = SetCursor( m_hWaitCursor );
					ReadTraceDataFile( achDataFileName );
					CreateThread(NULL, 0, DisplayFullTrace, 0, 0, NULL);
					SetCursor(m_hOldCursor);
					break;

				case ID_HELP_ABOUT:
					MessageBox(hwnd,TEXT ("Trace Demonstration Program\n")
                               TEXT ("(c) LC Technologies, 2000-2005"),
								  szAppName, MB_ICONINFORMATION | MB_OK);
					break;
			}
			break;

		case WM_DESTROY:
			#ifndef NO_EYEGAZE
			EgExit(&stEgControl);
			#endif
			ReleaseDC(hwnd, hdc);
			DeleteDC(memdc);
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, uiMessage, wParam, lParam);
			break;
	}
	return 0;
}
/****************************************************************************/
DWORD WINAPI CreateDimBitmap(_TCHAR *achDimBitmapFileName)
{
	int x, y;
	unsigned int iR, iG, iB, iPix;

	for (x = 0; x < iWindowWidthPix; x++)
	{
		for (y = 0; y < iWindowHeightPix; y++)
		{
			iPix = GetPixel(memdcBitmap,x,y);
			iR = ((iPix & 0x000000FF) >> 1) & 0x000000FF;
			iG = ((iPix & 0x0000FF00) >> 1) & 0x0000FF00;
			iB = ((iPix & 0x00FF0000) >> 1) & 0x00FF0000;
			iPix = iR+iG+iB + 0x00808080; // This brightens the bitmap
			SetPixel(memdcBitmapDim,x,y,iPix);

/* This ought to perform the same operation as above, just faster: */
// 		SetPixel(memdcBitmapDim,x,y,(iPix >> 1) || 0x00808080);
		}
	}
/* Write the resulting screen shot out to disk. 									 */
	CreateBMPFile(hwndEyegaze, achDimBitmapFileName,
					  CreateBitmapInfoStruct(hwndEyegaze, hbitDesktopDim),
					  hbitDesktopDim, memdcBitmapDim);

	return 0;
}
/****************************************************************************/
/* The following example code defines a function that allocates memory for  */
/* and initializes members within a BITMAPINFOHEADER structure.				 */
/* This is example code taken from MSDN.												 */

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP		bmp;
	PBITMAPINFO pbmi;
	WORD			cClrBits;

// Retrieve the bitmap's color format, width, and height.
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
       errhandler(_TEXT("GetObject"), hwnd);

// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		 cClrBits = 1;
	else if (cClrBits <= 4)
		 cClrBits = 4;
	else if (cClrBits <= 8)
		 cClrBits = 8;
	else if (cClrBits <= 16)
		 cClrBits = 16;
	else if (cClrBits <= 24)
		 cClrBits = 24;
	else cClrBits = 32;

// Allocate memory for the BITMAPINFO structure. (This structure
// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
// data structures.)

	if (cClrBits != 24)
		  pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
						 sizeof(BITMAPINFOHEADER) +
						 sizeof(RGBQUAD) * (1<< cClrBits));

// There is no RGBQUAD array for the 24-bit-per-pixel format.

	 else
		  pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
						 sizeof(BITMAPINFOHEADER));

// Initialize the fields in the BITMAPINFO structure.

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		 pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;

// Compute the number of bytes in the array of color
// indices and store the result in biSizeImage.
// Width must be DWORD aligned unless bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 15) /16 * 2
											* pbmi->bmiHeader.biHeight
											* cClrBits;
// Set biClrImportant to 0, indicating that all of the
// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;

	return pbmi;
}
/****************************************************************************/
/* The following example code defines a function that initializes the		 */
/* remaining structures, retrieves the array of palette indices, opens the  */
/* file, copies the data, and closes the file.										 */

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi,
						 HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;						 // file handle
	BITMAPFILEHEADER hdr;		 // bitmap file-header
	PBITMAPINFOHEADER pbih; 	 // bitmap info-header
	LPBYTE lpBits; 				 // memory pointer
	DWORD dwTotal; 				 // total count of bytes
	DWORD cb;						 // incremental count of bytes
	BYTE *hp;						 // byte pointer
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER) pbi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

   if (!lpBits) errhandler(_TEXT("GlobalAlloc"), hwnd);

// Retrieve the color table (RGBQUAD array) and the bits
// (array of palette indices) from the DIB.
	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS))
	{
       errhandler(_TEXT("GetDIBits"), hwnd);
	}

// Create the .BMP file.
	hf = CreateFile(pszFile,
						 GENERIC_READ | GENERIC_WRITE,
						 (DWORD) 0,
						 NULL,
						 CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 (HANDLE) NULL);
   if (hf == INVALID_HANDLE_VALUE) errhandler(_TEXT("CreateFile"), hwnd);
	hdr.bfType = 0x4d42; 		 // 0x42 = "B" 0x4d = "M"
// Compute the size of the entire file.
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
					 pbih->biSize + pbih->biClrUsed
					 * sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
						 pbih->biSize + pbih->biClrUsed
						 * sizeof (RGBQUAD);

// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL))
	{
      errhandler(_TEXT("WriteFile"), hwnd);
	}

// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
					  + pbih->biClrUsed * sizeof (RGBQUAD),
					  (LPDWORD) &dwTmp, ( NULL)))
       errhandler(_TEXT("WriteFile"), hwnd);

// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
          errhandler(_TEXT("WriteFile"), hwnd);

// Close the .BMP file.
   if (!CloseHandle(hf)) errhandler(_TEXT("CloseHandle"), hwnd);

// Free memory.
	GlobalFree((HGLOBAL)lpBits);
   lpBits = NULL;
}
/****************************************************************************/
void InitBMI(char *pData, int ImageSize, int Width, int Height)
{
	int tmp;
	BITMAPINFOHEADER *pBMI;
	memset( pData, 0, sizeof( BITMAPFILEHEADER ));
	*pData	  = 'B';
	*(pData+1) = 'M';
	// total size of the file
	tmp = ImageSize + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	*(pData+2) = tmp & 255;
	*(pData+3) = ( tmp >>  8 ) & 255;
	*(pData+4) = ( tmp >> 16 ) & 255;
	*(pData+5) = ( tmp >> 24 ) & 255;
	// The offset to the start of the actual image data
	tmp = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
	*(pData+10) = tmp & 255;
	*(pData+11) = ( tmp >>	8 ) & 255;
	*(pData+12) = ( tmp >> 16 ) & 255;
	*(pData+13) = ( tmp >> 24 ) & 255;

	pBMI = (BITMAPINFOHEADER*)((unsigned char*) pData + sizeof( BITMAPFILEHEADER ) );
	memset( pBMI, 0, sizeof( BITMAPINFOHEADER ) );
	pBMI->biSize = sizeof( BITMAPINFOHEADER );
	pBMI->biPlanes = 1;
	pBMI->biBitCount = 24;
	pBMI->biCompression = BI_RGB;
	pBMI->biXPelsPerMeter = 1;
	pBMI->biYPelsPerMeter = 1;
	pBMI->biClrUsed = 0;
	pBMI->biClrImportant = 0;
	pBMI->biWidth	= Width;
	pBMI->biHeight = Height;
	pBMI->biSizeImage = ImageSize;
}


void SaveScreenBitmap(HWND hwnd)
{
   _TCHAR FileName[261];
	FILE *pBMFile;
	HANDLE hDIBBuffer;
	char *pDIBBuffer, *pData;
	int ImageSize;
	HDC hDC, hMemDC;
	BITMAPINFO *pBMI;
	HBITMAP hBM;
	RECT Window;
	int ClientX, ClientY;
	FileName[0] = 0;
	if (BitmapSaveNameLCT( hwnd, FileName))
	{
		hDC = GetDC( hwnd );
		GetClientRect( hwnd, &Window );
		ClientX = Window.right - Window.left;
		ClientY = Window.bottom - Window.top;
		ImageSize = ClientY * ((( ClientX * 3 ) + 3 ) & 0xfffffffc );
		hDIBBuffer = GlobalAlloc( GMEM_FIXED, ImageSize + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) );
		pDIBBuffer = (char*) GlobalLock( hDIBBuffer );
		InitBMI( pDIBBuffer, ImageSize, ClientX, ClientY );
		pData = pDIBBuffer + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER );
		pBMI = (BITMAPINFO*)((unsigned char*) pDIBBuffer + sizeof( BITMAPFILEHEADER ));
		// Grab a DDB bitmap from the screen, then use CreateDIBitmap to convert it.
		hMemDC = CreateCompatibleDC( hDC );
		hBM = CreateCompatibleBitmap( hDC, ClientX, ClientY );
		SelectObject( hMemDC, hBM );
		BitBlt( hMemDC, 0, 0, ClientX, ClientY, hDC, 0, Window.top, SRCCOPY );
		GetDIBits( hDC, hBM, 0, ClientY, pData, pBMI, DIB_RGB_COLORS );
		//if (PaletteSize) OutImageFile.Write( BMPRead.pPalette, PaletteSize );
      _wfopen_s(&pBMFile, FileName, _TEXT("w") );
		fwrite( pDIBBuffer,	  14, 1, pBMFile );
		fwrite( pDIBBuffer+14, 40, 1, pBMFile );
		fwrite( pDIBBuffer+54, ImageSize, 1, pBMFile );
		fclose( pBMFile );
		DeleteObject( hBM );
		DeleteDC( hMemDC );
		ReleaseDC( hwnd, hDC );
		GlobalUnlock( hDIBBuffer );
		GlobalFree( hDIBBuffer );
      hDIBBuffer = NULL;
	}
}
/****************************************************************************/
void errhandler(_TCHAR *ach, HWND hwnd)
{
   _tprintf(_TEXT("error received from %s, error: %i\n"), ach, GetLastError());
}
/****************************************************************************/
// winbase.h isn't creating this macro
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

HBITMAP LoadBitmapLCT(_TCHAR *achFileName)
{
   HBITMAP hbm=NULL;

/* Check to see if the bitmap exists.                                       */
   if (GetFileAttributes(achFileName) == INVALID_FILE_ATTRIBUTES)
	{
      return hbm;
	}
	hbm = LoadImage(NULL,				// handle of the instance containing the image
						 achFileName,		// name or identifier of image
						 IMAGE_BITMAP, 	// type of image
						 0,					// desired width
						 0,					// desired height
						 LR_DEFAULTCOLOR | LR_LOADFROMFILE);	// load flags

	return hbm;
}
/****************************************************************************/
/****************************************************************************/
void InitTrace(void)
{
/* Initialize the Eyegaze software. 													 */
#ifndef NO_EYEGAZE
	stEgControl.iNDataSetsInRingBuffer = BUFFER_LEN;
	stEgControl.bEgCameraDisplayActive = FALSE;
	stEgControl.iScreenWidthPix		  = iScreenWidthPix;
	stEgControl.iScreenHeightPix		  = iScreenHeightPix;
	stEgControl.iEyeImagesScreenPos	  = 1;
	stEgControl.iCommType				  = EG_COMM_TYPE_LOCAL;
//   stEgControl.iCommType 				 = EG_COMM_TYPE_SOCKET;
   stEgControl.pszCommName = _TEXT("127.0.0.1");
	EgInit(&stEgControl);

	EgEyeImageInit(&stEyeImageInfo, 4);

/* Start the Eyegaze image processing when we're ready to start taking      */
/* data.                                                                    */
	stEgControl.bTrackingActive = TRUE;
#endif

/* Zero out the text file name.															 */
	achTextFileName[0] = '\0';
	achBitmapFileName[0] = '\0';

/* Initialize the font area and load the fonts. 									 */
	lct_fontinit();
	lctFontLoad(LCT_FONT_1);
	lctFontLoad(LCT_FONT_3);

/* Set the number of gazepoint data samples to be collected from the 		 */
/* collection time and the eyetracker sampling rate.								 */
#ifdef NO_EYEGAZE
	iSamplesToCollect = 0;
	fCollectionTimeSeconds = 0.0f;
#else
	iSamplesToCollect = (int)(fCollectionTimeSeconds * stEgControl.iSamplePerSec);

/* If the data arrays can't accommodate the desired collection time,        */
	if (iSamplesToCollect > MAX_GAZEPOINTS)
	{
/* 	Limit the number of gazepoint data samples that can be collected, 	 */
/* 	and reset the collection time appropriately. 								 */
		iSamplesToCollect = MAX_GAZEPOINTS;
		fCollectionTimeSeconds = (float)iSamplesToCollect /
									 stEgControl.iSamplePerSec;
	}
#endif

/* Initialize the fixation detection parameters.									 */
	fMinFixMs = 100;
	fGazeDeviationThreshMm	= 6.35F; 	/*  0.25 inch */

/* Set the delay time for the replay speed.											 */
#ifdef NO_EYEGAZE
	iReplayIntervalMs = nint(1000.0F / (60 * fReplaySpeed));
#else
	iReplayIntervalMs = nint(1000.0F / (stEgControl.iSamplePerSec * fReplaySpeed));
#endif

/* Read in the prior trace.dat file, if it exists. 								 */
   ReadTraceDataFile(_TEXT("trace.dat"));

	iNVisionSystems = lctNVisionSystems();

#if defined MILL_CREEK
	fCollectionTimeSeconds=300.0F;   /* length of time to collect gazepoint  */
#endif

}
/****************************************************************************/
DWORD WINAPI CollectData(void *pVoid)
{
#ifndef NO_EYEGAZE
/* This is the beginning of a new thread used to display the scene and		 */
/* collect the data. 																		 */

/* Switch off the display of the eye images in the other thread.            */
   bDisplayEyeImages = FALSE;

/* Clear the screen. 																		 */
	TraceClearScreen();

/* Kludge to keep file name off screen during data collection. 				 */
	achDataFileName[0] = '\0';

/* Put the graphics into Trace Drawing mode. 										 */
	lct_typedot(ROP_MODE_TRACE_DRAW);

/* Under Windows, make sure we turn off the eye images.							 */
	bDisplayState = FALSE;
	InvalidateRect(hwndEyegaze, NULL, TRUE);

/* Draw the scene.																			 */
	DrawScene(ORIGINAL_STIMULUS, 0);

/* Collect the gazepoint data.															 */
	CollectGazeTrace();

/* Disply the trace results.																 */
	DisplayFullTrace(pVoid);

/* At end of data collection, re-enable the menu items we disabled.			 */
	EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_ENABLED);
	EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_ENABLED);
	EnableMenuItem(hMenu, ID_COLLECTDATA_STOP,					 MF_GRAYED);

/* re-enable the display of eye images in the other thread.                 */
   bDisplayEyeImages = TRUE;

#endif
	return 0;
}
/****************************************************************************/
DWORD WINAPI DisplayMovingTrace(void *pVoid)
{
/* This is the beginning of a new thread used to display the Moving Trace.  */

   _TCHAR achEscapeMessage[] = _TEXT("Press ESCape to exit replay mode");
	int iLen;
	int iTaskbar;

/* Clear the screen. 																		 */
	TraceClearScreen();

/* Put the graphics into Trace Drawing mode. 										 */
	lct_typedot(ROP_MODE_TRACE_DRAW);

/* Under Windows, make sure we turn off the eye images.							 */
	bDisplayState = FALSE;
	InvalidateRect(hwndEyegaze, NULL, TRUE);

/* Draw the scene.																			 */
	DrawScene(REPLAY_DATA, FALSE);

/* Find out the height of the screen, not counting the area obscurred by	 */
/* the Windows taskbar. 																	 */
/* NOTE: This doesn't actually work correctly because Microsoft's           */
/* 		SystemParametersInfo() call doesn't work to determine the size of  */
/* 		the taskbar.  We're using SM_CYMENU as an approximation.           */
//   SystemParametersInfo (SPI_GETWORKAREA, sizeof (int), &iWorkArea, 0);
	iTaskbar = GetSystemMetrics(SM_CYMENU);

/* Display a message at the bottom of the screen indicating that the user	 */
/* must press ESCape to exit replay mode. 											 */
	iLen = lct_fontpixn(LCT_FONT_1, achEscapeMessage, 0, 0);
	lct_fontsprt(LCT_FONT_1, achEscapeMessage, iWindowWidthPix - iLen-10,
					 iWindowHeightPix-iTaskbar-10, 0, BLACK, 0);

	DrawRawTimePlots();
	ReplayMovingRawTrace();

	EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_ENABLED);
	EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_ENABLED);

	lct_fontsprt(LCT_FONT_1, achEscapeMessage, iWindowWidthPix - iLen-10,
					 iWindowHeightPix-iTaskbar-10, 0, BRIGHT_WHITE, 0);

	return 0;
}
/****************************************************************************/
DWORD WINAPI DisplayFullTrace(void *pVoid)
{
/* Clear the screen. 																		 */
	TraceClearScreen();

/* Put the graphics into Trace Drawing mode. 										 */
	lct_typedot(ROP_MODE_TRACE_DRAW);

/* Under Windows, make sure we turn off the eye images.							 */
	bDisplayState = FALSE;
	InvalidateRect(hwndEyegaze, NULL, TRUE);

/* Draw the scene.																			 */
	DrawScene(REPLAY_DATA, FALSE);

/* Draw the raw trace.																		 */
	DrawRawTrace();

/* Draw the raw time plots.																 */
	DrawRawTimePlots();

	return 0;
}
/****************************************************************************/
void DrawScene(int iDisplayType, BOOL bInvertBitmap)
{
/* Put the graphics into Trace Drawing mode. 										 */
	lct_typedot(ROP_MODE_TRACE_DRAW);

	if (iSceneType == TEXT_SCENE)   Draw_Text();
	if (iSceneType == BITMAP_SCENE) DrawBitmap(iDisplayType, bInvertBitmap);
}
/****************************************************************************/
void CollectGazeTrace(void)
{
#ifndef NO_EYEGAZE
	UINT	uiStartCameraFieldCount;
                                /* camera field count at which the gaze-    */
										  /*	 point data collection started			 */
	int	i; 						  /* test-run data sample index					 */
   int   iTestTimeIndex;        /* test-run sample index corresponding      */
										  /*	 to the image sample returned by 		 */
										  /*	 bTrackGazepoint()							 */
   int   iFixStartSample;       /* sample index at which fixation started   */
	int	iEyeMotionState;		  /* DETECT FIXATION RETURN VALUE:				 */
										  /*	MOVING				  0						 */
										  /*				 The eye was in motion			 */
                                /*            iMinFixSamples ago            */
										  /*	FIXATING 			  1						 */
										  /*				 The eye was fixating			 */
                                /*            iMinFixSamples ago            */
										  /*	FIXATION_COMPLETED  2						 */
										  /*				 A completed fixation has		 */
										  /*				 just been detected; the		 */
										  /*				 fixation ended					 */
                                /*            iMinFixSamples ago.           */
	int	ii;						  /* dummy gaze sample index for associating  */
										  /*	 gaze samples with fixation indices 	 */

/*----- DetectFixation() arguments -----------------------------------------*/

                                /* DETECT FIXATION INPUT PARAMETERS:        */
int   iMinFixSamples;           /* minimum fix time expressed in terms      */
                                /*   of gazepoint samples                   */
float fGazeDeviationThreshPix;  /* gaze deviation threshold (pixels)        */

                                /* DETECT FIXATION OUTPUT PARAMETERS:       */
                                /* Delayed Gazepoint data with              */
                                /*   fixation annotations:                  */
int   bGazepointFoundDelayed;   /* sample gazepoint-found flag,             */
                                /*   iMinFixSamples ago                     */
float fXGazeDelayed;            /* sample gazepoint coordinates,            */
float fYGazeDelayed;            /*   iMinFixSamples ago                     */
float fGazeDeviationDelayed;    /* deviation of the gaze from the           */
                                /*   present fixation,                      */
                                /*   iMinFixSamples ago                     */

                                /* Fixation Data - delayed:                 */
float fXFixDelayed;             /* fixation point as estimated              */
float fYFixDelayed;             /*   iMinFixSamples ago                     */
int   iSaccadeDurationDelayed;  /* duration of the saccade                  */
                                /*   preceeding the preset fixation         */
                                /*   (samples)                              */
int   iFixDurationDelayed;      /* duration of the present fixation         */
                                /*   (samples)                              */


/* Set the fixation-detection control parameter arguments.						 */
	iMinFixSamples = nint(fMinFixMs * stEgControl.iSamplePerSec / 1000.0F);
	fGazeDeviationThreshPix = fGazeDeviationThreshMm *
										 stEgControl.fHorzPixPerMm;

/* Initialize the fixation detection function.										 */
	iLastFixCollected = -1;
	InitFixation(iMinFixSamples);

/*---------------------- Start Data Collection Loop ------------------------*/

/* Collect the data samples.																 */
	for (i = 0; i < iSamplesToCollect; i++)
	{
/* - - - - - - - - - - - - -	Track Gazepoint - - - - - - - - - - - - - - - */

/* 	Track the user gazepoint.															 */
		EgGetData(&stEgControl);

		if (bDisplayState == TRUE)
		{
			EgEyeImageDisplay(0,
									stEgControl.iScreenWidthPix - stEyeImageInfo.iWidth,
									0,
									stEyeImageInfo.iWidth,
									stEyeImageInfo.iHeight,
									hdc);

         if (iNVisionSystems == 2)
         {
         EgEyeImageDisplay(1,
                           0,
                           0,
                           stEyeImageInfo.iWidth,
                           stEyeImageInfo.iHeight,
                           hdc);
         }
		}

/* 	If this is the first data sample in the test run,							 */
		if (i == 0)
		{
/* 		Save the starting camera field count.										 */
			uiStartCameraFieldCount = stEgControl.pstEgData->ulCameraFieldCount;
		}

/* - - - - - - - - - Fill In Missing Gazepoint Samples - - - - - - - - - - -*/

/* 	Compute the test time index corresponding to the image sample			 */
/* 	returned by EgGetData().  (offset from the beginning of this run) 	 */
      iTestTimeIndex = (int)((stEgControl.pstEgData->ulCameraFieldCount - uiStartCameraFieldCount));

/* 	If the eye tracker skipped any camera samples, fill in any missing	 */
/* 	gazepoint data with zeros. 														 */
      while (i < iTestTimeIndex)
		{
/* 		Record zeros:																		 */
         stRawGazepoint[i].bGazeTracked    = FALSE;
			stRawGazepoint[i].iXGazeWindowPix = 0;
			stRawGazepoint[i].iYGazeWindowPix = 0;
         stRawGazepoint[i].fPupilDiamMm    = 0.0F;
         stRawGazepoint[i].fXEyeballMm     = 0.0F;
         stRawGazepoint[i].fYEyeballMm     = 0.0F;
         stRawGazepoint[i].fFocusOffsetMm  = 0.0F;
         stRawGazepoint[i].fFocusRangeMm   = stEgControl.pstEgData->fFocusRangeImageTime;
         stRawGazepoint[i].iFixIndex       = -1;

/* 		Keep the DetectFixation() function up to date.							 */
			iEyeMotionState = DetectFixation(
									  stRawGazepoint[i].bGazeTracked,
									  (float)stRawGazepoint[i].iXGazeWindowPix,
									  (float)stRawGazepoint[i].iYGazeWindowPix,
									  fGazeDeviationThreshPix,
									  iMinFixSamples,

									  &bGazepointFoundDelayed,
									  &fXGazeDelayed, &fYGazeDelayed,
									  &fGazeDeviationDelayed,

									  &fXFixDelayed, &fYFixDelayed,
									  &iSaccadeDurationDelayed,
									  &iFixDurationDelayed);

/* 		If a completed fixation has just been detected, 						 */
			if (iEyeMotionState == FIXATION_COMPLETED)
			{
/* 			Compute the starting sample for the fixation.						 */
            iFixStartSample = i - (iFixDurationDelayed-1)
											- iMinFixSamples;

/* 			Store the fixation in the temporary fixation holding array. 	 */
            AddFixation(&iLastFixCollected, iFixStartSample,
								 fXFixDelayed, fYFixDelayed,
								 iSaccadeDurationDelayed, iFixDurationDelayed);

/* 			Tag all the raw gazepoints associated with this fixation.		 */
            for (ii = iFixStartSample;
                 ii < iFixStartSample + iFixDurationDelayed; ii++)
				{
               stRawGazepoint[ii].iFixIndex = iLastFixCollected;
				}
			}

/* 		Increment the sample index.													 */
			i++;

/* 		If the sample index exceeds the maximum data sample size,			 */
			if (i >= iSamplesToCollect)
			{
/* 			Break out of the fill-in loop.											 */
				break;
			}
		}

/* 	If the sample index exceeds the maximum data sample size,				 */
		if (i >= iSamplesToCollect)
		{
/* 		Break out of the data collection loop. 									 */
			break;
		}

/*- - - - - - - - - - - - Store Gazepoint Trace - - - - - - - - - - - - - - */

/* 	Record whether the gaze was found.												 */
		stRawGazepoint[i].bGazeTracked = stEgControl.pstEgData->bGazeVectorFound;

/* 	Originally assume the point is not associated with a fixation. 		 */
      stRawGazepoint[i].iFixIndex = -1;

/* 	If the gazepoint was tracked this sample, 									 */
		if (stRawGazepoint[i].bGazeTracked == TRUE)
		{
/* 		Record the raw gazepoint data:												 */
			stRawGazepoint[i].iXGazeWindowPix = stEgControl.pstEgData->iIGaze - iWindowHorzOffset;
			stRawGazepoint[i].iYGazeWindowPix = stEgControl.pstEgData->iJGaze - iWindowVertOffset;
         stRawGazepoint[i].fPupilDiamMm  = stEgControl.pstEgData->fPupilRadiusMm * 2;
         stRawGazepoint[i].fXEyeballMm   = stEgControl.pstEgData->fXEyeballOffsetMm;
         stRawGazepoint[i].fYEyeballMm   = stEgControl.pstEgData->fYEyeballOffsetMm;
         stRawGazepoint[i].fFocusOffsetMm = stEgControl.pstEgData->fFocusRangeOffsetMm;
         stRawGazepoint[i].fFocusRangeMm = stEgControl.pstEgData->fFocusRangeImageTime;
		}
/* 	Otherwise, if the gazepoint was not tracked this sample, 				 */
		else
		{
/* 		Record zeros:																		 */
			stRawGazepoint[i].iXGazeWindowPix 	  = 0;
			stRawGazepoint[i].iYGazeWindowPix 	  = 0;
         stRawGazepoint[i].fPupilDiamMm  = 0.0F;
         stRawGazepoint[i].fXEyeballMm   = 0.0F;
         stRawGazepoint[i].fYEyeballMm   = 0.0F;
         stRawGazepoint[i].fFocusOffsetMm = 0.0F;
         stRawGazepoint[i].fFocusRangeMm = stEgControl.pstEgData->fFocusRangeImageTime;
		}

/* - - - - - - - - - - - - - Process Fixations - - - - - - - - - - - - - - -*/

/* 	Check for fixations. 																 */
		iEyeMotionState = DetectFixation(
								  stRawGazepoint[i].bGazeTracked,
								  (float)stRawGazepoint[i].iXGazeWindowPix,
								  (float)stRawGazepoint[i].iYGazeWindowPix,
								  fGazeDeviationThreshPix,
								  iMinFixSamples,

								  &bGazepointFoundDelayed,
								  &fXGazeDelayed, &fYGazeDelayed,
								  &fGazeDeviationDelayed,

								  &fXFixDelayed, &fYFixDelayed,
								  &iSaccadeDurationDelayed,
								  &iFixDurationDelayed);

/* 	If a completed fixation has just been detected, 							 */
		if (iEyeMotionState == FIXATION_COMPLETED)
		{
/* 		Compute the starting sample for the fixation.							 */
         iFixStartSample = i - (iFixDurationDelayed-1)
										- iMinFixSamples;

/* 		Store the fixation in the temporary fixation holding array. 		 */
         AddFixation(&iLastFixCollected, iFixStartSample,
							 fXFixDelayed, fYFixDelayed,
							 iSaccadeDurationDelayed, iFixDurationDelayed);

/* 		Tag all the raw gazepoints associated with this fixation.			 */
         for (ii = iFixStartSample;
              ii < iFixStartSample + iFixDurationDelayed; ii++)
			{
            stRawGazepoint[ii].iFixIndex = iLastFixCollected;
			}
		}

/* 	Stop collecting data if the user requests it.								 */
		if (bStopDataCollection == TRUE) break;
	}

/*----------------------- End Data Collection Loop -------------------------*/

/* Record the index of the last gazepoint data sample actually collected	 */
/* during the test run. 																	 */
	iLastSampleCollected = i;
	if (iLastSampleCollected > iSamplesToCollect -1)
	{
		 iLastSampleCollected = iSamplesToCollect -1;
	}

/* If the eye is fixating, but the fixation has not been completed and		 */
/* therefore has not yet been recored, 												 */
	if (iEyeMotionState == FIXATING)
	{
/* 	Increase the duration of this last fixation to account for the 		 */
/* 	time delay in the DetectFixation() function. 								 */
		iFixDurationDelayed += iMinFixSamples;

/* 	Compute the starting sample for the fixation.								 */
      iFixStartSample = iLastSampleCollected - iFixDurationDelayed + 1;

/* 	Record the last fixation.															 */
      AddFixation(&iLastFixCollected, iFixStartSample,
						 fXFixDelayed, fYFixDelayed,
						 iSaccadeDurationDelayed, iFixDurationDelayed);

/* 	Tag all the raw gazepoint samples associated with this last fixation. */
      for (ii = iFixStartSample; ii <= iLastSampleCollected; ii++)
		{
         stRawGazepoint[ii].iFixIndex = iLastFixCollected;
		}
	}

/* Set the data-collected flag true.													 */
	bDataCollectedFlag = TRUE;

/* Write the trace data file. 															 */
   WriteTraceDataFile(_TEXT("trace.dat"));

/* If Auto Save Data is turned on, generate a serialized file name and		 */
/* write the data out.																		 */
	if (bAutoSaveData == TRUE)
	{
		int iSeq;

      iSeq = iGetFileSequenceNumber(_TEXT("TraceData*.DAT"), 9);
      swprintf_s(achDataFileName, MAX_FILE_NAME_LEN, _TEXT("TraceData%04i.DAT"), iSeq);

		WriteTraceDataFile(achDataFileName);
	}
#endif
}
/****************************************************************************/
void AddFixation(int *iLastFixCollected, int iFixStartSample,
                  float fXFix, float fYFix,
                  int iSaccadeDuration, int iFixDuration)

/* This function adds a fixation to the fixation data array.					 */

{
/* If the fixation array is already full, 											 */
	if (*iLastFixCollected >= MAX_FIXATIONS-1)
	{
/* 	Return without adding the fixation. 											 */
		return;
	}

/* Increment the fixation index. 														 */
	*iLastFixCollected = *iLastFixCollected + 1;

/* Store the fixation in the temporary holding array. 							 */
   stFixPoint[*iLastFixCollected].iXFixPix       = nint(fXFix);
   stFixPoint[*iLastFixCollected].iYFixPix       = nint(fYFix);
   stFixPoint[*iLastFixCollected].iSaccadeDurCnt = iSaccadeDuration;
   stFixPoint[*iLastFixCollected].iFixDurCnt     = iFixDuration;
   stFixPoint[*iLastFixCollected].iFixStartSamp  = iFixStartSample;
}
/****************************************************************************/
/****************************************************************************/
/* Graphics functions																		 */
/****************************************************************************/
FILE* UnicodeOpenFile( const _TCHAR *pFileName )
{
// This function returns the open file pointer
// It attempts to open the file to support Unicode reads but if it's not a unicode file
// it closes it and reopens it as a regular file.  The _fgets, etc seem to do the translation
// to Unicode.
   FILE *pFile;

   #ifdef _UNICODE
   {
     // open the file in binary mode and check for the Unicode character
     unsigned short Tmp;
     _wfopen_s(&pFile, pFileName, _T("rb") );
     if (pFile != NULL)
     {
        fread( &Tmp, 2, 1, pFile );
        if (Tmp != 0xfeff)
        {
           fclose(pFile);
           _wfopen_s(&pFile, pFileName, _T("r") );
        }
     }
   }
   #else
      pFile = fopen( pFileName, _T("r") );
   #endif
   return pFile;
}
/****************************************************************************/
void Draw_Text(void)

/* This function draws the text from the selected trace_?.txt file on the	 */
/* screen.                                                                  */

{
   FILE	*tracetxt;
   int	i;
   int	x0, y0;                         /* screen plot coordinates (pix)   */
   _TCHAR  achTemp[MAX_FILE_NAME_LEN+1];
   _TCHAR  achFaceName[MAX_FILE_NAME_LEN+1] = _TEXT("Times New Roman");
   int	iDeciPoints = 300;
   int	iLineSpacing = 768/18;
   int	iLineSpacingPercent = 120;
   BOOL	bSelectFont = FALSE;
   int	iSkippedLines = 0;
   int   iLineCharLength;

   UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);

/* Open the selected trace text file.                                       */
   if (( tracetxt = UnicodeOpenFile( achTextFileName )) == NULL )
   {
      MessageBox(hwndEyegaze, _TEXT("The text file (above) cannot be located."), achTextFileName, MB_OK );
		return;
   }

/* Set the font size and line spacing to be used.                           */

   _fgetts(achTemp, MAX_FILE_NAME_LEN, tracetxt);
   achTemp[_tcsclen(achTemp)-1] = '\0'; // remove <CR>
   if (_tcsnccmp(achTemp, _TEXT("FACENAME:"), 9) == 0)
   {
      wcscpy_s(achFaceName, MAX_FILE_NAME_LEN, &achTemp[9]);
		bSelectFont = TRUE;
   }

   _fgetts(achTemp, MAX_FILE_NAME_LEN, tracetxt);
   achTemp[_tcsclen(achTemp)-1] = '\0'; // remove <CR>
   if (_tcsnccmp(achTemp, _TEXT("FONTSIZE:"), 9) == 0)
   {
      iDeciPoints = _ttoi(&achTemp[9]);
		bSelectFont = TRUE;
   }

   _fgetts(achTemp, MAX_FILE_NAME_LEN, tracetxt);
   achTemp[_tcsclen(achTemp)-1] = '\0'; // remove <CR>
   if (_tcsnccmp(achTemp, _TEXT("LINESPAC:"), 9) == 0)
   {
      iLineSpacingPercent = _ttoi(&achTemp[9]);
		bSelectFont = TRUE;
   }

	if (bSelectFont == TRUE)
   {
		lctFontLoadSpecific(LCT_FONT_3, achFaceName, iDeciPoints, 0);
   }

   iLineSpacing = lct_fontheight(LCT_FONT_3, _TEXT("Mg"))  * iLineSpacingPercent / 100;

/* Rewind to the beginning of the file.                                     */
   rewind(tracetxt);

/* Turn off screen updates while we're modifying the screen.  This way the  */
/* update will flash to the screen all at once when we're done.             */
   EyegazeScreenUpdatesActive(FALSE);

/* Cycle through all the lines in the text file.                            */
   for (i = 0; i < 30; i++)
   {
      // On the first line, remove the UNICODE flag (0xfeff)
      #ifdef _UNICODE
         if (i == 0)
         {
            fread( achTemp, 2, 1, tracetxt );
            if (achTemp[0] != 0xfeff) rewind(tracetxt); // if it wasn't the flag, rewind the file
         }
      #endif
/*    Get the line of text from the text file.                              */
      if (_fgetts(achTemp, MAX_FILE_NAME_LEN, tracetxt) == NULL) break;

/*    Skip past any lines with display directives.                          */
      if ((_tcsnccmp(achTemp, _TEXT("FACENAME:"), 9) == 0) ||
          (_tcsnccmp(achTemp, _TEXT("FONTSIZE:"), 9) == 0) ||
          (_tcsnccmp(achTemp, _TEXT("LINESPAC:"), 9) == 0))
      {
	      iSkippedLines++;
	      continue;
      }

/*    Strip the carriage return off the end of the line of text.            */
      //achTemp[_tcsclen(achTemp)-1] = '\0';
      iLineCharLength = _tcsclen( achTemp );
      while(( iLineCharLength > 0 )&&
            (( achTemp[ iLineCharLength-1 ] == '\r' )|| // remove <CR><LF>
               ( achTemp[ iLineCharLength-1 ] == '\n' )))
      {      
         achTemp[ iLineCharLength - 1 ] = '\0';
         iLineCharLength--;
      }

/*    Determine the vertical position of the text line on the screen.       */
      y0 = 200 + iLineSpacing * (i - iSkippedLines);

/*    Display the line of text.                                             */
      x0 = 10;
      x0 = 80; // 070711 kludge to insert left margin in text before NHK demo
      lct_fontsprt(LCT_FONT_3, achTemp, x0, y0, 0, BLACK, 0);
   }

/* Close the trace text file.                                               */
   fclose(tracetxt);

/* Display the generated file name on the screen.                           */
   if (bDisplayDataFileNameInWindow == TRUE)
   {
      lct_fontsprt(LCT_FONT_1, achDataFileName, 0, lct_fontheight(LCT_FONT_1, _TEXT("Mg")), 0, BLACK, 0);
   }
   UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);

   EyegazeScreenUpdatesActive(TRUE);
}
/****************************************************************************/
void DrawBitmap(int iDisplayType, BOOL bInvertBitmap)

/* This function draws the bitmap image for the user to scan.					 */
/* The function is called with iDisplayType=ORIGINAL_STIMULUS on initial	 */
/* stimulus displays and with iDisplayType=REPLAY_DATA, indicating a dim	 */
/* display, for use when replaying gaze traces. 									 */
/* The bInvertBitmap boolean is used to remove the bitmap from the scene    */
/* by XORing it with itself.                                                */

{
	static HBITMAP hbm = NULL;

/* Switch back to 'normal' drawimg mode to display the bitmap.              */
	lct_typedot(ROP_MODE_NORMAL);

	if (achBitmapFileName[0] == '\0')
	{
		SelectBitmapLCT(hwndEyegaze);
		pszDisplayedFile = achBitmapFileName;
	}

/* If the display is bright, for gazepoint data collection, 					 */
	if (iDisplayType == ORIGINAL_STIMULUS)
	{
/* 	If we've loaded a bitmap before this, delete it.                      */
		if (hbm != NULL) DeleteObject(hbm);

/* 	Load the "bright" bitmap.                                             */
		hbm = LoadBitmapLCT(achBitmapFileName);
      if (hbm == NULL)
      {
         MessageBox(hwndEyegaze, _TEXT("The bitmap file does not exist"), achBitmapFileName, MB_OK );
         SelectBitmapLCT(hwndEyegaze);
      }
	}
/* Otherwise, if this is a dim display used for trace replays, 				 */
	else
	{
/* 	If we've loaded a bitmap before this, delete it.                      */
		if (hbm != NULL) DeleteObject(hbm);

/* 	Attempt to load the "dim" version of the display.                     */
		hbm = LoadBitmapLCT(achDimBitmapFileName);
/* 	If a dim file does not exist, 													 */
		if (hbm == NULL)
		{
			int iLen;
         _TCHAR achBusyMessage[] = _TEXT("Please Wait, Generating Faded Image");

/* 		Display a message on screen that we're creating the dim bitmap.    */
			iLen = lct_fontpixn(LCT_FONT_1, achBusyMessage, 0, 0);
			lct_fontsprt(LCT_FONT_1, achBusyMessage, (iWindowWidthPix-iLen)/2,
							 iWindowHeightPix/2, 0, BLACK, 0);

/* 		If we've loaded a bitmap before this, delete it.                   */
			if (hbm != NULL) DeleteObject(hbm);

/* 		Create a dim bitmap and load it. 											 */
			hbm = LoadBitmapLCT(achBitmapFileName);
			SelectObject(memdcBitmap, hbm);
			CreateDimBitmap(achDimBitmapFileName);

/* 		If we've loaded a bitmap before this, delete it.                   */
			if (hbm != NULL) DeleteObject(hbm);

			hbm = LoadBitmapLCT(achDimBitmapFileName);
		}
	}

/* Make sure we successfully loaded a bitmap.										 */
	if (hbm == NULL)
	{
		MessageBox(hwndEyegaze,TEXT ("Trace Demonstration Program\n")
									 TEXT ("Error loading bitmap file"),
									 szAppName, MB_ICONINFORMATION | MB_OK);
	}
	else
	{
		SelectObject(memdcBitmap, hbm);
	}

	if (iDisplayType == ORIGINAL_STIMULUS)
	{
/* 	Clear the booleans since we're about to replace the screen.           */
		bRawTraceOnscreen = FALSE;
		bTimePlotOnscreen = FALSE;
		bFixationTraceOnscreen = FALSE;

		BitBlt(memdc, 0, 0, iWindowWidthPix, iWindowHeightPix, memdcBitmap, 0, 0, SRCCOPY  /*SRCINVERT*/);

		bBitmapOnscreen = TRUE;
	}
	else // iDisplayType == REPLAY_DATA
	{
// pre-typedot(ROP_MODE_TRACE_DRAW) method:		BitBlt(memdc, 0, 0, iWindowWidthPix, iWindowHeightPix, memdcBitmap, 0, 0, bInvertBitmap ? SRCINVERT : SRCCOPY);

		if (bBitmapOnscreen == TRUE)
		{
			lct_clearscreen(BRIGHT_WHITE);
			bBitmapOnscreen = FALSE;
		}
		else
		{
			BitBlt(memdc, 0, 0, iWindowWidthPix, iWindowHeightPix, memdcBitmap, 0, 0, SRCCOPY);
			bBitmapOnscreen = TRUE;
		}

/* 	Restore the other displays on top of the bitmap.							 */
		if (bRawTraceOnscreen == TRUE)
		{
			bRawTraceOnscreen = FALSE;
			DrawRawTrace();
		}
		if (bTimePlotOnscreen == TRUE)
		{
			bTimePlotOnscreen = FALSE;
			DrawRawTimePlots();
		}
		if (bFixationTraceOnscreen == TRUE)
		{
			bFixationTraceOnscreen = FALSE;
			DrawFixationTrace();
		}
	}

/* Display the generated file name on the screen.									 */
   if (bDisplayDataFileNameInWindow == TRUE)
   {
      lct_fontsprt(LCT_FONT_1, achDataFileName, 0, lct_fontheight(LCT_FONT_1, _TEXT("Mg")), 0, BLACK, 0);
   }
   UpdateTitleBarText(bAutoSaveData, achUserName, pszDisplayedFile, achDataFileName);

   InvalidateRect(hwndEyegaze, NULL, TRUE);

/* Without this call, it's possible to get only a part of the bitmap        */
/* displayed on the screen.																 */
	UpdateWindow(hwndEyegaze);

/* Switch back into Trace Drawing mode now that we're done.                 */
	lct_typedot(ROP_MODE_TRACE_DRAW);

}
/****************************************************************************/
void DrawRawTrace(void)

/* This function superimposes the trace of the user's raw gazepoint on      */
/* the screen. 																				 */

{
	int	i;
   int   iXGazePast;
   int   iYGazePast;
   int   iLineColor;
   BOOL  bPastPointPlotted;           /* flag indicating that a past        */
												  /*	 gazepoint has been plotted, so	 */
												  /*	 we know whether or not to draw	 */
												  /*	 a connecting line					 */
	int	x0, y0, x1, y1;				  /* screen plot coordinates (pix)		 */
   UINT  uiLinePattern;               /* solid or dashed line pattern       */

/* If the trace is on screen we're erasing now, else we're drawing.         */
	if (bRawTraceOnscreen == TRUE)
	{
		lct_typedot(ROP_MODE_TRACE_ERASE);
		bRawTraceOnscreen = FALSE;
	}
	else
	{
		lct_typedot(ROP_MODE_TRACE_DRAW);
		bRawTraceOnscreen = TRUE;
	}

/* Turn off screen updates while we're modifying the screen.  This way the  */
/* update will flash to the screen all at once when we're done.             */
	EyegazeScreenUpdatesActive(FALSE);

/* Initially, no past gazepoint has been plotted.									 */
   bPastPointPlotted = FALSE;

/* Plot the first raw gazepoint. 														 */
	if (stRawGazepoint[0].bGazeTracked == TRUE)
	{
		x0 = stRawGazepoint[0].iXGazeWindowPix;
		y0 = stRawGazepoint[0].iYGazeWindowPix;
		lct_circle(x0, y0, 2, RAW_GAZEPOINT_COLOR, 0);
      iXGazePast = stRawGazepoint[0].iXGazeWindowPix;
      iYGazePast = stRawGazepoint[0].iYGazeWindowPix;
      bPastPointPlotted = TRUE;
	}

/* Loop over the remaining raw gazepoint data samples.							 */
	for (i = 1; i <= iLastSampleCollected; i++)
	{
		if (stRawGazepoint[i].bGazeTracked == TRUE)
		{
/* 		Set the color of the gazepoint connecting line depending on 		 */
/* 		whether the previous point was found or not. 							 */
			if (stRawGazepoint[i-1].bGazeTracked == TRUE)
			{
            iLineColor   = GAZEPOINT_CONNECT_COLOR;
            uiLinePattern = SOLID_LINE;
			}
			else
			{
            iLineColor   = GAZEPOINT_DISCONNECT_COLOR;
            uiLinePattern = DASHED_LINE;
			}

/* 		If a past gazepoint was plotted, 											 */
         if (bPastPointPlotted == TRUE)
			{
/* 			Plot the connecting line from the past known point to the		 */
/* 			present point. 																 */
            x0 = iXGazePast;
            y0 = iYGazePast;
				x1 = stRawGazepoint[i].iXGazeWindowPix;
				y1 = stRawGazepoint[i].iYGazeWindowPix;
            lct_dashed_line(x0, y0, x1, y1, iLineColor, uiLinePattern);
			}

/* 		Plot a red circle at the present gazepoint.								 */
			x0 = stRawGazepoint[i].iXGazeWindowPix;
			y0 = stRawGazepoint[i].iYGazeWindowPix;
			lct_circle(x0, y0, 2, RAW_GAZEPOINT_COLOR, 0);

/* 		Set the coordinates of the past known point for future use. 		 */
         iXGazePast = stRawGazepoint[i].iXGazeWindowPix;
         iYGazePast = stRawGazepoint[i].iYGazeWindowPix;
         bPastPointPlotted = TRUE;
		}
	}

	EyegazeScreenUpdatesActive(TRUE);
}
/****************************************************************************/
void DrawRawTimePlots(void)

/* This function draws the three time plots for raw x-gaze, y-gaze and		 */
/* pupil diameter.																			 */

{
	int	i;

/* If the trace is on screen we're erasing now, else we're drawing.         */
	if (bTimePlotOnscreen == TRUE)
	{
		lct_typedot(ROP_MODE_TRACE_ERASE);
		bTimePlotOnscreen = FALSE;
	}
	else
	{
		lct_typedot(ROP_MODE_TRACE_DRAW);
		bTimePlotOnscreen = TRUE;
	}

/* Turn off screen updates while we're modifying the screen.  This way the  */
/* update will flash to the screen all at once when we're done.             */
	EyegazeScreenUpdatesActive(FALSE);

/* Plot the axes for the three time graphs.											 */
	DrawTimeplotAxes();

/* Plot the raw gazepoint data for the individual time points. 				 */
	for (i = 0; i <= iLastSampleCollected; i++)
	{
		PlotRawTimePoint(i,RAW_GAZEPOINT_COLOR);
	}

	EyegazeScreenUpdatesActive(TRUE);
}
/****************************************************************************/
void	DrawTimeplotAxes(void)

/* This function draws the axes for the gazepoint and pupil diameter time	 */
/* plots.																						 */

{
   int   iTick, iLastTick;       /* tick mark indices                       */
   int   iPixPerSec;             /* time plot scale factor (pix/sec)        */
	_TCHAR achPrintStr[21]; 		/* print string									 */
	int	x0, x1, y0, y1;			/* screen plot coordinates (pix) 			 */
	int	i; 							/* gazepoint index								 */
   float fMinPupilDiamMm;        /* minimum and maximum pupil diameters     */
   float fMaxPupilDiamMm;        /*   recorded during test run              */

/* If there are too many sample points to plot with an expanded time scale, */
	if (iSamplesToCollect >= (iWindowWidthPix - GRAPH_LEFT) / 2)
	{
/* 	Set the pixels-per-sample to 1, for a non-expanded time scale. 		 */
		iPixPerSample = 1;
	}
/* Otherwise, if there is enough room for an expanded time plot,				 */
	else
	{
/* 	Set the pixels-per-sample to 2.													 */
		iPixPerSample = 2;
	}

/* Set the time plot scale factor (pix/gzpt-samp). 								 */
#ifdef NO_EYEGAZE
   iPixPerSec = iPixPerSample * 60;
#else
   iPixPerSec = iPixPerSample * stEgControl.iSamplePerSec;
#endif

/* Compute the amplitude scale factors for plotting x and y gaze values.	 */
	/* Note: the -160 on the monitor height accounts for the fact that		 */
	/*  there is no display on the upper portion of the screen. 				 */
	fXGazeScaleFactor = (float)GRAPH_HEIGHT / iWindowWidthPix;
	fYGazeScaleFactor = (float)GRAPH_HEIGHT /(iWindowHeightPix - 160);

/* Find the minimum and maximum pupil diameters for the test run.	          */
   fMinPupilDiamMm = 10.0F;
   fMaxPupilDiamMm =  0.0F;
	for (i = 0; i <= iLastSampleCollected; i++)
	{
/* 	If the gazepoint was tracked this sample, 									 */
		if (stRawGazepoint[i].bGazeTracked == TRUE)
		{
/* 		Keep track of the min and max pupil diameter.							 */
         if (stRawGazepoint[i].fPupilDiamMm < fMinPupilDiamMm)
             fMinPupilDiamMm = stRawGazepoint[i].fPupilDiamMm;
         if (stRawGazepoint[i].fPupilDiamMm > fMaxPupilDiamMm)
             fMaxPupilDiamMm = stRawGazepoint[i].fPupilDiamMm;
		}
	}

/* Set the minimum plotted pupil diameter to be slightly less than the		 */
/* actual minimum diameter, but make sure the plot minimum is at least		 */
/* 0.5 mm less than the max diameter.													 */
   fMinPupilDiameterPlot = fMinPupilDiamMm - 0.1F;
   if (fMinPupilDiameterPlot > fMaxPupilDiamMm - 0.5F)
       fMinPupilDiameterPlot = fMaxPupilDiamMm - 0.5F;

/* Compute the amplitude scale factor for plotting the pupil diameter.		 */
	fPupilDiameterScaleFactor = GRAPH_HEIGHT /
                         (fMaxPupilDiamMm - fMinPupilDiameterPlot);

/* Draw the time (horizontal) axes for the x, y and diameter plots.			 */
	x0 = GRAPH_LEFT;
	x1 = GRAPH_LEFT + iSamplesToCollect * iPixPerSample - 1;

	y0 = X_GRAPH_BOT + 1;
	y1 = y0;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

	y0 = D_GRAPH_BOT + 1;
	y1 = y0;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

	y0 = Y_GRAPH_BOT + 1;
	y1 = y0;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

/* Draw the vertical axes for the x, y and diameter plots.						 */
	x0 = GRAPH_LEFT - 1;
	x1 = x0;

	y0 = X_GRAPH_BOT;
	y1 = X_GRAPH_BOT - GRAPH_HEIGHT;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

	y0 = Y_GRAPH_BOT;
	y1 = Y_GRAPH_BOT - GRAPH_HEIGHT;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

	y0 = D_GRAPH_BOT;
	y1 = D_GRAPH_BOT - GRAPH_HEIGHT;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

/* Label the graphs. 																		 */
	x0 = 2;
	y0 = X_GRAPH_BOT - GRAPH_HEIGHT/2 + 4;
   lct_fontsprt(LCT_FONT_1, _TEXT(" X "), x0, y0, 0, TIME_AXIS_COLOR, 0);

	y0 = Y_GRAPH_BOT - GRAPH_HEIGHT/2 + 4;
   lct_fontsprt(LCT_FONT_1, _TEXT(" Y "), x0, y0, 0, TIME_AXIS_COLOR, 0);

	y0 = D_GRAPH_BOT - GRAPH_HEIGHT/2 + 4;
   lct_fontsprt(LCT_FONT_1, _TEXT("DIA"), x0, y0, 0, TIME_AXIS_COLOR, 0);

/* Label the minimum plotted pupil diameter. 										 */
	x0 = GRAPH_LEFT - 4;
	x1 = GRAPH_LEFT - 2;
	y0 = D_GRAPH_BOT;
	y1 = y0;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);
	swprintf_s(achPrintStr, 20, _TEXT("%3.1f"), fMinPupilDiameterPlot);
	x0 = GRAPH_LEFT - 30;
	y0 = D_GRAPH_BOT +  2;
	lct_fontsprt(LCT_FONT_1, achPrintStr, x0, y0, 0, TIME_AXIS_COLOR, 0);
	x0 = GRAPH_LEFT - 26;
	y0 = D_GRAPH_BOT + 10;
   lct_fontsprt(LCT_FONT_1, _TEXT("mm"), x0, y0, 0, TIME_AXIS_COLOR, 0);

/* Label the maximum pupil diameter.													 */
	x0 = GRAPH_LEFT - 4;
	x1 = GRAPH_LEFT - 2;
	y0 = D_GRAPH_BOT - GRAPH_HEIGHT;
	y1 = y0;
	lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);
   swprintf_s(achPrintStr, 20, _TEXT("%3.1f"), fMaxPupilDiamMm);
	x0 = 10;
	y0 = D_GRAPH_BOT - GRAPH_HEIGHT + 4;
	lct_fontsprt(LCT_FONT_1, achPrintStr, x0, y0, 0, TIME_AXIS_COLOR, 0);

/* Put one second tick marks along the time axes.                           */
   iLastTick = (int)(fCollectionTimeSeconds + 0.99);
   for (iTick = 0; iTick <= iLastTick; iTick++)
	{
      x0 = GRAPH_LEFT + iTick * iPixPerSec - 1;
		x1 = x0;

		y0 = X_GRAPH_BOT + 2;
		y1 = X_GRAPH_BOT + 4;
		lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

		y0 = Y_GRAPH_BOT + 2;
		y1 = Y_GRAPH_BOT + 4;
		lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);

		y0 = D_GRAPH_BOT + 2;
		y1 = D_GRAPH_BOT + 4;
		lct_line(x0, y0, x1, y1, TIME_AXIS_COLOR);
	}

/* Label the collection time. 															 */
   x0 = GRAPH_LEFT + iLastTick * iPixPerSec - 8;
	if (x0 > iWindowWidthPix - 15)
		 x0 = iWindowWidthPix - 15;
	y0 = D_GRAPH_BOT+12;
   swprintf_s(achPrintStr, 20, _TEXT("%i"), iLastTick);
	lct_fontsprt(LCT_FONT_1, achPrintStr, x0, y0, 0, TIME_AXIS_COLOR, 0);

   x0 = GRAPH_LEFT + iLastTick * iPixPerSec - 12;
	if (x0 > iWindowWidthPix - 23)
		 x0 = iWindowWidthPix - 23;
	y0 = D_GRAPH_BOT+20;
   lct_fontsprt(LCT_FONT_1, _TEXT("sec"), x0, y0, 0, TIME_AXIS_COLOR, 0);
}
/****************************************************************************/
void PlotRawTimePoint(int index, int color)

/* This function plots the raw gazepoint and pupil diameter values for		 */
/* the present time index on time graphs. 											 */

{
	int	x_plot;						  /* plot amplitude for x gaze value (pix) */
	int	y_plot;						  /* plot amplitude for y gaze value (pix) */
	int	d_plot;						  /* plot amplitude for pupil diam (pix)	 */
	int	past_index; 				  /* time index									 */
	int	x_plot_past;				  /* plot amplitudes of prior values (pix) */
	int	y_plot_past;				  /*													 */
	int	d_plot_past;				  /*													 */
	int	x0, y0, x1, y1;			  /* screen plot coordinates (pix)			 */

/* Set the index of the previous point.												 */
	past_index = index - 1;
	if (past_index < 0) past_index = 0;

/* Scale the data for plotting.															 */
	x_plot		= nint(fXGazeScaleFactor *
							 stRawGazepoint[index].iXGazeWindowPix);
	x_plot_past = nint(fXGazeScaleFactor *
							 stRawGazepoint[past_index].iXGazeWindowPix);

	y_plot = 	  nint(fYGazeScaleFactor *
							 (iWindowHeightPix -
							  stRawGazepoint[index].iYGazeWindowPix));
	y_plot_past = nint(fYGazeScaleFactor *
							 (iWindowHeightPix -
							  stRawGazepoint[past_index].iYGazeWindowPix));

	d_plot = 	  nint(fPupilDiameterScaleFactor *
                      (stRawGazepoint[index].fPupilDiamMm -
							  fMinPupilDiameterPlot));
	d_plot_past = nint(fPupilDiameterScaleFactor *
                      (stRawGazepoint[past_index].fPupilDiamMm -
							  fMinPupilDiameterPlot));

/* Limit the plot points to the graph dimensions.									 */
	if (x_plot < 0) x_plot = 0;
	if (y_plot < 0) y_plot = 0;
	if (d_plot < 0) d_plot = 0;

	if (x_plot_past < 0) x_plot_past = 0;
	if (y_plot_past < 0) y_plot_past = 0;
	if (d_plot_past < 0) d_plot_past = 0;

	if (x_plot > GRAPH_HEIGHT) x_plot = GRAPH_HEIGHT;
	if (y_plot > GRAPH_HEIGHT) y_plot = GRAPH_HEIGHT;
	if (d_plot > GRAPH_HEIGHT) d_plot = GRAPH_HEIGHT;

	if (x_plot_past > GRAPH_HEIGHT) x_plot_past = GRAPH_HEIGHT;
	if (y_plot_past > GRAPH_HEIGHT) y_plot_past = GRAPH_HEIGHT;
	if (d_plot_past > GRAPH_HEIGHT) d_plot_past = GRAPH_HEIGHT;

/* Shift the plot points to their respective axes and invert the sign to	 */
/* accommodate j increaseing downward. 												 */
	x_plot = X_GRAPH_BOT - x_plot;
	y_plot = Y_GRAPH_BOT - y_plot;
	d_plot = D_GRAPH_BOT - d_plot;

	x_plot_past = X_GRAPH_BOT - x_plot_past;
	y_plot_past = Y_GRAPH_BOT - y_plot_past;
	d_plot_past = D_GRAPH_BOT - d_plot_past;

/* If the present gazepoint is valid,													 */
	if (stRawGazepoint[index].bGazeTracked == TRUE)
	{
/* 	Plot the x and y points.															 */
/* 	If the prior gazepoint is valid, 												 */
		if (stRawGazepoint[past_index].bGazeTracked == TRUE)
		{
/* 		Draw a line from the last point to the present one.					 */
			if (iPixPerSample > 1)
			{
				x0 = index * 2 - 1 + GRAPH_LEFT;
				x1 = x0;
				y0 = x_plot_past;
				y1 = x_plot;
				lct_line(x0, y0, x1, y1, color);

				x0 = index * 2 + GRAPH_LEFT;
				y0 = x_plot;
				lct_dot(x0, y0, color);

				x0 = index * 2 - 1 + GRAPH_LEFT;
				x1 = x0;
				y0 = y_plot_past;
				y1 = y_plot;
				lct_line(x0, y0, x1, y1, color);

				x0 = index * 2 + GRAPH_LEFT;
				y0 = y_plot;
				lct_dot(x0, y0, color);
			}
			else
			{
				x0 = index + GRAPH_LEFT;
				x1 = x0;
				y0 = x_plot_past,
				y1 = x_plot;
				lct_line(x0, y0, x1, y1, color);

				y0 = y_plot_past,
				y1 = y_plot;
				lct_line(x0, y0, x1, y1, color);
			}
		}
/* 	But if the eye was not found in the prior point,							 */
		else
		{
/* 		Plot the present point only.													 */
			if (iPixPerSample > 1)
			{
				x0 = index * 2 - 1 + GRAPH_LEFT;
				x1 = index * 2 + GRAPH_LEFT;
				y0 = x_plot;
				y1 = y0;
				lct_line(x0, y0, x1, y1, color);

				y0 = y_plot;
				y1 = y0;
				lct_line(x0, y0, x1, y1, color);
			}
			else
			{
				x0 = index + GRAPH_LEFT;
				y0 = x_plot;
				lct_dot(x0, y0, color);

				y0 = y_plot;
				lct_dot(x0, y0, color);
			}
		}

/* 	Plot the pupil diameter. (solid graph) 										 */
		if (iPixPerSample > 1)
		{
			x0 = index * 2 - 1 + GRAPH_LEFT;
			x1 = x0;
			y0 = D_GRAPH_BOT;
			y1 = d_plot;
			lct_line(x0, y0, x1, y1, color);

			x0 = index * 2  + GRAPH_LEFT;
			x1 = x0;
			lct_line(x0, y0, x1, y1, color);
		}
		else
		{
			x0 = index + GRAPH_LEFT;
			x1 = x0;
			y0 = D_GRAPH_BOT;
			y1 = d_plot;
			lct_line(x0, y0, x1, y1, color);
		}
	}

/* Otherwise, it the gazepoint was not tracked, 									 */
	else
	{
/* 	Set the color to the gazepoint-disconnect color.							 */
		color = GAZEPOINT_DISCONNECT_COLOR;

/* 	Put a dot on the horizontal axis to indicate a failed track.			 */
		if (iPixPerSample > 1)
		{
			x0 = index * 2 - 1 + GRAPH_LEFT;
			x1 = index * 2 + GRAPH_LEFT;

			y0 = X_GRAPH_BOT + 1;
			y1 = y0;
			lct_line(x0, y0, x1, y1, color);

			y0 = Y_GRAPH_BOT+1;
			y1 = y0;
			lct_line(x0, y0, x1, y1, color);

			y0 = D_GRAPH_BOT+1;
			y1 = y0;
			lct_line(x0, y0, x1, y1, color);
		}
		else
		{
			x0 = index + GRAPH_LEFT;

			y0 = X_GRAPH_BOT + 1;
			lct_dot(x0, y0, color);

			y0 = Y_GRAPH_BOT + 1;
			lct_dot(x0, y0, color);

			y0 = D_GRAPH_BOT + 1;
			lct_dot(x0, y0, color);
		}
	}
}
/****************************************************************************/
void ReplayMovingRawTrace(void)

/* This function replays the raw trace over time, using the moving "snake"  */
/* display. 																					 */

{
	int	i; 							 /* time index of snake head					 */
	int	ii;							 /* time index of snake tail					 */
	int	direction;					 /* direction of the replay motion			 */
	BYTE	motion;						 /* flag indicating whether the replay 	 */
											 /*	display is in motion 					 */
	int	x0, y0, x1, y1;			 /* screen plot coordinates (pix)			 */
	int	iTimePointColor;			 /* used to highlight the moving trace 	 */

/* Initialize the position, direction and motion of the replay.				 */
	i = 0;
	direction = FORWARD;
	motion = TRUE;

/* Remove any buffered characters from the keyboard buffer. 					 */
	iManualKeyPressed = 0;

/* Stop the Eyegaze image processing while we're replaying.                 */
#ifndef NO_EYEGAZE
	stEgControl.bTrackingActive = FALSE;
#endif

/* Loop through the points.																 */
	for (EVER)
	{
/* 	If the snake is in motion, 														 */
		if (motion == TRUE)
		{
/* 		If the replay is moving forward, 											 */
			if (direction == FORWARD)
			{
/* 			Increment its position. 													 */
				++i;
/* 			When the snake reaches the end of the collected data, 			 */
				if (i > iLastSampleCollected)
				{
/* 				Limit the position.														 */
					i = iLastSampleCollected;
/* 				Stop the motion.															 */
					motion = FALSE;
/* 				Reverse the direction of motion. 									 */
					direction = BACKWARD;
				}
			}
			else
			{
/* 			Decrement its position. 													 */
				--i;
/* 			When the snake reaches the beginning,									 */
				if (i < SNAKE_LENGTH-1)
				{
/* 				Limit the position.														 */
					i = SNAKE_LENGTH-1;
/* 				Stop the motion.															 */
					motion = FALSE;
/* 				Reverse the direction of motion. 									 */
					direction = FORWARD;
				}
			}

/*----------------------------- The Head -----------------------------------*/

/* 		If we're moving forward, draw the head, else erase.          */
			if (direction == FORWARD)
			{
				lct_typedot(ROP_MODE_TRACE_DRAW);
				iTimePointColor = TIME_HIGHLIGHT_COLOR;
			}
			else
			{
				lct_typedot(ROP_MODE_TRACE_ERASE);
				iTimePointColor = RAW_GAZEPOINT_COLOR;
			}

/* 		Draw/erase the head of the snake.											 */
			if (stRawGazepoint[i].bGazeTracked == TRUE)
			{
/* 			If the gazepoint was found in the previous field,					 */
				if (stRawGazepoint[i-1].bGazeTracked == TRUE)
				{
/* 				Plot the connecting line from the previous to the present	 */
/* 				point.																		 */
					x0 = stRawGazepoint[i-1].iXGazeWindowPix;
					y0 = stRawGazepoint[i-1].iYGazeWindowPix;
					x1 = stRawGazepoint[i].iXGazeWindowPix;
					y1 = stRawGazepoint[i].iYGazeWindowPix;
					lct_line(x0, y0, x1, y1, GAZEPOINT_CONNECT_COLOR);
				}

/* 			Plot a red circle at the present gazepoint.							 */
				x0 = stRawGazepoint[i].iXGazeWindowPix;
				y0 = stRawGazepoint[i].iYGazeWindowPix;
				lct_circle(x0, y0, 2, RAW_GAZEPOINT_COLOR, 0);
			}

/* 		Plot the time history point for the head of the snake.				 */
			lct_typedot(ROP_MODE_TRACE_ERASE);
			PlotRawTimePoint(i, iTimePointColor);
			lct_typedot(ROP_MODE_TRACE_DRAW);
			PlotRawTimePoint(i, iTimePointColor);

/*------------------------------- The Tail ---------------------------------*/
/* 		If we're moving forward, erase the tail, else draw.                */
			if (direction == FORWARD)
			{
				lct_typedot(ROP_MODE_TRACE_ERASE);
				iTimePointColor = RAW_GAZEPOINT_COLOR;
			}
			else
			{
				lct_typedot(ROP_MODE_TRACE_DRAW);
				iTimePointColor = TIME_HIGHLIGHT_COLOR;
			}

/* 		Set the tail index.																 */
			ii = i - SNAKE_LENGTH;

/* 		If the tail is on the screen, 												 */
			if (ii > 0)
			{
/* 			Draw/erase the tail of the snake.										 */
				if (stRawGazepoint[ii].bGazeTracked == TRUE)
				{
/* 				If the gazepoint was found in the previous field,				 */
					if (stRawGazepoint[ii-1].bGazeTracked == TRUE)
					{
/* 					Plot the connecting line from the previous to the			 */
/* 					present point. 														 */
						x0 = stRawGazepoint[ii-1].iXGazeWindowPix;
						y0 = stRawGazepoint[ii-1].iYGazeWindowPix;
						x1 = stRawGazepoint[ii].iXGazeWindowPix;
						y1 = stRawGazepoint[ii].iYGazeWindowPix;
						lct_line(x0, y0, x1, y1, GAZEPOINT_CONNECT_COLOR);
					}

/* 				Plot a red circle at the present gazepoint.						 */
					x0 = stRawGazepoint[ii].iXGazeWindowPix;
					y0 = stRawGazepoint[ii].iYGazeWindowPix;
					lct_circle(x0, y0, 2, RAW_GAZEPOINT_COLOR, 0);
				}

/* 			Erase the time history point for the tail of the snake.			 */
				lct_typedot(ROP_MODE_TRACE_ERASE);
				PlotRawTimePoint(ii, iTimePointColor);
				lct_typedot(ROP_MODE_TRACE_DRAW);
				PlotRawTimePoint(ii, iTimePointColor);
			}
		}

/*--------------------------------------------------------------------------*/

/* 	Pause to control the replay speed.												 */
		delay_msec(iReplayIntervalMs);

/* 	If the user pressed a key, 														 */
		if (iManualKeyPressed != 0)
		{
/* 		If the key is an escape, return. 											 */
			if (iManualKeyPressed == 27)
			{
/* 			Restart the Eyegaze image processing.									 */
				#ifndef NO_EYEGAZE
				  stEgControl.bTrackingActive = TRUE;
				#endif
				return;
			}

/* 		If the key is space, toggle the motion flag. 							 */
			if (iManualKeyPressed == VK_SPACE /*' '*/)
			{
				 if (motion == TRUE) motion = FALSE;
				 else 					motion = TRUE;
			}

/* 		If the key is a right arrow,													 */
			if (iManualKeyPressed == VK_RIGHT /*RIGHT_ARROW*/)
			{
/* 			If the direction is backward, 											 */
				if (direction == BACKWARD)
				{
/* 				Reverse the direction.													 */
					--i;
					direction = FORWARD;
				}
/* 			Put the trace in motion.													 */
				motion = TRUE;
			}

/* 		If the key is a left arrow,													 */
			if (iManualKeyPressed == VK_LEFT /*LEFT_ARROW*/)
			{
/* 			If the direction is forward,												 */
				if (direction == FORWARD)
				{
/* 				Reverse the direction.													 */
					++i;
					direction = BACKWARD;
				}
/* 			Put the trace in motion.													 */
				motion = TRUE;
			}
			iManualKeyPressed = 0;
		}
	}
}
/****************************************************************************/
void DrawFixationTrace(void)

/* This function superimposes the trace of the user's fixations on          */
/* the screen. 																				 */

{
	int	i; 								  /* fixation index							 */
	float sac_dur_ms; 					  /* duration of the connecting saccade */
												  /*	 (milliseconds)						 */
	float fix_dur_ms; 					  /* duration of the fixation 			 */
												  /*	 (milliseconds)						 */
	int	radius;							  /* radius of the fixation circle		 */
	int	x0, y0=0, x1, y1; 			  /* screen plot coordinates (pix)		 */
   int   iLineColor;                  /* color of the line connecting       */
												  /*	 fixations								 */
   UINT  uiLinePattern;                /* solid or dashed line pattern       */

	#define MIN_RADIUS		 2 		  /* minimum fixation circle radius 	 */
												  /*	 (pixels)								 */
	#define FIX_SIZE_SF		 2.0F 	  /* scale factor for fixation circle	 */
												  /*	 size (sq-pix / ms)					 */
	#define MAX_NORM_SAC_DUR 150.0F	  /* maximum normal duration of a		 */
												  /*	 saccade, used for plotting		 */
												  /*	 "continuous" or "discontinuous"  */
												  /*	 connecting lines between			 */
												  /*	 fixations								 */

/* If the trace is on screen we're erasing now, else we're drawing.         */
	if (bFixationTraceOnscreen == TRUE)
	{
		lct_typedot(ROP_MODE_TRACE_ERASE);
		bFixationTraceOnscreen = FALSE;
	}
	else
	{
		lct_typedot(ROP_MODE_TRACE_DRAW);
		bFixationTraceOnscreen = TRUE;
	}

/* Turn off screen updates while we're modifying the screen.  This way the  */
/* update will flash to the screen all at once when we're done.             */
	EyegazeScreenUpdatesActive(FALSE);

/* If at least one fixation was detected, 											 */
	if (iLastFixCollected >= 0)
	{
/* 	Set the location of the first fixation.										 */
      x0 = stFixPoint[0].iXFixPix;
      y0 = stFixPoint[0].iYFixPix;

/* 	Compute the duration of the fixation in milliseconds. 					 */
#ifdef NO_EYEGAZE
      fix_dur_ms = 1000.0F * stFixPoint[0].iFixDurCnt / 60;
#else
      fix_dur_ms = 1000.0F * stFixPoint[0].iFixDurCnt /
									  stEgControl.iSamplePerSec;
#endif

/* 	Set the radius of the fixation circle, so that the area is				 */
/* 	approximately proportional to the duration.									 */
		radius = nint((float)sqrt(FIX_SIZE_SF * fix_dur_ms / 3.14159));
		if (radius < MIN_RADIUS)  radius = MIN_RADIUS;

/* 	Plot concentric circles for the first fixation. 							 */
		lct_circle(x0, y0, 1, FIXATION_COLOR, 0);
		lct_circle(x0, y0, radius, FIXATION_COLOR, 0);
	}
	else
	{
		x0  = 20;
		y0 += 20;
      lct_fontsprt(LCT_FONT_3, _TEXT("No Fixation Data"),
						 x0, y0, 0, FIXATION_DISCONNECT_COLOR, 0);
	}

/* Loop over the remaining fixations.													 */
	for (i = 1; i <= iLastFixCollected; i++)
	{
/* 	Set the location of the fixation.												 */
      x1 = stFixPoint[i].iXFixPix;
      y1 = stFixPoint[i].iYFixPix;

/* 	Convert the time gap between fixations to milliseconds.					 */
#ifdef NO_EYEGAZE
      sac_dur_ms = 1000.0F * stFixPoint[i].iSaccadeDurCnt / 60;
#else
      sac_dur_ms = 1000.0F * stFixPoint[i].iSaccadeDurCnt /
									  stEgControl.iSamplePerSec;
#endif

/* 	If the saccade is short,															 */
		if (sac_dur_ms <= MAX_NORM_SAC_DUR)
		{
/* 		Plot a "continuous" fixation connecting line.                      */
         iLineColor   = FIXATION_CONNECT_COLOR;
         uiLinePattern = SOLID_LINE;
		}
/* 	Otherwise, if the gap between the fixations is too large,				 */
		else
		{
/* 		Plot a "discontinuous" fixation connecting line.                   */
         iLineColor   = FIXATION_DISCONNECT_COLOR;
         uiLinePattern = DASHED_LINE;
		}

/* 	Draw a connecting line from the past to the present fixation.			 */
      lct_dashed_line(x0, y0, x1, y1, iLineColor, uiLinePattern);

/* 	Convert the duration of the fixation to milliseconds. 					 */
#ifdef NO_EYEGAZE
      fix_dur_ms = 1000.0F * stFixPoint[i].iFixDurCnt / 60;
#else
      fix_dur_ms = 1000.0F * stFixPoint[i].iFixDurCnt /
									  stEgControl.iSamplePerSec;
#endif

/* 	Set the radius of the fixation circle, so that the area is				 */
/* 	approximately proportional to the duration.									 */
		radius = nint((float)sqrt(FIX_SIZE_SF * fix_dur_ms / 3.14159));
		if (radius < MIN_RADIUS)  radius = MIN_RADIUS;

/* 	Draw concentric circles for the present fixation.							 */
		lct_circle(x1, y1, 1, FIXATION_COLOR, 0);
		lct_circle(x1, y1, radius, FIXATION_COLOR, 0);

/* 	Shift the present fixation to the past one, for use with the next 	 */
/* 	fixation.																				 */
		x0 = x1;
		y0 = y1;
	}

	EyegazeScreenUpdatesActive(TRUE);
}
/****************************************************************************/
/****************************************************************************/
/* Utility Functions 																		 */
/****************************************************************************/
void delay_msec(int delta_t_ms)

/* This function waits until delta_t has passed since the last call to		 */
/* this function.  If this function has not been initialized, or if the 	 */
/* difference between the present and last times is greater than delta_t,	 */
/* the functions returns immediately.	The function also returns				 */
/* immediately if a key is pressed. 													 */

{
	#define	CLOCK_MS  55				 /* milliseconds in a computer system	 */
                                     /*   clock tick                        */

	int	 hour, min, sec, hsec;		 /* present system time values			 */
	long	 clock_time_ms;				 /* present system clock time of day	 */
												 /*	in ,milliseconds						 */
	static long  return_time_ms;		 /* time that the present interval ends */

/* Compute the return time to be one interval beyond the last sample time.  */
	return_time_ms += (long)delta_t_ms;

/* Loop until the designated time or a key is pressed.							 */
	for (EVER)
	{
/* 	Get the current system time.														 */
		lct_sysitime(&hour, &min, &sec, &hsec);

/* 	Compute the current time in milliseconds. 									 */
		clock_time_ms = (long)hour * 3600000L +
							 (long)min	*	 60000L +
							 (long)sec	*	  1000L +
							 (long)hsec *		 10L;

/* 	If the function is out of synch with real time, 							 */
		if ((return_time_ms < clock_time_ms - CLOCK_MS	 - 10) ||
			 (return_time_ms > clock_time_ms + delta_t_ms + 10))
		{
/* 		 Set the return time to the present clock time and return			 */
/* 		 immediately.																		 */
			 return_time_ms = clock_time_ms;
			 return;
		}

/* 	If the last sample time is greater than the present time, assume		 */
/* 	that the last sample time was not properly intitialized and return;	 */
/* 	or if the present time has reached the return time,						 */
		if ((clock_time_ms >= return_time_ms) || (iManualKeyPressed != 0))  return;
	}
	#undef  CLOCK_MS
}
/****************************************************************************/
void WriteTraceDataFile(_TCHAR *pchTraceDataFileName)

/* This function writes the trace data to the disk file. 						 */

{
	FILE *fp_trace_file; 			  /* trace data output file					 */
	time_t  lTime;
	struct  tm	tmToday;
   _TCHAR date_string[21];         /* string for today's date               */
   _TCHAR time_string[21];         /* string for time test was run          */
	int  i;								  /* general index								 */

/* Open the trace file for writing. 													 */
   _wfopen_s(&fp_trace_file, pchTraceDataFileName, _T("w"));
	if (fp_trace_file == NULL)
	{
      _tprintf(_TEXT("Error opening %s file for writing\n"), pchTraceDataFileName);
		return;
	}

/* Get the present date and time.														 */
	time(&lTime);
	localtime_s(&tmToday, &lTime);

   swprintf_s(date_string, 20,_TEXT("%02i/%02i/%04i"),
			  tmToday.tm_mon+1,
			  tmToday.tm_mday,
			  tmToday.tm_year+1900);
   swprintf_s(time_string, 20,_TEXT("%02i:%02i:%02i"),
			  tmToday.tm_hour,
			  tmToday.tm_min,
			  tmToday.tm_sec);

/* Write a header line for the file.													 */
   _ftprintf(fp_trace_file, _TEXT("Gazepoint Trace Data File,  %s  %s  %s\n"),
			  time_string, date_string, achUserName);

/* Record the scene type for which the trace was collected. 					 */
   _ftprintf(fp_trace_file, _TEXT("Scene Type: "));
   if (iSceneType == TEXT_SCENE)   _ftprintf(fp_trace_file, _TEXT("text %i %i %s"),
														 iScreenWidthPix, iScreenHeightPix, achTextFileName);
   if (iSceneType == BITMAP_SCENE) _ftprintf(fp_trace_file, _TEXT("bitmap %i %i %s"),
														 iScreenWidthPix, iScreenHeightPix, achBitmapFileName);
   _ftprintf(fp_trace_file, _TEXT("\n"));

/* - - - - - - - - - - - Write Raw Gazepoint Trace - - - - - - - - - - - - -*/

/* Write the header for the raw gazepoint trace.									 */
#ifndef NO_EYEGAZE
   _ftprintf(fp_trace_file, _TEXT("Raw Gazepoint Data (%i Hz Sampling Rate):\n\n"),
           stEgControl.iSamplePerSec * iNVisionSystems);
#endif

/* Write the header data for the columns. 											 */
   _ftprintf(fp_trace_file,
      _TEXT("samp  Eye     Gazepoint  Pupil   Eyeball-Position  Focus   Fix\n"));
   _ftprintf(fp_trace_file,
      _TEXT("indx Found    X      Y    Diam     X     Y     Z   Range  Indx\n"));
   _ftprintf(fp_trace_file,
      _TEXT("     (t/f)  (pix)  (pix)  (mm)   (mm)  (mm)  (mm)   (mm)      \n\n"));

/* Print the raw data for the gazepoint sample. 									 */
	for (i = 0; i <= iLastSampleCollected; i++)
	{
      _ftprintf(fp_trace_file, _TEXT("%3i %5i %6i %6i %6.2f %6.1f %5.1f %5.1f %7.1f %4i\n"),
			i,
			stRawGazepoint[i].bGazeTracked,
			stRawGazepoint[i].iXGazeWindowPix,
			stRawGazepoint[i].iYGazeWindowPix,
         stRawGazepoint[i].fPupilDiamMm,
         stRawGazepoint[i].fXEyeballMm,
         stRawGazepoint[i].fYEyeballMm,
         stRawGazepoint[i].fFocusOffsetMm,
         stRawGazepoint[i].fFocusRangeMm,
         stRawGazepoint[i].iFixIndex);
	}

/* - - - - - - - - - - - - Write Fixation Trace - - - - - - - - - - - - - - */

/* Write the header for the raw gazepoint trace.									 */
#ifndef NO_EYEGAZE
   _ftprintf(fp_trace_file, _TEXT("\nFixation Data: (%i Hz Sampling Rate)\n\n"),
           stEgControl.iSamplePerSec * iNVisionSystems);
#endif

/* Write the header data for the columns. 											 */
   _ftprintf(fp_trace_file,
      _TEXT(" fix    Fixation     Sac   Fix   Fix \n"));
   _ftprintf(fp_trace_file,
      _TEXT("indx    X      Y     Dur   Dur  Start\n"));
   _ftprintf(fp_trace_file,
      _TEXT("      (pix)  (pix)  (cnt) (cnt)  Samp\n\n"));

/* Print the fixation data points.														 */
	for (i = 0; i <= iLastFixCollected; i++)
	{
      _ftprintf(fp_trace_file, _TEXT("%3i %6i %6i %5i %6i %6i\n"),
			i,
         stFixPoint[i].iXFixPix,
         stFixPoint[i].iYFixPix,
         stFixPoint[i].iSaccadeDurCnt,
         stFixPoint[i].iFixDurCnt,
         stFixPoint[i].iFixStartSamp);
	}

/*- - - - - - - - - - - - - - Close Trace File - - - - - - - - - - - - - - -*/

/* Close the trace data file. 															 */
	fclose(fp_trace_file);
}
/****************************************************************************/
void ReadTraceDataFile(_TCHAR *pchTraceDataFileName)

/* This function reads the trace data from the trace.dat disk file.			 */

{
	FILE *fp_trace_file; 		  /* trace data output file						 */
   _TCHAR file_line_string[MAX_FILE_NAME_LEN+1];
                                /* line of text from the trace data file    */
	int  i;							  /* program index for gazepoint or fixaton	 */
	int  i_file_index;			  /* trace.dat file's index for gazepoint or  */
										  /*	 fixation										 */
	int  n;							  /* number of data items extracted from		 */
										  /*	 the line of file data						 */
	BOOL read_error = FALSE;	  /* flag indicating whether an error was 	 */
										  /*	 encountered reading the trace.dat file */
   _TCHAR *pch;
	int  iRecordedScreenWidthPix, iRecordedScreenHeightPix;
										  /* The screen dimensions in effect during	 */
										  /*	 recording. 									 */

/* Open the trace file for reading. 													 */
   _wfopen_s(&fp_trace_file, pchTraceDataFileName, _T("r"));
	if (fp_trace_file == NULL)
	{
//      MessageBox( hwndEyegaze, _TEXT("Couldn't open the Trace Data file"), pchTraceDataFileName, MB_OK );
		return;
	}

/* Read past the first header line in the file. 									 */
   _fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file);

/* Read the scene type for which the trace was collected.						 */
   _fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file);
	iSceneType = NO_SCENE;
   if ((pch = _tcsstr(file_line_string, _TEXT("text"))) != NULL)
	{
		iSceneType = TEXT_SCENE;
      swscanf_s(pch+5, _TEXT("%i %i %s"),
         &iRecordedScreenWidthPix,
         &iRecordedScreenHeightPix,
         achTextFileName,
         MAX_FILE_NAME_LEN);
//      wcscanf_s(&file_line_string[16], MAX_FILE_NAME_LEN-16, _TEXT("%s"),
//         achTextFileName, MAX_FILE_NAME_LEN);
		pszDisplayedFile = achTextFileName;
	}
   else if ((pch = _tcsstr(file_line_string, _TEXT("bitmap"))) != NULL)
	{
		iSceneType = BITMAP_SCENE;
/* 	The bitmap file name starts seven characters after the beginning of	 */
/* 	the word 'bitmap'.                                                    */
      _stscanf_s(pch+7, _TEXT("%i %i %s"),
         &iRecordedScreenWidthPix,
         &iRecordedScreenHeightPix,
         achBitmapFileName,
         MAX_FILE_NAME_LEN);
      wcscpy_s(achDimBitmapFileName, MAX_FILE_NAME_LEN, achBitmapFileName);
      wcscat_s(achDimBitmapFileName, MAX_FILE_NAME_LEN, _TEXT(".dim"));
		pszDisplayedFile = achBitmapFileName;
	}
	else
	{
     MessageBox( hwndEyegaze, _TEXT("File doesn't say Text or Bitmap"),
        _TEXT("Error - Bad Format"), MB_OK );
	  fclose(fp_trace_file);
	  return;
	}

/* Select the appropriate menu item based on what's in this file.           */
	{
		HMENU hMenu;
		hMenu = GetMenu(hwndEyegaze);
		CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,		MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_UNCHECKED);
		if (iSceneType == TEXT_SCENE)   CheckMenuItem(hMenu, ID_TRACKGAZE_READTEXT,	  MF_CHECKED);
		if (iSceneType == BITMAP_SCENE) CheckMenuItem(hMenu, ID_TRACKGAZE_LOOKATBITMAP, MF_CHECKED);
	}

/* - - - - - - - - - - - Read Raw Gazepoint Trace - - - - - - - - - - - - - */

/* Read past the next six header and blank lines at the beginning of the	 */
/* file. 																						 */
	for (i = 0; i < 6; i++)
	{
      _fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file);
	}

/* Loop through all the raw gazepoint data lines.									 */
	for (i = 0; i < MAX_GAZEPOINTS; i++)
	{
/* 	Read the file line.																	 */
      _fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file);

/* 	If the line is empty, break out of the fixation loop. 					 */
      if (_tcslen(file_line_string) < 3)  break;

/* 	Get the gazepoint data out of the file line string.						 */
      n = swscanf_s(file_line_string, _TEXT("%i%i%i%i%f%f%f%f%f%i"),
			&i_file_index,
			&stRawGazepoint[i].bGazeTracked,
			&stRawGazepoint[i].iXGazeWindowPix,
			&stRawGazepoint[i].iYGazeWindowPix,
         &stRawGazepoint[i].fPupilDiamMm,
         &stRawGazepoint[i].fXEyeballMm,
         &stRawGazepoint[i].fYEyeballMm,
         &stRawGazepoint[i].fFocusOffsetMm,
         &stRawGazepoint[i].fFocusRangeMm,
         &stRawGazepoint[i].iFixIndex);

/* 	If we did not get the expected ten pieces of data from the line,		 */
		if (n != 10)
		{
         _tprintf(_TEXT("Error in ReadTraceDataFile(): ")
                _TEXT("Number of gazepoint data on file line %i != 10\n"), n);
			read_error = TRUE;
		}

/* 	If the gazepoint index on the file line does not match the program	 */
/* 	index,																					 */
		if (i_file_index != i)
		{
         _tprintf(_TEXT("Error in ReadTraceDataFile(): ")
                _TEXT("Fix index %i != program index %i\n"), i_file_index, i);
			read_error = TRUE;
		}
	}

/* Set the total number of gazepoint data collected.								 */
	iLastSampleCollected = i-1;

/*- - - - - - - - - - - - - Read Fixation Trace - - - - - - - - - - - - - - */

/* Read past the six header and blank lines at the beginning of the			 */
/* fixation data. 																			 */
	for (i = 0; i < 6; i++)
	{
      _fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file);
	}

/* Loop through all the fixation data lines. 										 */
	for (i = 0; i < MAX_FIXATIONS; i++)
	{
/* 	Read the file line.																	 */
      if (_fgetts(file_line_string, MAX_FILE_NAME_LEN, fp_trace_file) == NULL) break;

/* 	If the line is empty, break out of the fixation loop. 					 */
      if (_tcsclen(file_line_string) < 3)  break;

/* 	Get the gazepoint data out of the file line string.						 */
      n = swscanf_s(file_line_string, _TEXT("%i%i%i%i%i%i"),
			&i_file_index,
         &stFixPoint[i].iXFixPix,
         &stFixPoint[i].iYFixPix,
         &stFixPoint[i].iSaccadeDurCnt,
         &stFixPoint[i].iFixDurCnt,
         &stFixPoint[i].iFixStartSamp);

/* 	If we did not get the expected six pieces of data from the file line, */
		if (n != 6)
		{
         _tprintf(_TEXT("Error in ReadTraceDataFile(): ")
                _TEXT("Number of fixation data on file line %i != 6\n"), n);
			read_error = TRUE;
		}

/* 	If the fix index on the file line does not match the program index,	 */
		if (i_file_index != i)
		{
         _tprintf(_TEXT("Error in ReadTraceDataFile(): ")
                _TEXT("Fix index %i != program index %i\n"), i_file_index, i);
			read_error = TRUE;
		}
	}

/* Set the total number of fixations collected. 									 */
	iLastFixCollected = i-1;

/*- - - - - - - - - - - - - - Close Trace File - - - - - - - - - - - - - - -*/

/* Close the trace data file. 															 */
	fclose(fp_trace_file);

/* If there was no read error,															 */
	if (read_error == FALSE)
	{
/* 	Find the min and max pupil diameters.											 */

/* 	Declare that some data is present for display.								 */
		bDataCollectedFlag = TRUE;
	}
/* Otherwise, if there was an error reading the file, 							 */
	else
	{
/* 	Declare that no data is present for display. 								 */
		bDataCollectedFlag = FALSE;

/* 	Tell the user. 																		 */
      _tprintf(_TEXT("iLastSampleCollected %i   iLastFixCollected %i\n"),
				          iLastSampleCollected, 	   iLastFixCollected);
	}
}
/****************************************************************************/
DWORD WINAPI CallCalibrate(void *pVoid)
{
#ifndef NO_EYEGAZE
   bDisplayEyeImages = FALSE; // don't allow the display thread to run while calibrating
	lct_typedot(ROP_MODE_NORMAL);  // switch to 'normal' graphics mode.
	EgCalibrate1(&stEgControl, hwndEyegaze, EG_CALIBRATE_NONDISABILITY_APP);
	lct_typedot(ROP_MODE_TRACE_DRAW);  // Switch back into Trace Drawing mode.
   bDisplayEyeImages = TRUE;

/* Clear the screen. 																		 */
	TraceClearScreen();

	EnableMenuItem(hMenu, ID_TRACKGAZE_CALIBRATE,				 MF_ENABLED);
	EnableMenuItem(hMenu, ID_COLLECTDATA_START,					 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYCOMPLETERAWTRACE, MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_DISPLAYMOVINGTRACE,		 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLERAWTRACE,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLEFIXATIONTRACE, 	 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLETIMEPLOT,			 MF_ENABLED);
	EnableMenuItem(hMenu, ID_DISPLAY_TOGGLESCREENDISPLAY, 	 MF_ENABLED);

	SendMessage(hwndEyegaze, WM_COMMAND, ID_CALIBRATE_COMPLETE, 0);
#endif
	return 0;
}
/****************************************************************************/
BOOL CALLBACK SetReplaySpeedDialogProc(HWND hDlg, UINT message,
													WPARAM wParam, LPARAM lParam)
{
   _TCHAR achText[41];

	switch (message)
	{
		case WM_INITDIALOG:
/* 		Set the current collection time value into the entry field. 		 */
         swprintf_s(achText, 40, _TEXT("%.1f"), fReplaySpeed);
			SetDlgItemText(hDlg, IDC_EDIT1, achText);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(hDlg, IDC_EDIT1, achText, sizeof(achText));
               swscanf_s(achText, _TEXT("%f"), &fReplaySpeed);
#ifdef NO_EYEGAZE
					iReplayIntervalMs = nint(1000.0F / (60 * fReplaySpeed));
#else
					iReplayIntervalMs = nint(1000.0F / (stEgControl.iSamplePerSec * fReplaySpeed));
#endif
					EndDialog(hDlg, 0);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;

}
/****************************************************************************/
BOOL CALLBACK GetUserNameDialogProc(HWND hDlg, UINT message,
												WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
/* 		Set the current user name into the entry field. 						 */
			SetDlgItemText(hDlg, IDC_EDIT1, achUserName);
			SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(hDlg, IDC_EDIT1, achUserName, sizeof(achUserName));
					EndDialog(hDlg, 0);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;

}
/****************************************************************************/
BOOL CALLBACK SetCollectionTimeDialogProc(HWND hDlg, UINT message,
														WPARAM wParam, LPARAM lParam)
{
   _TCHAR achText[41];

	switch (message)
	{
		case WM_INITDIALOG:
/* 		Set the current collection time value into the entry field. 		 */
         swprintf_s(achText, 40, _TEXT("%.1f"), fCollectionTimeSeconds);
			SetDlgItemText(hDlg, IDC_EDIT1, achText);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
/* 				Read out the value from the entry field and store it. 		 */
					GetDlgItemText(hDlg, IDC_EDIT1, achText, sizeof(achText));
               swscanf_s(achText, _TEXT("%f"), &fCollectionTimeSeconds);

#ifdef NO_EYEGAZE
					fCollectionTimeSeconds = 0.0f;
					iSamplesToCollect = 0;
#else

					iSamplesToCollect = (int)(fCollectionTimeSeconds * stEgControl.iSamplePerSec);
/* 				If the data arrays can't accommodate the desired collection time,        */
					if (iSamplesToCollect > MAX_GAZEPOINTS)
					{
/* 					Limit the number of gazepoint data samples that can be	 */
/* 					collected, and reset the collection time appropriately.	 */
						iSamplesToCollect = MAX_GAZEPOINTS;
						fCollectionTimeSeconds = (float)iSamplesToCollect /
														  stEgControl.iSamplePerSec;
					}
#endif

					EndDialog(hDlg, 0);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;

}
/****************************************************************************/
void SelectText(HWND hwnd)
{
	OPENFILENAME ofn; 		// common dialog box structure
   static _TCHAR szFile[260];       // buffer for file name

// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize	  = sizeof(OPENFILENAME);
	ofn.hwndOwner		  = hwnd;
	ofn.lpstrFile		  = szFile;
	ofn.nMaxFile		  = sizeof(szFile);
   ofn.lpstrFilter     = _TEXT("All\0*.*\0Text\0*.TXT\0\0");
	ofn.nFilterIndex	  = 2;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle      = _TEXT("Select Text for Trace");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

// Display the Open dialog box.
	if (GetOpenFileName(&ofn)==TRUE)
	{
      wcscpy_s(achTextFileName, MAX_FILE_NAME_LEN, ofn.lpstrFile);
		pszDisplayedFile = achTextFileName;
	}

/* Put the current directory back to the startup directory. 					 */
	SetCurrentDirectory(achCurrentDirectory);
}
/****************************************************************************/
void SelectBitmapLCT(HWND hwnd)
{
	OPENFILENAME ofn; 		// common dialog box structure
   static _TCHAR szFile[260];       // buffer for file name

// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize	  = sizeof(OPENFILENAME);
	ofn.hwndOwner		  = hwnd;
	ofn.lpstrFile		  = szFile;
	ofn.nMaxFile		  = sizeof(szFile);
   ofn.lpstrFilter     = _TEXT("All\0*.*\0Bitmaps\0*.BMP\0\0");
	ofn.nFilterIndex	  = 2;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle      = _TEXT("Select Bitmap for Trace");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

// Display the Open dialog box.
	if (GetOpenFileName(&ofn)==TRUE)
	{
      wcscpy_s(achBitmapFileName,    MAX_FILE_NAME_LEN, ofn.lpstrFile);
      wcscpy_s(achDimBitmapFileName, MAX_FILE_NAME_LEN, achBitmapFileName);
      wcscat_s(achDimBitmapFileName, MAX_FILE_NAME_LEN, _TEXT(".dim"));
		pszDisplayedFile = achBitmapFileName;
	}

/* Put the current directory back to the startup directory. 					 */
	SetCurrentDirectory(achCurrentDirectory);
}
/****************************************************************************/
BOOL BitmapSaveNameLCT( HWND hwnd, _TCHAR *pFileName )
{
	OPENFILENAME ofn; 		// common dialog box structure
	BOOL bRC;

	// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof( OPENFILENAME ));
	ofn.lStructSize	  = sizeof( OPENFILENAME );
	ofn.hwndOwner		  = hwnd;
	ofn.lpstrFile		  = pFileName;
	ofn.nMaxFile		  = 260;
   ofn.lpstrFilter     = _TEXT("All\0*.*\0Bitmaps\0*.BMP\0\0");
	ofn.nFilterIndex	  = 2;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle      = _TEXT("Save Bitmap to...");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	// Display the Open dialog box.
	bRC = GetSaveFileName( &ofn );

/* Put the current directory back to the startup directory. 					 */
	SetCurrentDirectory(achCurrentDirectory);

	return bRC;
}
/****************************************************************************/
BOOL ReplayFileNameLCT( HWND hwnd, _TCHAR *pFileName )
{
	OPENFILENAME ofn; 		// common dialog box structure
	BOOL bRC;

	// Initialize OPENFILENAME
	ZeroMemory( &ofn, sizeof( OPENFILENAME ));
	ofn.lStructSize	  = sizeof( OPENFILENAME );
	ofn.hwndOwner		  = hwnd;
	ofn.lpstrFile		  = pFileName;
	ofn.nMaxFile		  = 260;
   ofn.lpstrFilter     = _TEXT("All\0*.*\0Data Files\0*.DAT\0\0");
	ofn.nFilterIndex	  = 2;
	ofn.lpstrFileTitle  = NULL;
	ofn.nMaxFileTitle   = 0;
	ofn.lpstrInitialDir = NULL;
   ofn.lpstrTitle      = _TEXT("Save Bitmap to...");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	// Display the Open dialog box.
	bRC = GetOpenFileName( &ofn );

/* Put the current directory back to the startup directory. 					 */
	SetCurrentDirectory(achCurrentDirectory);

	return bRC;
}
/****************************************************************************/
void TraceClearScreen(void)
{
	lct_clearscreen(BRIGHT_WHITE);

/* Clear the booleans since we're about to replace the screen.              */
	bRawTraceOnscreen 	  = FALSE;
	bTimePlotOnscreen 	  = FALSE;
	bFixationTraceOnscreen = FALSE;
	bBitmapOnscreen		  = FALSE;

}
/****************************************************************************/
void UpdateTitleBarText(BOOL bAutoSaveActive, _TCHAR *pszUserName, _TCHAR *pszFileDisplayed,
                        _TCHAR *pszDataFileName)
{
   _TCHAR achTitleBarText[261];

   swprintf_s(achTitleBarText, 260, _TEXT("Eyegaze Trace Demonstration - LC Technologies, Inc.  -- "));

	if (bAutoSaveActive == TRUE)
	{
      wcscat_s(achTitleBarText, 260, _TEXT("Auto Save Active   "));
	}
	else
	{
      wcscat_s(achTitleBarText, 260, _TEXT("Auto Save Inactive   "));
	}

	if (pszUserName != NULL)
	{
      wcscat_s(achTitleBarText, 260, pszUserName);
      wcscat_s(achTitleBarText, 260, _TEXT("    "));
	}

	if (pszFileDisplayed != NULL)
	{
      wcscat_s(achTitleBarText, 260, pszFileDisplayed);
      wcscat_s(achTitleBarText, 260, _TEXT("    "));
	}

	if (pszDataFileName != NULL)
	{
      wcscat_s(achTitleBarText, 260, pszDataFileName);
	}

	SetWindowText(hwndEyegaze, achTitleBarText);
}
/****************************************************************************/
DWORD WINAPI DisplayEyeImages(void *pVoid)
{
/* This thread exists simply to keep the eye images updated when the data   */
/* collection thread is not running.                                        */
   for (;;)
   {
/*    Sit here and do nothing while the bDisplayEyeImages flag is false.    */
      while (bDisplayEyeImages == FALSE) Sleep(250);

		EgGetData(&stEgControl);

		if (bDisplayState == TRUE)
		{
			EgEyeImageDisplay(0,
									stEgControl.iScreenWidthPix - stEyeImageInfo.iWidth,
									0,
									stEyeImageInfo.iWidth,
									stEyeImageInfo.iHeight,
									hdc);

         if (iNVisionSystems == 2)
         {
         EgEyeImageDisplay(1,
                           0,
                           0,
                           stEyeImageInfo.iWidth,
                           stEyeImageInfo.iHeight,
                           hdc);
         }
		}

   }
}
/****************************************************************************/
