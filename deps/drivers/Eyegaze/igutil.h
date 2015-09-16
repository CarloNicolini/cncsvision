/****************************************************************************

File Name:       IGUTIL.H
Program Name:    Eyegaze Utility Function Prototypes and Constant Definitions

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

*****************************************************************************/
/* PROTOTYPES FOR EYEGAZE UTILITY FUNCTIONS:                                */
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

void  SmoothGazepoint(int iEyeFound,
                      int iIGaze, int iJGaze,
                      int *iIGazeSmooth, int *iJGazeSmooth,
                      int iNSmoothPoints);          /* In SMTHGAZE.C        */

/****************************************************************************/
int   IkeysGetGazeCoordinates(int *iIGaze, int *iJGaze);

void  SendKeystrokeToEyegaze(int ch);
int   LctNNewKbdCharsAvailable(int iCallingFunction);
int   LctGetKbdChar(int iCallingFunction);
int   LctReadLastKbdChar(void);
int   lctScreenWidthPix(void);
int   lctScreenHeightPix(void);

/****************************************************************************/
#ifdef __cplusplus
}          /* Assume C declarations for C++ */
#endif  /* __cplusplus */
/****************************************************************************/

