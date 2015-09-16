/****************************************************************************/
/* LctCalib.h                                                               */
/****************************************************************************/
#ifndef  LCTCALIB_H_INCLUDED
#define  LCTCALIB_H_INCLUDED
/****************************************************************************/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

struct _LctCalInputs
{
   BOOL bEgWorldIsMonitor;           /* flag indicatining whether the       */
                                     /*   Eyegaze ststem's current World    */
                                     /*   frame is a monitor display, where */
                                     /*   the gaze points can be            */
                                     /*   programatically displayed, or     */
                                     /*   some other type of environment    */
   BOOL   bStandAlone;               /* flag indicating whether the         */
                                     /*   calibration procedure is being    */
                                     /*   run stand alone or by an          */
                                     /*   application program               */
   BOOL   bFullCalibration;          /* flag indicating whether this        */
                                     /*   this calibration process          */
                                     /*   includes the calibration of the   */
                                     /*   6 MonToGim transform parameters   */
                                     /*   for use in the NominalCal.dat     */
                                     /*   file.                             */
                                     /*   Used only with Eyefollower        */
                                     /*   This option has not been used     */
                                     /*   since about 2002, when we found   */
                                     /*   it was better to measure the      */
                                     /*   MonToGim parameters manually.     */
   int    iEgScreenWidthPix;         /* Physical screen dimensions, pixels. */
   int    iEgScreenHeightPix;        /* Physical screen dimensions, pixels. */
   int    iEgWindowWidthPix;         /* Window width we're calibrating in.  */
   int    iEgWindowHeightPix;        /* Window height we're calibrating in. */
   int    iEgWindowHorzOffsetPix;    /* Window offset from screen's 0,0.    */
   int    iEgWindowVertOffsetPix;    /* Window offset from screen's 0,0.    */
   int    *piObserverWindowWidthPix; /* Pointer to server window width      */
   int    *piObserverWindowHeightPix;/* Pointer to server window height     */
   char   achComPort[15];
   USHORT usSerialIRQ;
   void   *hfComm;                   /* Comm handle.  Used w/ 2pc option    */
   int    iCommType;                 /* Set to the communication method:    */
                                     /*   EG_COMM_TYPE_LOCAL,       Single  */
                                     /*   EG_COMM_TYPE_SOCKET,      Double  */
   int    iCalEgOrEcsType;           /* EG_CALIBRATE_DISABILITY_APP or      */
                                     /* EG_CALIBRATE_NONDISABILITY_APP      */
   int    iFlags;                    /* Operational flags:                  */
                                     /*   CAL_FLAG_SEND_IMAGES              */
};

   #define CAL_FLAG_SEND_IMAGES 0x00000001

void LctCalibrate(
        struct _stEgControl  *pstEgControl,
        struct _LctCalInputs *pstLctCalInputs);
void SendKeyCommandToCalibrate(int iKeyCommand);
void CalibrateDisplayEyeImages(BOOL bDisplayStateIn);

#ifdef __cplusplus
}          /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/****************************************************************************/
#endif  // LCTCALIB_H_INCLUDED
/****************************************************************************/

