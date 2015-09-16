/****************************************************************************

  GazeDemo.C  Windows GUI program that tracks a users gaze and moves
              the mouse pointer to the gaze point

This program demonstrates the use of the Eyegaze functions in a simple 
example Windows program.  After performing standard Windows setup functions,
GazeDemo executes the Eyegaze Calibration procedure, which in this example 
is a separate program spawned from within GazeDemo.  Upon completion of the 
calibration process, GazeDemo creates and initiates an Eyegaze thread which 
continually tracks the user's gazepoint and moves the Windows mouse cursor 
to follow the user's gazepoint as he scans the desktop. 

(To keep the GazeDemo program window from covering too much of the original 
desktop, GazeDemo creates a small program window and positions it in the 
upper right corner of the screen.)

  Created 11/16/1998

  LC Technologies, Inc.

  Note: WinMain() and MainWndProc() are largely Windows boiler-plate code 
  having nothing to do with Eyegaze.  GazeDemo() is the central Eyegaze 
  function.

*****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <egwin.h>

/****************************************************************************/
/* Comment out this line, or change the "RUN_LOCALLY" to "NO_RUN_LOCALLY"   */
/* to switch this program to operating in the double computer               */
/* configuration.                                                           */
/* (See code just before EgInit call to see what's different.)              */
#define RUN_LOCALLY

/* NOTE: When operating in the double computer configuration, the IP        */
/*       address of the Eyegaze Server machine must be provided on the      */
/*       GazeDemo command line.                                             */

/****************************************************************************/
#define  TRUE           1       /* Self explanatory                         */
#define  FALSE          0       /*                                          */
#define  EVER          ;;       /* used in infinite for loop                */

/* Function Prototypes:                                                     */

int   WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
long  WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI GazeDemo(void *pVoid);
void  SmoothGazepoint(int g_p_vector_found,
                      int iIGaze, int iJGaze,
                      int *iIGazeSmooth, int *iJGazeSmooth,
                      int iNSmoothPoints);

/* Global Variables:                                                        */

_TCHAR   szAppName[] = TEXT("GazeDemo"); /* Application Name                */
HWND     hwndEyegaze;              /* Client window handle                  */
int      iScreenWidth;             /* pixel dimensions of the full          */
int      iScreenHeight;            /*   computer screen                     */
char     achIPAddress[50];

static struct _stEgControl stEgControl;
                                 /* The eyetracking application must define */
                                 /*   (and fill in) this Eyegaze control    */
                                 /*   structure                             */
                                 /*   (See structure template in EgWin.h)   */

/****************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpszCmdLine,
                   int       nCmdShow)
{
   HWND       hwnd;
   MSG        msg;
   WNDCLASSEX wndclass;

/* Attempt to parse an IP address out of the command line.  Note that this  */
/* is used only in the case where stEgControl.iCommType is set to           */
/* EG_COMM_TYPE_SOCKET.                                                     */
   if (sscanf_s(lpszCmdLine,"%s", achIPAddress, _countof(achIPAddress)) != 1)
   {
      strcpy_s(achIPAddress, _countof(achIPAddress), "127.0.0.1");
   }

/* Initialize GazeDemo window data, register window class and create main   */
/* window.                                                                  */
   if (!hPrevInstance)
   {
      wndclass.cbSize        = sizeof(WNDCLASSEX);
      wndclass.style         = CS_VREDRAW | CS_HREDRAW;
      wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
      wndclass.cbWndExtra    = 0; //sizeof(LPVOID)+sizeof(LPSTR);
      wndclass.cbClsExtra    = 0;
      wndclass.hInstance     = hInstance;
      wndclass.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
      wndclass.hIconSm       = LoadIcon(NULL,IDI_APPLICATION);
      wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
      wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
      wndclass.lpszMenuName  = NULL;
      wndclass.lpszClassName = szAppName;

/*    Register the class                                                    */
      if (!RegisterClassEx(&wndclass)) return FALSE;
   }

/* Obtain the current screen dimensions so Eyegaze knows the screen size.   */
   iScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
   iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

/* Create the GazeDemo window.                                              */
   hwnd = CreateWindow (szAppName,            // window class name
                        szAppName,            // window caption
                        WS_OVERLAPPEDWINDOW,  // window style
                        0,                    // initial x position
                        0,                    // initial y position
                        iScreenWidth,         // initial x size
                        iScreenHeight,        // initial y size
                        NULL,                 // parent window handle
                        NULL,                 // window menu handle
                        hInstance,            // program instance handle
                        NULL);                // creation parameters

/* Display the GazeDemo window.                                             */
   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

/* Get and dispatch messages until a WM_QUIT message is received.           */
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);   /* Translates virtual key codes             */
      DispatchMessage(&msg);    /* Dispatches message to window             */
   }

   return msg.wParam;           /* Returns the value from PostQuitMessage   */
}
/****************************************************************************/
long WINAPI MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   long Tid;

   switch (message)
   {
      case WM_CREATE:
/*       Save the window handle in an external variable used elsewhere.     */
         hwndEyegaze = hwnd;

/*       On startup, create an eyetracking thread to produce gaze info.     */
         CreateThread(NULL, 0, GazeDemo, 0, 0, &Tid);
         break;

      case WM_ENDSESSION:
      case WM_CLOSE:
/*       Call the EgExit function to shut down the vision subsystem.        */
         EgExit(&stEgControl);
         PostMessage(hwnd, WM_DESTROY, (WORD)0, (LONG)0);
         break;

      case WM_DESTROY:
         PostQuitMessage(0);

      default:
         return DefWindowProc(hwnd, message, wParam, lParam);
   }
   return 0L;
}
/****************************************************************************/
DWORD WINAPI GazeDemo(void *pvoid)
{
/* This function is the core of the Eyegaze application that is executed    */
/* as a separate thread when the program starts.                            */

   #define EG_BUFFER_LEN 60      /* The constant EG_BUFFER_LEN sets the     */
                                 /*   number of past samples stored in      */
                                 /*   its gazepoint data ring buffer.       */
                                 /*   Assuming an Eyegaze sample rate of    */
                                 /*   60 Hz, the value 60 means that one    */
                                 /*   second's worth of past Eyegaze data   */
                                 /*   is always available in the buffer.    */
                                 /*   The application can get up to 60      */
                                 /*   samples behind the Eyegaze image      */
                                 /*   processing without losing eyetracking */
                                 /*   data.                                 */

/* Demonstration Program variables:                                         */
   int iIGazeSmooth;             /* smoothed values of the eyegaze          */
                                 /*   coordinates (pixels)                  */
   int iJGazeSmooth;
   int iVis;                     /* The vision system returning data        */
   int rc;                       /* Return code from EgInit function call   */

/*---------------------------- Function Code -------------------------------*/

/* Calibrate to the Eyegaze user (test subject).                            */
/* Since an Eyegaze thread has not yet been started, execute the external   */
/* Eyegaze Calibration program (CALIBRATE.EXE), which starts and terminates */
/* its own Eyegaze thread.                                                  */
/* (See discussion of alternative calibration calls in the Eyegaze System   */
/* Programmer's Manual. NOTE: If using the double computer configuration,   */
/* spawning the external CALIBRATE.EXE is not an option.  EgCalibrate must  */
/* be used.  See Programmer's Manual)                                       */
/* P_WAIT causes the calibration program to terminate before this program   */
/* proceeds.                                                                */
   //_spawnl(P_WAIT, "Calibrate.exe", "Calibrate.exe", NULL);

/* Set the input control constants in stEgControl required for starting     */
/* the Eyegaze thread.                                                      */
   stEgControl.iNDataSetsInRingBuffer = EG_BUFFER_LEN;
                                 /* Tell Eyegaze the length of the Eyegaze  */
                                 /*   data ring buffer                      */
   stEgControl.bTrackingActive = FALSE;
                                 /* Tell Eyegaze not to begin image         */
                                 /*   processing yet (so no past gazepoint  */
                                 /*   data samples will have accumulated    */
                                 /*   in the ring buffer when the tracking  */
                                 /*   loop begins).                         */
   stEgControl.bEgCameraDisplayActive = FALSE;
                                 /* Tell Eyegaze not to display the full    */
                                 /*   camera image in a separate window.    */
   stEgControl.iScreenWidthPix  = iScreenWidth;
   stEgControl.iScreenHeightPix = iScreenHeight;
                                 /* Tell the image processing software what */
                                 /*   the physical screen dimensions are    */
                                 /*   in pixels.                            */
   stEgControl.iEyeImagesScreenPos = 1;

#if defined RUN_LOCALLY
   stEgControl.iCommType = EG_COMM_TYPE_LOCAL;
                                 /* We are calibrating on the local         */
                                 /*   computer, not over a serial or TCP/IP */
                                 /*   link.                                 */
#else
   stEgControl.iCommType = EG_COMM_TYPE_SOCKET;
   stEgControl.pszCommName = achIPAddress;
#endif

/* Create the Eyegaze image processing thread.                              */
   rc = EgInit(&stEgControl);
   if (rc != 0)
   {
      _TCHAR achErrorText[120];
      swprintf_s(achErrorText, _countof(achErrorText),
                 _T("Error %i Initializing Eyegaze"), rc);
      MessageBox(NULL, achErrorText,
                 _T("GazeDemo"), MB_ICONEXCLAMATION | MB_OK);
   }

   EgCalibrate2(&stEgControl, EG_CALIBRATE_NONDISABILITY_APP);

/* Push the GazeDemo program window up into the upper right corner of the   */
/* screen.                                                                  */
   SetWindowPos(hwndEyegaze,             // handle to window
                HWND_TOP,                // placement-order handle
                iScreenWidth-160,        // horizontal position
                0,                       // vertical position
                160,                     // width
                50,                      // height
                0);                      // Flags

/* Tell Eyegaze to start eyetracking, i.e. to begin image processing.       */
/* Note: No eyetracking takes place until this flag is set to true, and     */
/* eye image processing stops when this flag is reset to false.             */
   stEgControl.bTrackingActive = TRUE;

/* Loop for ever until the program is terminated.                           */
   for (EVER)
   {
/*-----------------------  Synchronize to Eyegaze --------------------------*/

/*    This code keeps the GazeDemo loop synchronized with the real-time     */
/*    Eyegaze image processing, i.e. looping at 60 Hz, but insures that all */
/*    gazepoint data samples are processed, even if the GazeDemo loop gets  */
/*    a little behind the real-time Eyegaze image processing.  Data         */
/*    buffers allow the GazeDemo loop to process all past gazepoint data    */
/*    samples even if the loop falls up to EG_BUFFER_LEN samples behind     */
/*    real time.                                                            */

/*    If the ring buffer has overflowed,                                    */
      if (stEgControl.iNBufferOverflow > 0)
      {
/*       The application program acts on data loss if necessary.            */
//         (appropriate application code)
      }

/*    The image processing software, running independently of this          */
/*    application, produces a new eyegaze data sample every 16.67 milli-    */
/*    seconds. If an unprocessed Eyegaze data sample is still available     */
/*    for processing, EgGetData() returns immediately, allowing the         */
/*    application to catch up with the Eyegaze image processing.  If the    */
/*    next unprocessd sample has not yet arrived, EgGetData blocks until    */
/*    data is available and then returns.  This call effectively puts the   */
/*    application to sleep until new Eyegaze data is available to be        */
/*    processed.                                                            */
      iVis = EgGetData(&stEgControl);

/*-------------------- Process Next Eyegaze Data Sample --------------------*/

/*    Compute the smoothed gazepoint, averaging over the last 12 samples.   */
      SmoothGazepoint(stEgControl.pstEgData->bGazeVectorFound,
                      stEgControl.pstEgData->iIGaze,
                      stEgControl.pstEgData->iJGaze,
                      &iIGazeSmooth, &iJGazeSmooth, 12);

/*    If the gazepoint was found this iteration,                            */
      if (stEgControl.pstEgData->bGazeVectorFound == TRUE)
      {
/*        Move the mouse to the smoothed gazpoint location.                 */
          mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,
                     iIGazeSmooth*65535/iScreenWidth,
                     iJGazeSmooth*65535/iScreenHeight, 0,  0);
      }
   }

   return 0;
   if (pvoid); // quiet the compiler.
}
/***************************************************************************/
void SmoothGazepoint(int iEyeFound,
                     int iIGaze, int iJGaze,
                     int *iIGazeSmooth, int *iJGazeSmooth,
                     int iNSmoothPoints)

/* This function smooths the gazepoint by averaging the all the valid       */
/* gazepoints within the last iNSmoothPoints.                               */

{
   #define SMOOTH_BUF_LEN   60

   SINT  iIGazeSave[SMOOTH_BUF_LEN];    /* past gazepoint I buffer          */
   SINT  iJGazeSave[SMOOTH_BUF_LEN];    /* past gazepoint J buffer          */
   SINT  iEyeFoundSave[SMOOTH_BUF_LEN]; /* buffer of past eye-found flags   */
   SINT  iBufIndex;                     /* buffer index of the present      */
                                        /*   gazepoint                      */
                                        /*   -- varies from 0 to            */
                                        /*      N_SMOOTH_BUF_LEN            */
   int   iIGazeSum;                     /* gazepoint summations used for    */
   int   iJGazeSum;                     /*   averaging                      */
   int   i;                             /* past sample index                */
   int   n;                             /* buffer index                     */
   int   iNAvgPoints;                   /* number of actual gazepoints      */
                                        /*   averaged                       */

/* Make sure the number of points we're asked to smooth is at least one     */
/* but not more than the smooth buffer size.                                */
   if (iNSmoothPoints < 1)             iNSmoothPoints = 1;
   if (iNSmoothPoints > SMOOTH_BUF_LEN)  iNSmoothPoints = SMOOTH_BUF_LEN;

/* Increment the buffer index for the new gazepoint.                        */
   iBufIndex++;
   if (iBufIndex >= SMOOTH_BUF_LEN) iBufIndex = 0;

/* Record the newest gazepoint in the buffer.                               */
   iEyeFoundSave[iBufIndex] = iEyeFound;
   iIGazeSave[iBufIndex]    = iIGaze;
   iJGazeSave[iBufIndex]    = iJGaze;

/* Initialize the average summations.                                       */
   iNAvgPoints = 0;
   iIGazeSum   = 0;
   iJGazeSum   = 0;

/* Loop through the past iNSmoothPoints.                                    */
   for (i = 0; i < iNSmoothPoints; i++)
   {
/*    Find the buffer index of the ith prior point.                         */
      n = iBufIndex - i;
      if (n < 0) n += SMOOTH_BUF_LEN;

/*    If the eye was found on the ith prior sample,                         */
      if (iEyeFoundSave[n] == TRUE)
      {
/*       Accumulate the average summations.                                 */
         iNAvgPoints++;
         iIGazeSum += iIGazeSave[n];
         iJGazeSum += iJGazeSave[n];
      }
   }

/* If there were one or more valid gazepoints during the last               */
/* iNSmoothPoints,                                                          */
   if (iNAvgPoints > 0)
   {
/*    Set the smoothed gazepoint to the average of the valid gazepoints     */
/*    collected during that period.                                         */
      *iIGazeSmooth = iIGazeSum / iNAvgPoints;
      *iJGazeSmooth = iJGazeSum / iNAvgPoints;
   }

/* Otherwise, if no valid gazepoints were collected during the last         */
/* iNSmoothPoints, leave the smoothed gazepoint alone.                      */
/* (no code)                                                                */
}
/****************************************************************************/

