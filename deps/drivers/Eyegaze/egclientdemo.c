/****************************************************************************

File Name:       EgClientDemo.c
Program Name:    Demonstrate Client access the Eyegaze System

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

Date Created:    08/27/90
                 02/24/00 converted to Windows program

*****************************************************************************

   This EgClientDemo program demonstrates client computer control of a
peripheral Eyegaze System.  It contains procedures to a) support the
Eyegaze System in performing a calibration for a person looking at the
client computer monitor, and b) receive and process gazepoint data from
the Eyegaze System in real time.  This EgClientDemo program is intended
to be used as the starting point for the development of custom,
client-computer Eyegaze applications.  The code is compatible with
Windows-based computers.  If the client computer is not running Windows,
the developer must first modify the communications portions of this code
to be compatible with the client computer operating system.

   The EgClientDemo program communicates with the Eyegaze System via the
EgServer program which runs on the Eyegaze computer.  See the comments
in the EgServer program for a list of the commands to which the Eyegaze
System responds.

   The EgClientDemo program allows the user to control the Eyegaze
System manually by selecting menu items on the client computer that
correspond to the Eyegaze System's EgServer commands.  Upon receiving a
command, EgClientDemo issues the appropriate command to the Eyegaze
System in accordance with the EgServer protocol.  To demonstrate using
eyegaze data in real time during an eyegaze data collection and
processing phase, EgClientDemo displays the gazepoint on the client
computer's monitor.  To terminate the EgClientDemo program, select File,
Exit on the client computer keyboard.

*****************************************************************************/
/* REQUIRED FUNCTION, VARIABLE AND CONSTANT DEFINITIONS                     */

#include <windows.h>
#undef DELETE
#include <stdio.h>
#include <string.h>      /* string manipulation functions                   */
#include <stdlib.h>      /* C run-time functions                            */
#include <conio.h>       /* console I/O functions                           */
#include <process.h>     /* computer process control functions              */
#include <lctsupt.h>     /* Eyegaze support functions                       */
#include <lctcolor.h>    /* color constants for ESG calls                   */
#include <lctfont.h>     /* Eyegaze font number definitions                 */
#include <egwin.h>       /* Eyegaze functions and variables                 */
#include <clientimages.h>
#include <lctwin.h>
#include <lctascii.h>
#include "EgClientDemo.h"
#include <gazevergence.h>

/****************************************************************************/
/* FUNCTION DEFINITIONS:                                                    */

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT uiMessage,
                            WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OpenFileDialogProc(HWND hDlg, UINT message,
                                 WPARAM wParam, LPARAM lParam);
DWORD WINAPI ProcessGazepointData(void *hfEyegazePort);
void  PlotGazepoint(int iIPix, int iJPix, int gazepoint_found);
DWORD WINAPI InitEgClientDemo(void *pv);
DWORD WINAPI SupportEyegazeCalibration(void *pVoid);

/****************************************************************************/
/* PROGRAM CONSTANTS                                                        */

#define  EVER            ;;      /* Used in infinite for(EVER) loops        */

#define  HORZ_MARK_SIZE  10      /* Size of the gazepoint screen marker     */
                                 /*   on the computer monitor (pixels)      */
#define  VERT_MARK_SIZE  10      /* Size of the gazepoint screen marker     */
                                 /*   on the computer monitor (pixels)      */

/****************************************************************************/
/* GLOBAL VARIABLES:                                                        */

SINT   iIGaze, iJGaze;           /* received eyegaze coordinates (pixels)   */
float  fPupilRadiusMm;           /* received pupil radius (MM)              */
SINT   iDeltaCameraFieldCount;   /* number of camera fields that have       */
                                 /*   occurred since the last report.       */
                                 /*   (This number is typically 2 when the  */
                                 /*   Eyegaze System is operating at 30Hz   */
                                 /*   and is 1 when operating at 60Hz.      */
                                 /*   Camera fields may be skipped when     */
                                 /*   the image processing takes more than  */
                                 /*   camera field period to process a      */
                                 /*   field.)                               */
char  achFileName[260];

/****************************************************************************/

HWND hwndEyegaze;
_TCHAR szAppName[] = TEXT("EgClientDemo");

HDC      memdc;                 /* Stores the virtual device handle         */
HBITMAP  hbit;                  /* Stores the virtual bitmap                */
HBRUSH   hbrush;                /* Stores the brush handle                  */

HDC      hdc;
HPALETTE hpal;
UINT     cxClient, cyClient;
int      hvb;
RECT     stWindowRect;
RECT     stEyegazeRect;
int      iWindowWidthPix;
int      iWindowHeightPix;
int      iScreenWidth;
int      iScreenHeight;
int      iWindowHorzOffset;
int      iWindowVertOffset;
BOOL     bStopCommunications;
_TCHAR   achCommAddress[21];
void     *hfEyegazePort;         /* handle of the serial communications     */
                                 /*   port to the Eyegaze System            */
BOOL     bCommEstablished;
#define BUFFER_LEN 1
struct _stEgControl stEgControl;
_TCHAR  achCmdLine[260];
struct _stGazeVergence stGazeVergence;

/***************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpszCmdLine,
                   int       nWinMode)
{
   HWND       hwnd;
   MSG        msg;
   WNDCLASSEX wndclass;
   HANDLE     hAccel;  // handle to the keyboard accelerators

/* Initialize window data, register window class and create main window.    */
   if (!hPrevInstance)
   {
      wndclass.cbSize        = sizeof(WNDCLASSEX);
      wndclass.style         = 0;
      wndclass.lpfnWndProc   = WindowFunc;
      wndclass.cbClsExtra    = 0;
      wndclass.cbWndExtra    = 0;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
      wndclass.hIconSm       = LoadIcon(NULL,IDI_APPLICATION);
      wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
      wndclass.hbrBackground = GetStockObject(BLACK_BRUSH);
      wndclass.lpszMenuName  = szAppName;
      wndclass.lpszClassName = szAppName;

/*    Register the class                                                    */
      if (!RegisterClassEx(&wndclass)) return FALSE;
   }

/* Get screen coordinates.                                                  */
   iScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
   iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

/* Save the command line to extract the server IP address or com port name. */
   #if defined UNICODE
      lctAsciiToWideChar(lpszCmdLine, achCmdLine);
   #else
      wcscpy_s(achCmdLine, 260, (_TCHAR *)lpszCmdLine);
   #endif

   hwnd = CreateWindow(szAppName, // window class name
      TEXT("EgClientDemo - Eyegaze System - LC Technologies, Inc."), // window caption
      WS_OVERLAPPEDWINDOW,        // window style
      0,                          // initial x position
      0,                          // initial y position
      iScreenWidth,               // initial x size
      iScreenHeight,              // initial y size
      NULL,                       // parent window handle
      NULL,                       // window menu handle
      hInstance,                  // program instance handle
      NULL);                      // creation parameters

/* Display the window                                                       */
   ShowWindow(hwnd, nWinMode);
   UpdateWindow(hwnd);

   hAccel = LoadAccelerators(hInstance, TEXT ("EGCLIENTDEMO"));

/* Get and dispatch messages until a WM_QUIT message is received.           */
   while (GetMessage(&msg, NULL, 0, 0))
   {
      if (!TranslateAccelerator(hwnd, hAccel, &msg))
      {
         TranslateMessage(&msg);   /* Translates virtual key codes          */
         DispatchMessage(&msg);    /* Dispatches message to window          */
      }
   }

   return msg.wParam;           /* Returns the value from PostQuitMessage   */
}
/****************************************************************************/
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT uiMessage,
                            WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT ps;
   long        Tid;
   HMENU       hMenu;
   static HINSTANCE hInstance;
   POINT       point;

   switch (uiMessage)
   {
      case WM_CREATE:
/*       Save the instance handle for the dialog box calls below.           */
         hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

/*       Determine the upper-left corner of the client area in screen       */
/*       coordinates.                                                       */
         point.x = 0;
         point.y = 0;
         ClientToScreen(hwnd,&point);
         GetClientRect(hwnd, &stWindowRect);

         iWindowWidthPix = stWindowRect.right - stWindowRect.left;
         iWindowHeightPix = stWindowRect.bottom - stWindowRect.top;
         iWindowHorzOffset = point.x;
         iWindowVertOffset = point.y;

/*       Create the virtual window.                                         */
         hdc    = GetDC(hwnd);
         memdc  = CreateCompatibleDC(hdc);
         hbit   = CreateCompatibleBitmap(hdc, iScreenWidth, iScreenHeight);
         SelectObject(memdc, hbit);
         hbrush = GetStockObject(BLACK_BRUSH);
         SelectObject(memdc, hbrush);
         PatBlt(memdc, 0, 0, iScreenWidth, iScreenHeight, PATCOPY);

         hwndEyegaze = hwnd;
         lctSetWindowHandle(hwnd, memdc, hdc, szAppName, wcslen(szAppName));

/*       Assume loopback connection.                                        */
         wcscpy_s(achCommAddress, 20, TEXT("127.0.0.1"));
         if (_tcslen(achCmdLine) > 0)
            wcscpy_s(achCommAddress, 20, achCmdLine);

/*       Before calling InitEgClientDemo, set stEgControl.hEyegaze to a     */
/*       NULL so the functions that need it won't use it until it's been    */
/*       initialized.                                                       */
         stEgControl.hEyegaze = NULL;

         CreateThread(NULL, 0, InitEgClientDemo, 0, 0, &Tid);
         break;;

      case WM_PAINT:
         BeginPaint(hwnd, &ps);
         BitBlt(hdc,
               // ps.rcPaint.left + iWindowHorzOffset, ps.rcPaint.top + iWindowVertOffset,
                ps.rcPaint.left, ps.rcPaint.top,
                ps.rcPaint.right  - ps.rcPaint.left,
                ps.rcPaint.bottom - ps.rcPaint.top,
                memdc,
                ps.rcPaint.left, ps.rcPaint.top,
                SRCCOPY);
         EndPaint(hwnd, &ps);
         break;

/*    If the user sizes or moves the window, obtain the new size and        */
/*    offsets from screen origin.                                           */
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

      case WM_COMMAND:
         hMenu = GetMenu(hwnd);
         switch(LOWORD (wParam))
         {
            case ID_FILE_EXIT:
               bStopCommunications = TRUE;
               SendMessage(hwnd, WM_CLOSE, 0, 0);
/*             Close the eyegaze communications.                            */
               EgExit(&stEgControl);
               SendMessage(hwnd, WM_CLOSE, 0, 0);
               break;

            case ID_COMMUNICATIONS_CALIBRATE:
               bStopCommunications = FALSE;
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_GRAYED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_GRAYED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_END,          MF_ENABLED);
/*             Perform Eyegaze 'C'alibration on the client monitor.         */
               CreateThread(NULL, 0, SupportEyegazeCalibration, 0, 0, &Tid);

               break;

            case ID_COMMUNICATIONS_BEGIN:
               bStopCommunications = FALSE;
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_GRAYED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_GRAYED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_END,          MF_ENABLED);
               CreateThread(NULL, 0, ProcessGazepointData, hfEyegazePort, 0, &Tid);
               break;

            case ID_COMMUNICATIONS_END:
               bStopCommunications = TRUE;
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_ENABLED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_ENABLED);
               EnableMenuItem(hMenu, ID_COMMUNICATIONS_END,          MF_GRAYED);
               break;

            case ID_DATALOG_OPENFILE:
/*             'O'pen a file and receive file header data.                  */
               if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_OPENFILE),
                         hwnd, OpenFileDialogProc) == TRUE)
               {
                  EnableMenuItem(hMenu, ID_DATALOG_OPENFILE,            MF_GRAYED);
                  EnableMenuItem(hMenu, ID_DATALOG_BEGINDATACOLLECTION, MF_ENABLED);
                  EnableMenuItem(hMenu, ID_DATALOG_MARKEVENT,           MF_ENABLED);
                  EnableMenuItem(hMenu, ID_DATALOG_CLOSEFILE,           MF_ENABLED);

                  EgLogFileOpen(&stEgControl, achFileName, "a");

/*                Write the log file column headers to the file.            */
                  EgLogWriteColumnHeader(&stEgControl);
               }
               break;

            case ID_DATALOG_BEGINDATACOLLECTION:
               EnableMenuItem(hMenu, ID_DATALOG_ENDDATACOLLECTION,   MF_ENABLED);
               EnableMenuItem(hMenu, ID_DATALOG_BEGINDATACOLLECTION, MF_GRAYED);

/*             Start storing Eyegaze data to file.                          */
               EgLogStart(&stEgControl);
               break;

            case ID_DATALOG_MARKEVENT:
               EgLogMark(&stEgControl);
               break;

            case ID_DATALOG_ENDDATACOLLECTION:
/*             Stop storing Eyegaze data to file.                           */
               EgLogStop(&stEgControl);

               EnableMenuItem(hMenu, ID_DATALOG_ENDDATACOLLECTION,   MF_GRAYED);
               EnableMenuItem(hMenu, ID_DATALOG_BEGINDATACOLLECTION, MF_ENABLED);

               break;

            case ID_DATALOG_CLOSEFILE:
/*             Close the file.                                              */
               EgLogFileClose(&stEgControl);

               EnableMenuItem(hMenu, ID_DATALOG_OPENFILE,            MF_ENABLED);
               EnableMenuItem(hMenu, ID_DATALOG_BEGINDATACOLLECTION, MF_GRAYED);
               EnableMenuItem(hMenu, ID_DATALOG_MARKEVENT,           MF_GRAYED);
               EnableMenuItem(hMenu, ID_DATALOG_CLOSEFILE,           MF_GRAYED);

               break;

            case ID_HELP_ABOUT:
               MessageBox(hwnd,TEXT ("EgClientDemo Demonstration Program\n")
                               TEXT ("(c) LC Technologies, 2003-2010 "),
                          szAppName, MB_ICONINFORMATION | MB_OK);
               break;
         }
         break;

      case ID_CALIBRATION_ENDED:
         hMenu = GetMenu(hwnd);
         EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_ENABLED);
         EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_ENABLED);
         EnableMenuItem(hMenu, ID_COMMUNICATIONS_END,          MF_GRAYED);
         break;

      case WM_DESTROY:
/*       Close the eyegaze communications port.                             */
//         EgExit(&stEgControl);

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
DWORD WINAPI InitEgClientDemo(void *pv)
{
/* This is a separate thread that initializes the Eyegaze functions and a   */
/* few support functions for fonts and terminates.  The thread should live  */
/* for just a few seconds.                                                  */

   HMENU hMenu;
   int   rc;
   _TCHAR achText[260];

/* Set up the structures to pass to EgInit().                               */
   stEgControl.iNDataSetsInRingBuffer = BUFFER_LEN;
   stEgControl.bEgCameraDisplayActive = FALSE;
   stEgControl.bTrackingActive        = TRUE;
   stEgControl.iScreenWidthPix        = iScreenWidth;
   stEgControl.iScreenHeightPix       = iScreenHeight;
   stEgControl.iEyeImagesScreenPos    = 1;
   stEgControl.iCommType              = EG_COMM_TYPE_SOCKET;
   stEgControl.pszCommName            = achCommAddress;
   rc = EgInit(&stEgControl);
   if (rc != 0)
   {
      swprintf_s(achText, _countof(achText), TEXT("Error opening socket\nBe sure to start EgServer before starting EgClientDemo\nProvide the server IP address on the EgClientDemo command line."));

      MessageBox(hwndEyegaze, achText,
                 szAppName, MB_ICONEXCLAMATION | MB_OK);
      bCommEstablished = FALSE;
      exit(1); // Kill the process on terminal error
   }

   hMenu = GetMenu(hwndEyegaze);
   EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_ENABLED);
   EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_ENABLED);

   bCommEstablished = TRUE;

   lct_clearscreen(BLACK);

/* Initialize the fonts.                                                    */
   lct_fontinit();
   lctFontLoad(LCT_FONT_1);

   stGazeVergence.ulCameraFieldCount = 0;

   if (stEgControl.hEyegaze != NULL)
      EgSetScreenDimensions(
        &stEgControl,
         iScreenWidth,
         iScreenHeight,
         0,    //  iEgMonHorzOffset
         0,    //  iEgMonVertOffset
         iWindowWidthPix,
         iWindowHeightPix,
         iWindowHorzOffset,
         iWindowVertOffset);

   return 0;
}
/****************************************************************************/
/****************************************************************************/
/* EgClientDemo Support Functions                                           */
/****************************************************************************/
/****************************************************************************/
DWORD WINAPI ProcessGazepointData(void *hfEyegazePort)

/* This function executes a continuous loop to receive and process the      */
/* gazepoint data from the Eyegaze System.  This is the place in the        */
/* program where the developer should put most of his custom real-time      */
/* Eyegaze data processing functions.  Presently, this function simply      */
/* displays the gazepoint on the client monitor.  The gazepoint data        */
/* processing continues until the user stops it from the menu.              */
{
   int iVis;   // the number of the vision system producing the most recent sample

/* Clear the screen.                                                        */
   lct_clearscreen(BLACK);

/* Start data flowing from the Eyegaze system.                              */
   stEgControl.bTrackingActive = TRUE;

/* Loop until the user selects "End" to end the eyegaze processing.         */
   for (EVER)
   {
/*    Get the eyegaze data for the next camera frame.                       */
      iVis = EgGetData(&stEgControl);

/*    Adjust the gaze coordinates to accomodate the window offset.          */
      iIGaze = stEgControl.pstEgData->iIGaze - iWindowHorzOffset;
      iJGaze = stEgControl.pstEgData->iJGaze - iWindowVertOffset;

/*    Plot the gazepoint on the client monitor.                             */
      PlotGazepoint(iIGaze, iJGaze, stEgControl.pstEgData->bGazeVectorFound);

/*    One option in the EgServer program allows eye images to be sent to    */
/*    the client.  If this is enabled, the eye image events will come in    */
/*    through the EgGetEvent function.                                      */
/*    Check to see if there's an event waiting to be processed.             */
      {
         int iEvent;
         static char achBuffer[6000];
         iEvent = EgGetEvent(&stEgControl, (void *)&achBuffer);
// 060830 PLN removed         if (iEvent != EG_EVENT_UPDATE_EYE_IMAGE) DisplayEyeImages(hdc, (struct _stEventEyeImage *)achBuffer, 0);
      }

/*    If the user requests a stop,                                          */
      if (bStopCommunications == TRUE)
      {
/*       Break out of the forever loop.                                     */
         break;
      }
   }

/* The 2-PC interface requires you to switch bTrackingActive to FALSE,      */
/* call EgGetData() one last time to stop the data transmission from        */
/* the server.                                                              */
   stEgControl.bTrackingActive = FALSE;
   iVis = EgGetData(&stEgControl);

   return 0;
}
/****************************************************************************/
void PlotGazepoint(int iIPix, int iJPix, int gazepoint_found)

/* This function displays a mark on the computer monitor at the predicted   */
/* gazepoint (iIPix, iJPix).  The function is intended to be called once    */
/* per camera frame.  At each call, it erases the prior mark and draws a    */
/* new one.  The mark is yellow if the Eyegaze System is presently tracking */
/* the eye (gazepoint_found = 1) and the gazepoint is on the screen.  The   */
/* mark turns green but stays on the edge of the screen when the gazepoint  */
/* goes off the screen.  The mark turns red, and remains at its last known  */
/* location, when the Eyegaze System is not tracking the eye (i.e. when     */
/* gazepoint_found = 0).                                                    */

{
   SINT  iIPixPlot;              /* pixel coordinates of the user gazepoint */
   SINT  iJPixPlot;              /*   as displayed on the computer monitor, */
                                 /*   limited by the screen dimensions      */
   int  x1,y1,x2,y2;             /* graphics coordinates (pixels)           */
   int  iColor;                  /* color of the cursor                     */
   BOOL bLimitFlag;              /* indicates if disp point went off screen */

/* Erase the horizontal line of the old gazepoint screen marker.            */
   x1 = iIPixPlot - HORZ_MARK_SIZE;
   y1 = iJPixPlot;
   x2 = iIPixPlot + HORZ_MARK_SIZE;
   y2 = iJPixPlot;
   lct_line(x1,y1,x2,y2,BLACK);

/* Erase the vertical line of the old gazepoint screen marker.              */
   x1 = iIPixPlot;
   y1 = iJPixPlot - VERT_MARK_SIZE;
   x2 = iIPixPlot;
   y2 = iJPixPlot + VERT_MARK_SIZE;
   lct_line(x1,y1,x2,y2,BLACK);

/* Initialize the limit flag to FALSE and set it TRUE below only if a limit */
/* is encountered.                                                          */
   bLimitFlag = FALSE;

/* Limit the horizontal plot point value to the computer screen dimensions. */
   if (iIPix < HORZ_MARK_SIZE)
   {
      iIPixPlot = HORZ_MARK_SIZE;
      bLimitFlag = TRUE;
   }
   else
   {
      if (iIPix > iScreenWidth - HORZ_MARK_SIZE - 1)
      {
         iIPixPlot = iScreenWidth - HORZ_MARK_SIZE - 1;
         bLimitFlag = TRUE;
      } else {
         iIPixPlot = iIPix;
      }
   }

/* Limit the vertical plot point value to the computer screen dimensions.   */
   if (iJPix < VERT_MARK_SIZE)
   {
      iJPixPlot = VERT_MARK_SIZE;
      bLimitFlag = TRUE;
   }
   else
   {
      if (iJPix > iScreenHeight - VERT_MARK_SIZE - 1)
      {
         iJPixPlot = iScreenHeight - VERT_MARK_SIZE - 1;
         bLimitFlag = TRUE;
      } else {
         iJPixPlot = iJPix;
      }
   }

/* Make the cursor yellow if a good gaze point was found, green if the      */
/* point was off screen or red if the gazepoint was not found.              */
   if (gazepoint_found == 1)
   {
      if (bLimitFlag == FALSE)
      {
         iColor = YELLOW;
      }  else  {
         iColor = LIGHT_GREEN;
      }
   }
   else
   {
      iColor = LIGHT_RED;
   }

/* Draw the horizontal line of the new gazepoint screen marker.             */
   x1 = iIPixPlot - HORZ_MARK_SIZE;
   y1 = iJPixPlot;
   x2 = iIPixPlot + HORZ_MARK_SIZE;
   y2 = iJPixPlot;
   lct_line(x1,y1,x2,y2,iColor);

/* Draw the vertical line of the new gazepoint screen marker.               */
   x1 = iIPixPlot;
   y1 = iJPixPlot - VERT_MARK_SIZE;
   x2 = iIPixPlot;
   y2 = iJPixPlot + VERT_MARK_SIZE;
   lct_line(x1,y1,x2,y2,iColor);
}
/****************************************************************************/
DWORD WINAPI SupportEyegazeCalibration(void *pVoid)
{
   HMENU hMenu;

   EgCalibrate1(&stEgControl, hwndEyegaze, EG_CALIBRATE_NONDISABILITY_APP);

   hMenu = GetMenu(hwndEyegaze);
   EnableMenuItem(hMenu, ID_COMMUNICATIONS_CALIBRATE,    MF_ENABLED);
   EnableMenuItem(hMenu, ID_COMMUNICATIONS_BEGIN,        MF_ENABLED);
   EnableMenuItem(hMenu, ID_COMMUNICATIONS_END,          MF_GRAYED);

   lct_clearscreen(BLACK);

   return FALSE;
}
/****************************************************************************/
BOOL CALLBACK OpenFileDialogProc(HWND hDlg, UINT message,
                                 WPARAM wParam, LPARAM lParam)
{
   _TCHAR achText[260];

   switch (message)
   {
      case WM_INITDIALOG:
         return TRUE;

      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
            case IDOK:
               GetDlgItemText(hDlg, IDC_FILENAME, achText, sizeof(achText));
               swscanf_s(achText, TEXT("%s"), achFileName, 260);
               EndDialog(hDlg, 1);
               return TRUE;

            case IDCANCEL:
               EndDialog(hDlg, 0);
               return FALSE;
         }
         break;
   }
   return FALSE;
}
/****************************************************************************/

