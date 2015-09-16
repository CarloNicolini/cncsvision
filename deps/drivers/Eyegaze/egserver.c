#define _DEBUG_OUTPUT
/****************************************************************************

File Name:       EgServer.C
Program Name:    Eyegaze System Data Server

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

Date Created:    08/27/90
                 02/28/00  converted to Windows program

*****************************************************************************

   This EgServer program causes the Eyegaze System to act as a peripheral
device to an external client computer.  In this case, the Eyegaze camera is
mounted under the client computer monitor and the Eyegaze System measures a
person's gazepoint on the client computer's monitor.  In response to commands
from the client computer, the EgServer program a) calibrates the Eyegaze
System for a person looking at the client computer's monitor, and b) transmits
gazepoint data to the client computer in real time.  EgServer is designed to
communicate with a client program, such as EgClientDemo, running on the client
computer.  It is assumed that the Eyegaze and client computers are connected
by a TCP/IP network.

   The EgServer program in the Eyegaze System responds to the following
single-character commands received from the client computer:

SYNCHRONIZATION AND CALIBRATION COMMANDS - for establishing communications
between the client and the Eyegaze computers, and for starting the Eyegaze
Calibration procedure:

   Q   Query the Eyegaze System - The Eyegaze System Returns an 'A' to
       acknowledge communications readiness.
   C   Perform the Eyegaze calibration procedure - During the calibration
       procedure, the Eyegaze System sends the client a series of commands to
       display the calibration graphics and text on the client computer's
       monitor.
   G   Get the calibration coefficients from the Eyegaze System --
       The Eyegaze System sends the present calibration data to the client.

GAZE DATA TRANSMISSION COMMANDS - for controlling the transition of
gazepoint data to the client computer in real time:

   B   Begin the frame-by-frame transmission of gazepoint data.
   E   End the frame-by-frame transmission of gazepoint data.

COMMANDS FOR SAVING GAZE TRACE DATA TO A FILE ON THE EYEGAZE SYSTEM -
In some cases it may be desired to store the gaze trace data in a file on
the Eyegaze System rather than having the data transferred to the client and
storing it to a file there.  The following commands allow the client computer
program to tell the Eyegaze System to open a file, start and stop collecting
gazepoint data, mark time events, and close the file:

   O   Open a file and receive file header data - first 11 characters
       are file name - terminate header w/escape sequence.
   R   Start storing Eyegaze data to file.
   P   Stop storing Eyegaze data to file.
   M   Mark an event in the Eyegaze data.  The purpose of "marking" the
       Eyegaze data is to provide time synchronization signals in within
       the gaze trace history.  Each time EgServer receives an "M", the
       integer value of the mark indicator is increased by 1.
   F   Close the file.

Note: In some cases it is desired to collect gaze trace data from a test
program that cannot be modified (reprogrammed) to interact with the Eyegaze
System.  In this case it is possible to run the test program on the client
computer and MANUALLY control the collection of gazepoint data on the
Eyegaze System computer.  The procedure to collect gaze trace data from
such a program is as follows:

 a) Perform an Eyegaze calibration for the client computer.  Run the EgServer
    program on the Eyegaze computer and the EgClientDemo program on the client
    computer.  To calibrate the test subject, select the "Calibrate" option
    on the EgClientDemo program on the client computer.
 b) Select the "Manual Data Collection" option on the EgServer program on
    the Eyegaze System.
 c) Run the test program on the client computer.
 d) Control the data collection from the control screen on the Eyegaze
    System.

Caution:  Using manual control of data collection under EgServer, the
accuracy of time correlations between eyegaze activity and screen display
events is limited by the precision of manual event marking.

The format of the gaze trace data file stored on the Eyegaze System is as
follows.  The first seven lines contain header data.  Each following line
contains one sample of eyetracking data:

   samp   Eye     Gazepoint  Pupil   Eyeball-Position  Focus  Mark
   indx  Found    X      Y    Diam     X     Y     Z   Range  Count
         (t/f)  (pix)  (pix)  (mm)   (mm)  (mm)  (mm)   (mm)

JOYSTICK COMMANDS - for use with Eyefollower only:

   J   Joystick command -- followed by three signed characters.
       Each character is a signed relative position step for one of the
       three axes (-128 to +127).
       Order of characters:
          Gimbal yaw      (+ implies point camera to its right)
          Gimbal pitch    (+ implies point camera up)
          lens extension  (+ implies focus farther out)

   The EgServer program may be loaded into the Eyegaze computer either
before
or after the client program is started.  Eyegaze operations begin as soon as
both programs are running.  During the calibration procedure, the Eyegaze
System displays the calibration graphics on its own monitor while sending
the display commands to the client computer.  At all other times, the EgServer
program displays the person's gazepoint data on the Eyegaze System monitor,
whether or not the gazepoint data are being sent to the client.  To terminate
the EgServer program, press the Escape key the on the Eyegaze computer
keyboard.

*****************************************************************************/
/* REQUIRED FUNCTION, VARIABLE AND CONSTANT DEFINITIONS                     */

#define lct_comm_close             lct_socket_close
#define lct_comm_read_char         lct_socket_read_char
#define lct_comm_send_char         lct_socket_send_char
#define lct_comm_get_input_q_count lct_socket_get_input_q_count
#define EG_COMM_TYPE               EG_COMM_TYPE_SOCKET

#include <windows.h>
#undef DELETE
#include <stdio.h>       /* standard computer Input/Output functions        */
#include <string.h>      /* string manipulation functions                   */
#include <math.h>        /* high level math functions                       */
#include <stdlib.h>      /* C run-time functions                            */
#include <conio.h>       /* console I/O functions                           */
#include <process.h>     /* computer process control functions              */
#include <time.h>
#include <egwin.h>       /* Eyegaze functions and variables                 */
#include <lctsupt.h>     /* Eyegaze support functions                       */
#include <lctcolor.h>    /* color constants for ESG calls                   */
#include <igutil.h>      /* Eyegaze System utility functions                */
#undef NO_ERROR
#include <lctfont.h>
#include <lctkeys.h>
#include <LctCalib.h>
#include <suprt2pc.h>
#include <ClientImages.h>
#include <lctwin.h>
#include "EgServer.h"
#include <lcttimer.h>
#include <GazeVergence.h>
#include <EgErrorMessage.h>

/****************************************************************************/
/* FUNCTION PROTOTYPES:                                                     */

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT uiMessage,
                            WPARAM wParam, LPARAM lParam);
DWORD WINAPI InitSerialComm(void *pVoid);
BOOL  CALLBACK OpenFileDialogProc(HWND hDlg, UINT message,
                                  WPARAM wParam, LPARAM lParam);
void  EgEyeImageDraw(int iState);
DWORD WINAPI ReceiveThread(void *pVoid);
DWORD WINAPI EgServer(void *pVoid);
DWORD WINAPI SocketMonitor(void *pVoid);
void  PlotGazepoint(int iIPix, int iJPix, int iGazepointAge);
void  EgServerExit(void *hfComm);
void  CallCalibrate(void *hfComm);
void  lctSaveEyeImage(void);

/* Eyegaze functions to support communications with a client computer:      */

int   AcknowledgeClient(void *hfComm);
int   SendGazepointDataAscii(void *hfComm, int iVis);
int   SendGazepointDataBinary(void *hfComm, int iVis);
int   SendVergenceDataBinary(void *hfComm, int iVis);
void  WriteIniFile(void);
void  UpdateTextDisplays(HWND hwnd, int iCollectionSamples, int iMarkCount);

// For Clemson Wheatstone system:
void OverrideDistanceBetweenEyes(float fDbeOverrideMm);
void OverrideLensEyeRange(float fLensEyeRangeMm, struct _stEgControl *pstEgControl);
BOOL  CALLBACK OverrideDBEDialogProc(HWND hDlg, UINT message,
                                  WPARAM wParam, LPARAM lParam);
BOOL  CALLBACK OverrideRangeDialogProc(HWND hDlg, UINT message,
                                  WPARAM wParam, LPARAM lParam);


/****************************************************************************/
/* PROGRAM CONSTANTS:                                                       */

#define  EVER            ;;      /* Used in infinite for(EVER) loops        */

#define  HORZ_MARK_SIZE  10      /* Size of the gazepoint screen marker     */
                                 /*   on the computer monitor (pixels)      */
#define  VERT_MARK_SIZE  10      /* Size of the gazepoint screen marker     */
                                 /*   on the computer monitor (pixels)      */

/****************************************************************************/
/* GLOBAL VARIABLES                                                         */

int   iObserverWindowWidthPix;  /* pixel dimensions of the observation      */
int   iObserverWindowHeightPix; /*   window                                 */
char  achComPort[10];
char  achFileName[260];
UINT  uiMarkCount;
void *hfComm;                   /* handle to the communications port        */
                                /*   to the client computer                 */
BOOL  bSaveGazepointData=FALSE; /* flag indicating whether client wants     */
                                /*   eyegaze data to be saved to disk.      */
int   iCollectionSamples;
BOOL  bRecycleServerThread = FALSE;
BOOL  bSendGazepointDataFlag = FALSE;/* flag indicating if client wants     */
                                /*   eyegaze data to be sent out            */
BOOL  bSendVergenceDataFlag = FALSE;/* flag indicating if client wants      */
                                /*   vergence data to be sent out           */

float fClientMonWidthMm, fClientMonHeightMm;
int   iClientMonWidthPix = 1024, iClientMonHeightPix = 768;
int   iClientWindowWidthPix = 1, iClientWindowHeightPix = 1;
int   iClientWindowHorzOffset, iClientWindowVertOffset;

BOOL  bWaitForCalibration = FALSE;
BOOL  bReopenCommunications = FALSE;

/****************************************************************************/
HWND hwndEyegaze;
_TCHAR szAppName[] = TEXT("EgServer");

HDC      memdc;                 /* Stores the virtual device handle         */
HBITMAP  hbit;                  /* Stores the virtual bitmap                */
HBRUSH   hbrush;                /* Stores the brush handle                  */

HDC      hdc;
HPALETTE hpal;
UINT     cxClient, cyClient;
int      hvb;
HANDLE   hAccel;
RECT     stWindowRect;
int      iWindowWidthPix;
int      iWindowHeightPix;
int      iWindowHorzOffset;
int      iWindowVertOffset;
int      iScreenWidthPix;
int      iScreenHeightPix;
int      bStopCommunications = FALSE;
BOOL     bSerialCommEstablished;
BOOL     bSendDataAscii = FALSE;
BOOL     bSendEyeImages = FALSE;
BOOL     bSend60Hz      = FALSE;
BOOL     bSendIndividualEyeData = FALSE;

#define BUFFER_LEN    10
struct _stEgControl stEgControl;
char    achCmdLine[260];
struct _stEyeImageInfo stEyeImageInfo;
static BOOL bDisplayState = 1;

/***************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpszCmdLine,
                   int       nWinMode)
{
   HWND       hwnd;
   MSG        msg;
   WNDCLASSEX wndclass;

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
   iScreenWidthPix = GetSystemMetrics(SM_CXSCREEN);
   iScreenHeightPix = GetSystemMetrics(SM_CYSCREEN);

/* Save the command line to extract the server IP address or com port name. */
   strcpy_s(achCmdLine, 260, lpszCmdLine);

   hwnd = CreateWindow(szAppName, // window class name
      TEXT("EgServer - Eyegaze System - LC Technologies, Inc."), // window caption
      WS_OVERLAPPEDWINDOW,        // window style
      0,                          // initial x position
      0,                          // initial y position
      iScreenWidthPix,            // initial x size
      iScreenHeightPix,           // initial y size
      NULL,                       // parent window handle
      NULL,                       // window menu handle
      hInstance,                  // program instance handle
      NULL);                      // creation parameters

/* Display the window                                                       */
   ShowWindow(hwnd, nWinMode);
   UpdateWindow(hwnd);

   hAccel = LoadAccelerators(hInstance, TEXT ("EGSERVER"));

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
   static BOOL bControlKeyPressed = FALSE;

   switch (uiMessage)
   {
      case WM_CREATE:
/*       Save the instance handle for the dialog box calls below.           */
         hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

/*       Determine the upper-left corner of the client area in screen       */
/*       coordinates.                                                       */
         point.x = 0;
         point.y = 0;
         ClientToScreen(hwnd, &point);
         GetClientRect(hwnd, &stWindowRect);

         iWindowWidthPix   = stWindowRect.right - stWindowRect.left+1;
         iWindowHeightPix  = stWindowRect.bottom - stWindowRect.top+1;
         iWindowHorzOffset = point.x;
         iWindowVertOffset = point.y;

/*       Create the virtual window.                                         */
         hdc    = GetDC(hwnd);
         memdc  = CreateCompatibleDC(hdc);
         hbit   = CreateCompatibleBitmap(hdc, iScreenWidthPix, iScreenHeightPix);
         SelectObject(memdc, hbit);
         hbrush = GetStockObject(BLACK_BRUSH);
         SelectObject(memdc, hbrush);
         PatBlt(memdc, 0, 0, iScreenWidthPix, iScreenHeightPix, PATCOPY);

         hwndEyegaze = hwnd;
         lctSetWindowHandle(hwnd, memdc, hdc, szAppName, wcslen(szAppName));

/*       Assume, at first that the communications port is COM 1.            */
         strcpy_s(achComPort, _countof(achComPort), "COM1");

/*       Setup the video-related variables.                                 */
         iObserverWindowWidthPix  = iWindowWidthPix;
         iObserverWindowHeightPix = iWindowHeightPix;

/*       Look for a file called "egserver.ini".  If present, read in a      */
/*       value representing "bSendDataAscii".                               */
         {
            FILE *hf;
            hMenu = GetMenu(hwnd);
            fopen_s(&hf, "EgServer.ini", "r");
            if (hf != NULL)
            {
               fscanf_s(hf, "%i %i %i %i", &bSendDataAscii, &bSendEyeImages, &bSend60Hz, &bSendIndividualEyeData);
               fclose(hf);
            }
            if (bSendDataAscii == TRUE)
            {
               CheckMenuItem(hMenu, ID_DATA_ASCII,   MF_CHECKED);
               CheckMenuItem(hMenu, ID_DATA_BINARY,  MF_UNCHECKED);
            }
            else
            {
               CheckMenuItem(hMenu, ID_DATA_ASCII,   MF_UNCHECKED);
               CheckMenuItem(hMenu, ID_DATA_BINARY,  MF_CHECKED);
            }
            if (bSend60Hz == TRUE)
            {
               CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_CHECKED);
               CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_UNCHECKED);
            }
            else
            {
               CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_UNCHECKED);
               CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_CHECKED);
            }
            if (bSendEyeImages == TRUE)
            {
               CheckMenuItem(hMenu, ID_DATA_SENDEYEIMAGES,   MF_CHECKED);
            }
            else
            {
               CheckMenuItem(hMenu, ID_DATA_SENDEYEIMAGES,   MF_UNCHECKED);
            }
            if (bSendIndividualEyeData == TRUE)
            {
               CheckMenuItem(hMenu, ID_DATA_SENDINDIVIDUAL,   MF_CHECKED);
            }
            else
            {
               CheckMenuItem(hMenu, ID_DATA_SENDINDIVIDUAL,   MF_UNCHECKED);
            }
         }

         CreateThread(NULL, 0, EgServer, 0, 0, &Tid);
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
         iObserverWindowWidthPix  = LOWORD(lParam);
         iObserverWindowHeightPix = HIWORD(lParam);
         break;

      case WM_SETFOCUS:
         InvalidateRect(hwnd,NULL,TRUE);
         return 0;

/*    We want to catch function keys here and pass them to calibration.     */
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

            case 122:  // F11
               lctSaveEyeImage();
               break;

/*          Use the "F12" key to switch the video display on and off.       */
            case 123:
               if (bControlKeyPressed == TRUE)  // CTRL_F12 turns images off
               {
                  bDisplayState = !bDisplayState;
               }
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
         hMenu = GetMenu(hwnd);
         switch(LOWORD (wParam))
         {
            case ID_FILE_EXIT:
               EgServerExit(hfComm);
               SendMessage(hwnd, WM_CLOSE, 0, 0);
               break;

            case ID_DATA_ASCII:
               CheckMenuItem(hMenu, ID_DATA_ASCII,   MF_CHECKED);
               CheckMenuItem(hMenu, ID_DATA_BINARY,  MF_UNCHECKED);
               bSendDataAscii = TRUE;
               WriteIniFile();
               break;

            case ID_DATA_BINARY:
               CheckMenuItem(hMenu, ID_DATA_ASCII,   MF_UNCHECKED);
               CheckMenuItem(hMenu, ID_DATA_BINARY,  MF_CHECKED);
               bSendDataAscii = FALSE;
               WriteIniFile();
               break;

            case ID_DATA_60HZ:
               CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_CHECKED);
               CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_UNCHECKED);
               bSend60Hz = TRUE;
               WriteIniFile();
               break;

            case ID_DATA_120HZ:
               CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_UNCHECKED);
               CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_CHECKED);
               bSend60Hz = FALSE;
               WriteIniFile();
               break;

            case ID_DATA_SENDEYEIMAGES:
               bSendEyeImages = !bSendEyeImages;
               WriteIniFile();
               if (bSendEyeImages == TRUE)
               {
                  CheckMenuItem(hMenu, ID_DATA_SENDEYEIMAGES,   MF_CHECKED);
               }
               else
               {
                  CheckMenuItem(hMenu, ID_DATA_SENDEYEIMAGES,   MF_UNCHECKED);
               }
               break;

            case ID_DATA_SENDINDIVIDUAL:
               bSendIndividualEyeData = !bSendIndividualEyeData;
               WriteIniFile();
               if (bSendIndividualEyeData == TRUE)
               {
                  CheckMenuItem(hMenu, ID_DATA_SENDINDIVIDUAL,   MF_CHECKED);
               }
               else
               {
                  CheckMenuItem(hMenu, ID_DATA_SENDINDIVIDUAL,   MF_UNCHECKED);
               }
               break;

            case ID_DATA_OVERRIDEDBE:
/*             Display the Override DBE dialog box.                         */
               DialogBox(hInstance, MAKEINTRESOURCE(IDD_OVERRIDE_DBE),
                         hwnd, OverrideDBEDialogProc);
               break;

            case ID_DATA_OVERRIDERANGE:
/*             Display the Override Range dialog box.                       */
               DialogBox(hInstance, MAKEINTRESOURCE(IDD_OVERRIDE_RANGE),
                         hwnd, OverrideRangeDialogProc);
               break;

            case ID_MANUALDATACOLLECTION_OPENFILE:
/*             'O'pen a file and receive file header data.                  */

               if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_OPENFILE),
                         hwnd, OpenFileDialogProc) == TRUE)
               {
                  EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_OPENFILE,            MF_GRAYED);
                  EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_BEGINDATACOLLECTION, MF_ENABLED);
                  EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_MARKEVENT,           MF_ENABLED);
                  EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_CLOSEFILE,           MF_ENABLED);

                  EgLogFileOpen(&stEgControl, achFileName, "a");
                  uiMarkCount = 0;
               }
               break;

            case ID_MANUALDATACOLLECTION_BEGINDATACOLLECTION:
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_ENDDATACOLLECTION,   MF_ENABLED);
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_BEGINDATACOLLECTION, MF_GRAYED);

/*             Start storing Eyegaze data to file.                          */
               EgLogStart(&stEgControl);
               bSaveGazepointData = TRUE;
               break;

            case ID_MANUALDATACOLLECTION_MARKEVENT:
/*             Mark an event in the Eyegaze data.                           */
               uiMarkCount++;
               EgLogMark(&stEgControl);
               break;

            case ID_MANUALDATACOLLECTION_ENDDATACOLLECTION:
/*             Stop storing Eyegaze data to file.                           */
               EgLogStop(&stEgControl);
               bSaveGazepointData = FALSE;

               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_ENDDATACOLLECTION,   MF_GRAYED);
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_BEGINDATACOLLECTION, MF_ENABLED);

               break;

            case ID_MANUALDATACOLLECTION_CLOSEFILE:
/*             Close the file.                                              */
               EgLogFileClose(&stEgControl);
               bSaveGazepointData = FALSE;

               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_OPENFILE,            MF_ENABLED);
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_BEGINDATACOLLECTION, MF_GRAYED);
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_MARKEVENT,           MF_GRAYED);
               EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_CLOSEFILE,           MF_GRAYED);

               break;

            case ID_HELP_ABOUT:
               MessageBox(hwnd,TEXT ("EgServer Program\n")
                               TEXT ("(c) LC Technologies, 2000-2005 "),
                          szAppName, MB_ICONINFORMATION | MB_OK);
               break;
         }
         break;

      case WM_DESTROY:
/*       Close the Eyegaze communications port.                                   */
         lct_comm_close(hfComm);
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
               swscanf_s(achText, TEXT("%s"), achFileName, 260); // NOTE: convert wide to ascii here
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
BOOL CALLBACK OverrideDBEDialogProc(HWND hDlg, UINT message,
                                    WPARAM wParam, LPARAM lParam)
{
   _TCHAR achText[260];
   float  fDbeOverrideMm;

   switch (message)
   {
      case WM_INITDIALOG:
         return TRUE;

      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
            case IDOK:
               GetDlgItemText(hDlg, IDC_DBE, achText, sizeof(achText));
               swscanf_s(achText, TEXT("%f"), &fDbeOverrideMm, 260);
               OverrideDistanceBetweenEyes(fDbeOverrideMm);
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
BOOL CALLBACK OverrideRangeDialogProc(HWND hDlg, UINT message,
                                    WPARAM wParam, LPARAM lParam)
{
   _TCHAR achText[260];
   float  fLensEyeRangeMm;

   switch (message)
   {
      case WM_INITDIALOG:
         return TRUE;

      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
            case IDOK:
               GetDlgItemText(hDlg, IDC_RANGE, achText, sizeof(achText));
               swscanf_s(achText, TEXT("%f"), &fLensEyeRangeMm, 260);
               OverrideLensEyeRange(fLensEyeRangeMm, &stEgControl);
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
/****************************************************************************/
/* EgServer Main Program                                                    */
/****************************************************************************/
/****************************************************************************/
DWORD WINAPI EgServer(void *pVoid)
{
   int           iXPixServerWindow;  /* pixel locations of corresponding    */
   int           iYPixServerWindow;  /*   gazepoint on the observer monitor */
   _TCHAR        achErrorMessage[50];
   int           rc;
   long          Tid;
   int           iNVisionSystems;
   int           iVis;
   HMENU         hMenu = GetMenu(hwndEyegaze);

/* Initialize the Eyegaze system.                                           */
   stEgControl.iNDataSetsInRingBuffer = BUFFER_LEN;
   stEgControl.bEgCameraDisplayActive = FALSE;
   stEgControl.iScreenWidthPix        = iScreenWidthPix;
   stEgControl.iScreenHeightPix       = iScreenHeightPix;
   stEgControl.iEyeImagesScreenPos    = 1;
   stEgControl.iCommType              = EG_COMM_TYPE_LOCAL;
   rc = EgInit(&stEgControl);
   if (rc != 0)
   {
      _TCHAR achErrorText[120];
      _TCHAR achMessage[120];

      SetForegroundWindow(NULL);

/*    Convert the error return code to text for display.                    */
      EgErrorNumberToText(rc, achMessage, _countof(achMessage));

      swprintf_s(achErrorText, _countof(achErrorText),
         _TEXT("There was an error initializing the Eyegaze Edge System: %i\n%s"), rc, achMessage);
      MessageBox(NULL, achErrorText,
                 _T("Eyegaze Edge"), MB_ICONEXCLAMATION | MB_OK);

/*    Since EgInit() failed, there's no need to call EgExit().  Just        */
/*    terminate the program.                                                */
      exit(99);
   }

   EgEyeImageInit(&stEyeImageInfo, 4);
   iNVisionSystems = lctNVisionSystems();

/* If we have 2 vision systems, enable the 60/120 Hz send items on the menu */
   if (iNVisionSystems == 2)
   {
      EnableMenuItem(hMenu, ID_DATA_60HZ, MF_ENABLED);
      EnableMenuItem(hMenu, ID_DATA_120HZ, MF_ENABLED);
      if (bSend60Hz == FALSE)
      {
         CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_UNCHECKED);
         CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_CHECKED);
      }
      else
      {
         CheckMenuItem(hMenu, ID_DATA_60HZ,   MF_CHECKED);
         CheckMenuItem(hMenu, ID_DATA_120HZ,  MF_UNCHECKED);
      }
   }

   #define NO_BOOST_PRIORITY  // increase the priority of the EgServer thread
                           //   to make sure communications are not delayed
   #define LCT_THREAD_PRIORITY  THREAD_PRIORITY_TIME_CRITICAL
/* Set this thread to high priority to ensure that the Eyegaze image        */
/* processing functions get the processor resources required to keep up     */
/* with real time.                                                          */
   #if defined BOOST_PRIORITY
      if (SetThreadPriority(NULL, LCT_THREAD_PRIORITY) == 0)
      {
         int iError;
         iError = GetLastError();
      }
   #endif   // BOOST_PRIORITY

reopen_comms:
   bReopenCommunications = FALSE;
   #define SOCKET_SERVER 1
/* Open the port to the client computer.  Note that the following call      */
/* blocks, awaiting a connection from a client.                             */
/* NOTE: the Remote Address (here 127.0.0.1) is not used in server mode.    */
   hfComm = lct_socket_open(SOCKET_SERVER, "3999", TEXT("127.0.0.1"), achErrorMessage, 50);
   if (hfComm == NULL)
   {
      _TCHAR achDisplay[200];
      swprintf_s(achDisplay, _countof(achDisplay), TEXT("Error opening socket to client: %s\n"), achErrorMessage);
      MessageBox(hwndEyegaze, achDisplay,
                 szAppName, MB_ICONEXCLAMATION | MB_OK);
      exit(89);
   }
   bRecycleServerThread = FALSE;
   CreateThread(NULL, 0, SocketMonitor, 0, 0, &Tid);

/* Create a thread to monitor the communications from the client machine.   */
   CreateThread(NULL, 0, ReceiveThread, 0, 0, &Tid);

/* Enable the manual data collection menu.                                  */
   {
      HMENU hMenu;
      hMenu = GetMenu(hwndEyegaze);
      EnableMenuItem(hMenu, ID_MANUALDATACOLLECTION_OPENFILE, MF_ENABLED);
   }

/* Clear the screen to black.                                               */
   lct_clearscreen(BLACK);

/* Initialize the send-data flag to false.                                  */
   bSendGazepointDataFlag = FALSE;
   bSendVergenceDataFlag = FALSE;

/* Start the image processing software running.                             */
   stEgControl.bTrackingActive = TRUE;

/* Loop until the user requests a stop.                                     */
/* Typically, the loop iterates each camera frame, unless                   */
/* the Eyegaze System is calibrating or transmitting the calibration        */
/* results to the client.                                                   */
   for (EVER)
   {
/*    With the receive communications taking place in another thread, we    */
/*    must wait here while the calibration is running.  Two functions       */
/*    cannot both be calling EgGetData().                                   */
      while (bWaitForCalibration == TRUE) Sleep(100);

      if (bReopenCommunications == TRUE)
      {
        goto reopen_comms;
      }

/*    Track the user gazepoint.                                             */
      iVis = EgGetData(&stEgControl);

// UGLY hack for ugly problem:
// We're seeing three samples with the same CFC on binocular systems.  The second sample for a single
// vision system in a single CFC comes within 2 milliseconds of the first, so we're assuming that
// the second one is the one to throw out.  (James Weatherhead / Eyetracking Inc.  060831)
      {
         static unsigned long ulCFCPast[2];
         if (stEgControl.pstEgData->ulCameraFieldCount == ulCFCPast[iVis]) continue;
         ulCFCPast[iVis] = stEgControl.pstEgData->ulCameraFieldCount;
      }

      if (bDisplayState == TRUE)
      {
         EgEyeImageDisplay(
            0,
            stEgControl.iScreenWidthPix - stEyeImageInfo.iWidth,
            0,
            stEyeImageInfo.iWidth,
            stEyeImageInfo.iHeight,
            hdc);
         if (iNVisionSystems == 2)
         {
            EgEyeImageDisplay(
               1,
               0,
               0,
               stEyeImageInfo.iWidth,
               stEyeImageInfo.iHeight,
               hdc);
         }
      }

/*    If the Eyegaze System is in the send-data mode,                       */
/*    and either this is vision system 0 (always true for mono systems) or  */
/*    we want to send 120 Hz data, send data to the other PC.               */
      if ((bSendGazepointDataFlag == TRUE) &&
         ((iVis == 0) || (bSend60Hz == FALSE)))
      {
/*       Send the camera frame of data to the client.                       */
         if (bSendDataAscii == TRUE)
         {
            rc = SendGazepointDataAscii(hfComm, iVis);
         }
         else
         {
            rc = SendGazepointDataBinary(hfComm, iVis);

            if (bSendEyeImages == TRUE)
            {
               CreateAndSendTinyBitmap(
                  hfComm,
                  stEgControl.pstEgData->ulCameraFieldCount%2,
                  stEgControl.pstEgData->fFocusRangeOffsetMm,
                  stEgControl.pstEgData->bGazeVectorFound,
                  stEgControl.hEyegaze,
                  0,
                  0,
                  10,    // iris
                  90,    // pupil
                  250);  // glint
            }
         }

         if (rc == -1)
         {
/*          Close the now-defunct socket connection.                        */
            lct_socket_close(hfComm);
            goto reopen_comms;
         }
      }

// begin vergence
/*    If the Eyegaze System is in the send-vergence-data mode,              */
/*    and either this is vision system 0 (always true for mono systems) or  */
/*    we want to send 120 Hz data, send data to the other PC.               */
      if (bSendVergenceDataFlag == TRUE)
      {
/*       Send the camera frame of data to the client.                       */
/*       NOTE: We don't have an ASCII version of sending vergence data.     */
         rc = SendVergenceDataBinary(hfComm, iVis);

         if (rc == -1)
         {
/*          Close the now-defunct socket connection.                        */
            lct_socket_close(hfComm);
            goto reopen_comms;
         }
      }

/*    When the SocketMonitor detects a failed socket, it sets this flag     */
/*    to TRUE.                                                              */
      if (bRecycleServerThread == TRUE)
      {
/*       Close the now-defunct socket connection.                           */
         lct_socket_close(hfComm);

         goto reopen_comms;
      }

/*    If the Eyegaze System is in the save-data mode,                       */
      if (bSaveGazepointData == TRUE)
      {
         iCollectionSamples++;

         UpdateTextDisplays(hwndEyegaze, iCollectionSamples, uiMarkCount);
      }

/*    Compute the point on the observation monitor screen corresponding     */
/*    to the user's gazepoint on the client computer monitor screen.        */
      iXPixServerWindow = stEgControl.pstEgData->iIGaze *
         iObserverWindowWidthPix / iClientMonWidthPix;

      iYPixServerWindow = stEgControl.pstEgData->iJGaze *
         iObserverWindowHeightPix / iClientMonHeightPix;

/*    Plot the user's corresponding gazepoint on the observation screen,    */
/*    i.e. on the Eyegaze System's color monitor.                           */
      PlotGazepoint(iXPixServerWindow, iYPixServerWindow, stEgControl.pstEgData->bGazeVectorFound);

/*    If the user hits a key on the Eyegaze System keyboard,                */
      if (bStopCommunications == TRUE)
      {
         ExitThread(0);
      }
   }
   return 0;
}
/****************************************************************************/
/* EgServer Support Functions                                               */
/****************************************************************************/
void PlotGazepoint(int iIPix, int iJPix, int iGazepointAge)

/* This function displays a mark on the computer monitor at the predicted   */
/* gazepoint (iIPix, iJPix).  The function is intended to be called once    */
/* per camera frame.  At each call, it erases the prior mark and draws a    */
/* new one.  The mark is yellow if the Eyegaze System is presently tracking */
/* the eye (iGazepointAge > 0) and the gazepoint is on the screen.  The     */
/* mark turns green but stays on the edge of the screen when the gazepoint  */
/* goes off the screen.  The mark turns red, and remains at its last known  */
/* location, when the Eyegaze System is not tracking the eye.               */

{
   SINT   iIPixPlot;            /* pixel coordinates of the user gazepoint  */
   SINT   iJPixPlot;            /*   as displayed on the computer monitor,  */
                                /*   limited by the screen dimensions       */
   int    x1, y1, x2, y2;       /* graphics coordinates (pixels)            */
   int    iColor;               /* color of the cursor                      */
   BOOL   bLimitFlag;           /* indicates if disp point went off screen  */

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
   } else {
      if (iIPix > iObserverWindowWidthPix - HORZ_MARK_SIZE - 1)
      {
         iIPixPlot = iObserverWindowWidthPix - HORZ_MARK_SIZE - 1;
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
   } else {
      if (iJPix > iObserverWindowHeightPix - VERT_MARK_SIZE - 1)
      {
         iJPixPlot = iObserverWindowHeightPix - VERT_MARK_SIZE - 1;
         bLimitFlag = TRUE;
      } else {
         iJPixPlot = iJPix;
      }
   }

/* Make the cursor yellow if a good gaze point was found, green if the      */
/* point was off screen or red if the gazepoint was not found.              */
   if (iGazepointAge > 0)
   {
      if (bLimitFlag == FALSE)
      {
         iColor = YELLOW;
      }  else  {
         iColor = LIGHT_GREEN;
      }
   } else {
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
void  EgServerExit(void *hfComm)
{
/* Close the client communications port.                                    */
   lct_comm_close(hfComm);

   EgExit(&stEgControl);

/* Exit the EgServer program.                                               */
   exit(0);
}
/****************************************************************************/
/****************************************************************************/
/* EYEGAZE FUNCTIONS TO SUPPORT COMMUNICATIONS WITH CLIENT COMPUTER         */
/****************************************************************************/
int   AcknowledgeClient(void *hfComm)

/* This function waits until communications with the client computer is     */
/* established.  It continually tests the link until it receives the query  */
/* character 'Q' from the client.  Upon receipt of the 'Q', the Eyegaze     */
/* System responds by sending the acknowledge character 'A' back to the     */
/* client computer.  This function works in conjunction with the            */
/* query_eyegaze_system() function in the client computer.                  */
/* This function normally returns a 0 when it receives the query from       */
/* the client and sends back the acknowledgement. If the query is not       */
/* received, indicating that the communications link is not hooked up or    */
/* that the EgClientDemo program is not running on the client computer, the */
/* user terminate the function and return a FALSE.                          */
/* NOTE: This is used only on serial connections.                           */
{
/* Loop until the query is received or the user requests a stop.            */
   for (EVER)
   {
/*    If a character has arrived from the client,                           */
      if (lct_comm_get_input_q_count(hfComm) >= 1)
      {
/*       Get the character, and if it is a 'Q',                             */
         if (lct_comm_read_char(hfComm) == 'Q')
         {
/*          Send back the acknowledge char and declare the client ready.    */
            lct_comm_send_char(hfComm,'A');
            return TRUE;
         }
      }

/*    If the user hit the keyboard,                                         */
      if (bStopCommunications == TRUE)
      {
         return FALSE;
      }
   }

/* The program should never reach this point.                               */
   return FALSE;
}
/****************************************************************************/
int SendGazepointDataAscii(void *hfComm, int iVis)

/* This function sends a camera frame's worth of ASCII gazepoint data to    */
/* the client computer.  The receive_gazepoint_data_ascii() function in the */
/* EgClientDemo program must receive the data in the identical sequence     */
/* that it is transmitted here.                                             */

{
   int    i_gaze_send;                 /* eyetracker values limited so as   */
   int    j_gaze_send;                 /*   not to exceed transmission      */
   int    iRadiusX100Send;             /*   format ranges                   */
   int    iDeltaCameraFieldCount_send; /* number of camera fields that      */
                                       /*   have occurred since the last    */
                                       /*   transmission.                   */

   char   achGazepointData[64];        /* ASCII string of gazepoint data    */
                                       /*   to be sent to the client        */
                                       /*   compter                         */
   int    iMessageLength;              /* number of characters sent         */
                                       /*   to the client computer          */
   static unsigned long ulCameraFieldCountPast = 0;
   BOOL   bGazepointFoundSend;

/* Limit the transmitted values i_gaze, j_gaze, and pupil radius so         */
/* they do not exceed the format ranges allowed within the                  */
/* gazepoint_data_string.                                                   */
   i_gaze_send = stEgControl.pstEgData->iIGaze;
   j_gaze_send = stEgControl.pstEgData->iJGaze;

/* Limit the data values.                                                   */
   if (i_gaze_send > 9999) i_gaze_send = 9999;
   if (i_gaze_send < -999) i_gaze_send = -999;
   if (j_gaze_send > 9999) j_gaze_send = 9999;
   if (j_gaze_send < -999) j_gaze_send = -999;

   iRadiusX100Send = (int)(stEgControl.pstEgData->fPupilRadiusMm * 100);
   if (iRadiusX100Send > 999) iRadiusX100Send = 999;
   if (iRadiusX100Send < -99) iRadiusX100Send = -99;

/* Set the found flag for sending.                                          */
   bGazepointFoundSend = stEgControl.pstEgData->bGazeVectorFound;

/* Avoid large time deltas on first pass.                                   */
   if (ulCameraFieldCountPast == 0)
   {
      ulCameraFieldCountPast = stEgControl.pstEgData->ulCameraFieldCount - 1;
   }

/* Compute the number of camera field counts since the last transmission.   */
   if (stEgControl.pstEgData->ulCameraFieldCount > ulCameraFieldCountPast)
   {
      iDeltaCameraFieldCount_send = (int)(stEgControl.pstEgData->ulCameraFieldCount -
                                          ulCameraFieldCountPast);
   }
   else
   {
      iDeltaCameraFieldCount_send = -(int)(ulCameraFieldCountPast -
                                      stEgControl.pstEgData->ulCameraFieldCount);
   }

/* Save the past camera field count.                                        */
   ulCameraFieldCountPast = stEgControl.pstEgData->ulCameraFieldCount;

/* Limit the transmitted value of the camera frame count increment.         */
   if (iDeltaCameraFieldCount_send > 99) iDeltaCameraFieldCount_send = 99;
   if (iDeltaCameraFieldCount_send < -9) iDeltaCameraFieldCount_send = -9;

/* Create an ascii string of the data to be sent.                           */
   iMessageLength = 19; // Total string length plus checksum byte
   sprintf_s(achGazepointData, _countof(achGazepointData), "%4i%4i%3i%1i%2i",
           i_gaze_send, j_gaze_send,
           iRadiusX100Send, bGazepointFoundSend,
           iDeltaCameraFieldCount_send);

/* Send the gazepoint data.                                                 */
   LctSendMessage(hfComm, EG_MESSAGE_TYPE_GAZEINFO, achGazepointData, iMessageLength,
                  EG_COMM_TYPE);

#if defined _DEBUG_OUTPUT
      {
         double lct_TimerRead(unsigned int *puiProcessorSpeedMHz);
         static double dTimeNow, dTimePast;

         dTimeNow = lct_TimerRead(NULL);
         printf("sgda: iVis: %i  cfc: %u  gazetime: %7.3f  delta_t: %5.0f  i: %i  j: %i\n",
                 iVis, 
                 stEgControl.pstEgData->ulCameraFieldCount, 
                 stEgControl.pstEgData->dGazeTimeSec, 
                 dTimeNow - dTimePast,
                 stEgControl.pstEgData->iIGaze, 
                 stEgControl.pstEgData->iJGaze);
         fflush(stdout);
         dTimePast = dTimeNow;
      }
#endif

   return 0;
}
/****************************************************************************/
int SendGazepointDataBinary(void *hfComm, int iVis)

/* This function sends a camera frame's worth of ASCII gazepoint data to    */
/* the client computer.  The receive_gazepoint_data_ascii() function in the */
/* EgClientDemo program must receive the data in the identical sequence     */
/* that it is transmitted here.                                             */

{
   int    iMessageLength;              /* number of characters sent         */
                                       /*   to the client computer          */
   static char *pData=NULL;
#if defined _DEBUG_OUTPUT
   struct _stEgData *pstEgData; // DEBUG only
#endif

   if (pData == NULL)
   {
      pData = malloc(sizeof(struct _stEgData) * 5 + 6);
   }

   pData[0] = iVis;

/* NOTE: the message length calculation (5 + 1 + sizeof...) works out:
      5 is the message overhead consisting of:
         3 bytes of message length
         1 byte for the message type
         1 byte for the checksum at the end
      1 is the iVis value prepended to the data
*/

/* Calculate the message length to be 5 + the size of the egdata struct.    */
   iMessageLength = 5 + 1 + sizeof(struct _stEgData);

/* Send individual eye data or send combined left/right eye data depending  */
/* on user selection.                                                       */
   if (bSendIndividualEyeData == TRUE)
   {
      if (iVis == 0)
         memcpy(&pData[1], (char *)&stEgControl.pstEgData[1], sizeof(struct _stEgData)); // EGDATA_CAM_0_RIGHT 1
      else
         memcpy(&pData[1], (char *)&stEgControl.pstEgData[4], sizeof(struct _stEgData)); // EGDATA_CAM_1_LEFT  4
   }
   else
   {
         memcpy(&pData[1], (char *)stEgControl.pstEgData, sizeof(struct _stEgData));
   }

/* Send the gazepoint data.                                              */
   LctSendMessage(hfComm, EG_MESSAGE_TYPE_GAZEINFO, pData, iMessageLength,
                  EG_COMM_TYPE);
#if defined _DEBUG_OUTPUT
   pstEgData = (struct _stEgData *)&pData[1];
      {
         double lct_TimerRead(unsigned int *puiProcessorSpeedMHz);
         static double dTimeNow, dTimePast;

         dTimeNow = lct_TimerRead(NULL);
         printf("sgdb: iVis: %i  cfc: %u  gazetime: %7.3f  delta_t: %5.0f  i: %i  j: %i\n",
                 iVis, 
                 pstEgData->ulCameraFieldCount, 
                 pstEgData->dGazeTimeSec, 
                 dTimeNow - dTimePast,
                 pstEgData->iIGaze, 
                 pstEgData->iJGaze);
         fflush(stdout);
         dTimePast = dTimeNow;
      }
#endif

   return 0;
}
/****************************************************************************/
int SendVergenceDataBinary(void *hfComm, int iVis)

/* This function sends a camera frame's worth of vergence data to           */
/* the client computer.                                                     */
{
   int    iMessageLength;              /* number of characters sent         */
                                       /*   to the client computer          */
   static char *pData=NULL;
   struct _stGazeVergence stGazeVergence;
   BOOL   bVergenceFound;

/* If this is the first time into this function, allocate memory for the    */
/* message data.                                                            */
   if (pData == NULL) pData = malloc(sizeof(struct _stGazeVergence) * 5 + 6);

/* Set the first element of the data structure to the current vision system.*/
   pData[0] = iVis;

/* NOTE: the message length calculation (5 + 1 + sizeof...) works out:
      5 is the message overhead consisting of:
         3 bytes of message length
         1 byte for the message type
         1 byte for the checksum at the end
      1 is the iVis value prepended to the data
*/

/* Calculate the message length to be 5 + the size of the egdata struct.    */
   iMessageLength = 5 + 1 + sizeof(struct _stGazeVergence);

/* Obtain the vergence information (assuming binocular.)                    */
   bVergenceFound = bGetVergenceAnd3DGazePoint(&stGazeVergence);
#if defined _DEBUG_OUTPUT
   printf("svdb: bVergenceFound: %i, iVis: %i\n", bVergenceFound, stGazeVergence.iVis);
   fflush(stdout);
#endif

   memcpy(&pData[1], &stGazeVergence, sizeof(struct _stGazeVergence));

/* Send the gazepoint data.                                              */
   LctSendMessage(hfComm, EG_MESSAGE_TYPE_VERGENCE, pData, iMessageLength, EG_COMM_TYPE);

   return 0;
}
/****************************************************************************/
void CallCalibrate(void *hfComm)
{
/* This function calls LctCalibrate rather than EgCalibrate because it      */
/* needs to override the settings for the screen height, width; window      */
/* width, height, and offsets with the values from the client side.         */
/* Since we're calibrating on the client monitor, the default of reading    */
/* these values from the Eyegaze system aren't appropriate.                 */

   struct _LctCalInputs stLctCalInputs;

/* Set the stand-alone flag to false.                                       */
   stLctCalInputs.bStandAlone = FALSE;

   stLctCalInputs.bFullCalibration = FALSE;

/* NOTE: This assumes that we've received a Workstation Response from the   */
/*       client machine with screen and window parameter information.       */
   stLctCalInputs.iEgScreenWidthPix = iClientMonWidthPix-1;
   stLctCalInputs.iEgScreenHeightPix = iClientMonHeightPix-1;

   stLctCalInputs.iEgWindowWidthPix  = iClientWindowWidthPix;
   stLctCalInputs.iEgWindowHeightPix = iClientWindowHeightPix;
   stLctCalInputs.iEgWindowHorzOffsetPix = iClientWindowHorzOffset;
   stLctCalInputs.iEgWindowVertOffsetPix = iClientWindowVertOffset;

   stLctCalInputs.piObserverWindowWidthPix = &iObserverWindowWidthPix;
   stLctCalInputs.piObserverWindowHeightPix = &iObserverWindowHeightPix;

   strcpy_s(stLctCalInputs.achComPort, 15, achComPort);
   stLctCalInputs.hfComm = hfComm;

   stLctCalInputs.iCommType = EG_COMM_TYPE;

   stLctCalInputs.iCalEgOrEcsType = EG_CALIBRATE_NONDISABILITY_APP;

/* Pass the flag for sending eye images to the calibration function.        */
   stLctCalInputs.iFlags = 0;
   if (bSendEyeImages == TRUE)
   {
      stLctCalInputs.iFlags = CAL_FLAG_SEND_IMAGES;
   }

   LctCalibrate(&stEgControl, &stLctCalInputs);

   iClientMonWidthPix = stLctCalInputs.iEgScreenWidthPix+1;
   iClientMonHeightPix = stLctCalInputs.iEgScreenHeightPix+1;

   iClientWindowWidthPix = stLctCalInputs.iEgWindowWidthPix;
   iClientWindowHeightPix = stLctCalInputs.iEgWindowHeightPix;
   iClientWindowHorzOffset = stLctCalInputs.iEgWindowHorzOffsetPix;
   iClientWindowVertOffset = stLctCalInputs.iEgWindowVertOffsetPix;

   lct_clearscreen(BLACK);
}
/****************************************************************************/
void UpdateTextDisplays(HWND hwnd, int iCollectionSamples, int iMarkCount)
{
   CHAR  achInfo[100];
   HDC   hdc = GetDC(hwnd);

   swprintf_s(achInfo, _countof(achInfo), TEXT("Time (seconds):  %5i  Mark: %4i"), iCollectionSamples/60, uiMarkCount);

/* Draw a black rectangle over the prior text to erase it.                  */
   SelectObject(hdc, GetStockObject(BLACK_BRUSH));
   Rectangle(hdc, 10, 200, 310, 230);

   SetTextColor(hdc, 0x00FFFFFF);
   SetBkMode(hdc, TRANSPARENT);
   TextOut(hdc, 10, 210, achInfo, _tcslen(achInfo));

   ReleaseDC(hwnd, hdc);
}
/****************************************************************************/
DWORD WINAPI SocketMonitor(void *pVoid)
{
/* This thread monitors the socket for errors.  If an error on the socket   */
/* occurs, it sets the global boolean bRecycleServerThread to TRUE.  This   */
/* causes the main EgServer() thread to recycle and wait for another        */
/* connection.                                                              */

   for (EVER)
   {
      Sleep(1000);

      if (lct_socket_get_input_q_count(hfComm) == -1)
      {
         bRecycleServerThread = TRUE;
         ExitThread(0);
      }
   }
}
/****************************************************************************/
void WriteIniFile(void)
{
   FILE *hf;
   fopen_s(&hf, "EgServer.ini", "w");
   if (hf != NULL)
   {
      fprintf(hf, "%i %i %i %i\n", bSendDataAscii, bSendEyeImages, bSend60Hz, bSendIndividualEyeData);
      fclose(hf);
   }
}
/****************************************************************************/
DWORD WINAPI ReceiveThread(void *pVoid)
{
/* This thread was added to allow messages to be processed from the client  */
/* out of synch with the field data on the Eyegaze System.  Having a        */
/* separate thread allows us to receive client messages (specifically event */
/* marks) at any time.                                                      */

   unsigned int  uiMessageLength;
   int           iMessage;
   unsigned char auchData[2000];
   int           rc;

   for (;;)
   {
/*    Attempt to read a message from the client.  If none has arrived the   */
/*    read will block.                                                      */
      rc = LctReadMessage(hfComm, &iMessage, auchData, &uiMessageLength, EG_COMM_TYPE);
      // check return code here, -1 is error, 0 is success
      if (rc != 0) 
      {
#if defined _DEBUG
         printf("ERROR in EgServer: LctReadMessage returns: %i\n", rc); fflush(stdout);
#endif
         continue;
      }

#if defined _DEBUG_OUTPUT
      printf("EgServer, Msg rec'd: msg: %i (rc=%i, -1=error, 0=success)\n", iMessage, rc);
      fflush(stdout);
#endif

/*    Respond to legitimate commands.                                       */
      switch (iMessage)
      {
         case EG_MESSAGE_TYPE_CALIBRATE:
/*          MAKE SURE EgGetData above is stalled while this runs.           */
            bWaitForCalibration = TRUE;
            Sleep(100);  // wait 1/10th sec for other thread to complete
            CallCalibrate(hfComm);
            bWaitForCalibration = FALSE;
            break;

         case EG_MESSAGE_TYPE_BEGIN_SENDING_DATA:
            bSendGazepointDataFlag = TRUE;
#if defined _DEBUG_OUTPUT
            printf("EgServer Message Received: EG_MESSAGE_TYPE_BEGIN_SENDING_DATA  Flag: %i\n", bSendGazepointDataFlag);
            fflush(stdout);
#endif
            break;

         case EG_MESSAGE_TYPE_STOP_SENDING_DATA:
            bSendGazepointDataFlag = FALSE;
#if defined _DEBUG_OUTPUT
            printf("EgServer Message Received: EG_MESSAGE_TYPE_STOP_SENDING_DATA  Flag: %i\n", bSendGazepointDataFlag);
            fflush(stdout);
#endif
            break;

         case EG_MESSAGE_TYPE_BEGIN_SENDING_VERGENCE:
            bSendVergenceDataFlag = TRUE;
#if defined _DEBUG_OUTPUT
            printf("EgServer Message Received: EG_MESSAGE_TYPE_BEGIN_SENDING_VERGENCE  Flag: %i\n", bSendVergenceDataFlag);
            fflush(stdout);
#endif
            break;

         case EG_MESSAGE_TYPE_STOP_SENDING_VERGENCE:
            bSendVergenceDataFlag = FALSE;
#if defined _DEBUG_OUTPUT
            printf("EgServer Message Received: EG_MESSAGE_TYPE_STOP_SENDING_VERGENCE  Flag: %i\n", bSendVergenceDataFlag);
            fflush(stdout);
#endif
            break;

         case EG_MESSAGE_TYPE_FILE_OPEN:
            {
               int  iFileNameLength;
               int  iModeLength;
               char achFileName[254];
               char achMode[254];

/*             Read file name and open mode from the data packet.           */
               iFileNameLength = auchData[1];
               strncpy_s(achFileName, 254, &auchData[2], iFileNameLength);
               achFileName[iFileNameLength] = '\0';   // null-terminate the string

               iModeLength = auchData[2+iFileNameLength];
               strncpy_s(achMode, 254, &auchData[2+iFileNameLength+1], iModeLength);
               achMode[iModeLength] = '\0';   // null-terminate the string

               EgLogFileOpen(&stEgControl, achFileName, achMode);
               uiMarkCount = 0;
            }
            break;

         case EG_MESSAGE_TYPE_FILE_WRITE_HEADER:
            EgLogWriteColumnHeader(&stEgControl);
            break;

         case EG_MESSAGE_TYPE_FILE_APPEND_TEXT:
            {
               int iTextLength;
               char achText[254];

/*             Read text from the data packet.                              */
               iTextLength = auchData[1];
               strncpy_s(achText, 254, &auchData[2], iTextLength);
               achText[iTextLength] = '\0';   // null-terminate the string

               EgLogAppendText(&stEgControl, achText);
            }
            break;

         case EG_MESSAGE_TYPE_FILE_START_RECORDING:
/*          Start storing Eyegaze data to file.                             */
            bSaveGazepointData = TRUE;
            EgLogStart(&stEgControl);
            break;

         case EG_MESSAGE_TYPE_FILE_STOP_RECORDING:
/*          Stop storing Eyegaze data to file.                              */
            bSaveGazepointData = FALSE;
            EgLogStop(&stEgControl);
            break;

         case EG_MESSAGE_TYPE_FILE_MARK_EVENT:
/*          Mark an event in the Eyegaze data.                              */
            uiMarkCount++;
            EgLogMark(&stEgControl);
            break;

         case EG_MESSAGE_TYPE_FILE_CLOSE:
/*          Close the file.                                                 */
            bSaveGazepointData = FALSE;
            EgLogFileClose(&stEgControl);
            break;

         case EG_MESSAGE_TYPE_WORKSTATION_RESPONSE:
            sscanf_s(&auchData[1],"%4f,%4f,%4i,%4i,%4i,%4i,%4i,%4i",
                     &fClientMonWidthMm,
                     &fClientMonHeightMm,
                     &iClientMonWidthPix,
                     &iClientMonHeightPix,
                     &iClientWindowWidthPix,
                     &iClientWindowHeightPix,
                     &iClientWindowHorzOffset,
                     &iClientWindowVertOffset);
            EgSetScreenDimensions(&stEgControl,
               iClientMonWidthPix,
               iClientMonHeightPix,
               0,   // iEgMonHorzOffsetPix
               0,   // iEgMonVertOffsetPix
               iClientWindowWidthPix,
               iClientWindowHeightPix,
               iClientWindowHorzOffset,
               iClientWindowVertOffset);
            break;

/*       Close the connection and recycle.                                  */
         case EG_MESSAGE_TYPE_CLOSE_AND_RECYCLE:
            bSendGazepointDataFlag = FALSE;
            bSendVergenceDataFlag = FALSE;
/*          Close the now-defunct socket connection.                        */
            lct_comm_close(hfComm);
            bReopenCommunications = TRUE;
            return 0; // terminate this receive thread.
            break;

         default:
            break;
      }
   }
}
/****************************************************************************/

