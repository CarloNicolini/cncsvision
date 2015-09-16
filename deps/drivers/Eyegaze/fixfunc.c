/*****************************************************************************

File Name:       FIXFUNC.C
Program Name:    Eye Fixation Analysis Functions

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

Date Created:    10/20/89
                 04/12/95 modified: turned into collection of functions

*****************************************************************************/
/* DIAGNOSTIC CONTROLS:                                                     */

#if defined _DEBUG
   #define NO_DIAG_FIXFUNC
   #define NO_PRINT_RINGS
#endif

/****************************************************************************/
/* FUNCTION, VARIABLE AND CONSTANT DEFINITONS:                              */

#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <process.h>
#include <lctypdef.h>
#include <lctsupt.h>
#include <fixfunc.h>
#include <crtdbg.h>                 /* debug tools                          */

#define RING_SIZE    121            /* length of the delay line in          */
                                    /*   DetectFixation() --                */
                                    /*   should be greater than             */
                                    /*   iMinimumFixSamples                 */
#define NEW_FIX        0            /* the 3 types of fixations tracked     */
#define PRES_FIX       1            /*   by the algorithm                   */
#define PREV_FIX       2            /*                                      */

/****************************************************************************/
/* FUNCTION PROTOTYPES:                                                     */

void  ResetFixationHypothesis(
         int iNewPresOrPrev);
void  StartFixationHypothesisAtGazepoint(
         int iNewPresOrPrev, float fXGaze, float fYGaze);
void  UpdateFixationHypothesis(int iNewPresOrPrev,
         float fXGaze, float fYGaze, int iMininumFixSamples);
void  TestPresFixHypAndUpdateRingBuffer(int iMinimumFixSamples);
float CalcGazeDeviationFromFix(
         int iNewPresOrPrev, float fXGaze, float fYGaze);
void  TestPresentFixationHypothesis(int iMinimumFixSamples);
void  MoveNewFixToPresFix(void);
void  DeclarePresentFixationComplete(int iMinimumFixSamples);

#if defined DIAG_FIXFUNC
   void WriteDiagDataFileHeader(
           int   iMinimumFixSamples,
           float fGazeDeviationThreshold);
   void RecordDiagnosticData(
           BOOL  bGazepointFound,
           float fXGaze,
           float fYGaze,
           float fGazeDeviationThreshold,
           int   iMinimumFixSamples,
           float fXFixation,
           float fYFixation,
           int   iSaccadeDurationDelayed,
           int   iFixDurationDelayed);

#endif   /* DIAG_FIXFUNC */

/****************************************************************************/
/* STRUCTURES DEFINITIONS:                                                  */

struct _stFix                   /* FIXATION DATA                            */
{
   int   iStartCount;           /* call count to DetectFixation() when the  */
                                /*   fixation started                       */
   int   iEndCount;             /* call count to DetectFixation() when the  */
                                /*   fixation ended                         */
   int   iNEyeFoundSamples;     /* number of eye-found gazepoint samples    */
                                /*   collected so far in the fixation       */
                                /*   hypothesis                             */
                                /*   NOTE: If iNEyeFoundSamples is 0, the   */
                                /*   fixation hypothesis does not exist,    */
                                /*   i.e. there is no gazepoint data to     */
                                /*   support an existence hypothesis for    */
                                /*   the fixation.                          */
   float fXSum;                 /* summations for calculation of average    */
   float fYSum;                 /*   fixation position                      */
   float fX;                    /* average coordinate of the eye fixation   */
   float fY;                    /*   point (user selected units)            */
   BOOL  bFixationVerified;     /* flag indicating whether the fixation     */
                                /*   hypothesis has been verified           */
};

struct _stRingBuf               /* RING BUFFER STORING PAST GAZEPOINT AND   */
                                /*   FIXATION-STATE VALUES:                 */
{
    int   iDfCallCount;         /* DetectFixation call-count at the time    */
                                /*   of the sample                          */ 
    float fXGaze;               /* gazepoint coordinate                     */
    float fYGaze;               /*                                          */
    BOOL  bGazeFound;           /* gazepoint found flag                     */
    int   iEyeMotionState;      /* state of the eye motion:                 */
                                /*   MOVING                                 */
                                /*   FIXATING                               */
                                /*   FIXATION_COMPLETED                     */
    float fXFix;                /* current fixation coordinate - includes   */
    float fYFix;                /*   the current gazepoint                  */
    float fGazeDeviation;       /* gazepoint deviation from the prior       */
                                /*   fixation coordinate                    */
    int   iSacDuration;         /* saccade duration                         */
    int   iFixDuration;         /* fixation duration                        */
};

/****************************************************************************/
/* GLOBAL FIXFUNC VARIABLES:                                                */

int   iDFCallCount;             /* number of times the DetectFixation()     */
                                /*   function has been called since it was  */
                                /*   initialized                            */
                                /*   (60ths or 120ths of a second,          */
                                /*   depending on eyetracking sample rate)  */
int   iNOutsidePresCircle;      /* number of successive gazepoint samples   */
                                /*   outside the present fixation's         */
                                /*   acceptance circle                      */
float fPresDr;                  /* difference between gazepoint and         */
                                /*   fixation (x, y, and radius)            */
float fNewDr;                   /* difference between gazepoint and         */
                                /*   fixation (x, y, and radius)            */
int   iMaxMissedSamples;        /* maximum number of successive gazepoint   */
                                /*   samples that may go untracked before   */
                                /*   a fixation is allowed to continue      */
int   iMaxOutSamples;           /* maximum number of successive gazepoint   */
                                /*   samples that may go outside the        */
                                /*   fixation acceptance circle             */
struct _stFix stFix[3];         /* prior, present and new fixations         */
                                /*   indexed by: NEW_FIX   0                */
                                /*               PRES_FIX  1                */
                                /*               PREV_FIX  2                */
struct _stRingBuf stRingBuf[RING_SIZE];
int   iCurrentRingIndex;        /* ring index of the current gaze sample    */
int   iRingIndexDelay;          /* ring index of the gaze sample taken      */
                                /*   iMinimumFixSamples ago                 */

#if defined DIAG_FIXFUNC
   FILE  *hfFixFile;
#endif   /* DIAG_FIXFUNC */

/****************************************************************************/
void InitFixation(int iMinimumFixSamples)
                                       /* minimum number of gaze samples    */
                                       /*   that can be considered a        */
                                       /*   fixation                        */
                                       /*   Note: if the input value is     */
                                       /*   is less than 3, the function    */
                                       /*   sets it to 3                    */

/* This function clears the previous, present and new fixation hypotheses,  */
/* and it initializes DetectFixation()'s internal ring buffers of prior     */
/* gazepoint data.  InitFixation() should be called prior to a sequence     */
/* of calls to DetectFixation().                                            */

{
/* Set the maximum allowable number of consecutive samples that may go      */
/* untracked within a fixation.                                             */
   iMaxMissedSamples = 3;

/* Set the maximum allowable number of consecutive samples that may go      */
/* outside the fixation acceptance circle.                                  */
   iMaxOutSamples = 1;

/* Initialize the internal ring buffer.                                     */
   for (iCurrentRingIndex = 0; iCurrentRingIndex < RING_SIZE; iCurrentRingIndex++)
   {
      stRingBuf[iCurrentRingIndex].iDfCallCount    =  0;
      stRingBuf[iCurrentRingIndex].fXGaze          =  0.0F;
      stRingBuf[iCurrentRingIndex].fYGaze          =  0.0F;
      stRingBuf[iCurrentRingIndex].bGazeFound      =  FALSE;
      stRingBuf[iCurrentRingIndex].iEyeMotionState =  MOVING;
      stRingBuf[iCurrentRingIndex].fXFix           =  0.0F;
      stRingBuf[iCurrentRingIndex].fYFix           =  0.0F;
      stRingBuf[iCurrentRingIndex].fGazeDeviation  = -0.1F;
      stRingBuf[iCurrentRingIndex].iSacDuration    =  0;
      stRingBuf[iCurrentRingIndex].iFixDuration    =  0;
   }
   iCurrentRingIndex = 0;
   iRingIndexDelay = RING_SIZE - iMinimumFixSamples;

/* Set the number of times the DetectFixation() has been called since       */
/* initialization to zero, and initialize the previous fixation end count   */
/* so the first saccade duration will be a legitimate count.                */
   iDFCallCount              = 0;
   stFix[PREV_FIX].iEndCount = 0;

/* Reset the present fixation data.                                         */
   ResetFixationHypothesis(PRES_FIX);

/* Reset the new fixation data.                                             */
   ResetFixationHypothesis(NEW_FIX);
}
/****************************************************************************/
int DetectFixation(
                                       /* INPUT PARAMETERS:                 */
       int   bGazepointFound,          /* flag indicating whether or not    */
                                       /*   the image processing algo       */
                                       /*   detected the eye and computed   */
                                       /*   a valid gazepoint (TRUE/FALSE)  */
       float fXGaze,                   /* present gazepoint                 */
       float fYGaze,                   /*   (user specified units)          */
       float fGazeDeviationThreshold,  /* distance that a gazepoint may     */
                                       /*   vary from the average fixation  */
                                       /*   point and still be considered   */
                                       /*   part of the fixation            */
                                       /*   (user specified units)          */
       int   iMinimumFixSamples,       /* minimum number of gaze samples    */
                                       /*   that can be considered a        */
                                       /*   fixation                        */
                                       /*   Note: if the input value is     */
                                       /*   is less than 3, the function    */
                                       /*   sets it to 3                    */

                                       /* OUTPUT PARAMETERS:                */
                                       /* Delayed Gazepoint data with       */
                                       /*   fixation annotations:           */
       int   *pbGazepointFoundDelayed,
                                       /* sample gazepoint-found flag,      */
                                       /*   iMinimumFixSamples ago          */
       float *pfXGazeDelayed,          /* sample gazepoint coordinates,     */
       float *pfYGazeDelayed,          /*   iMinimumFixSamples ago          */
       float *pfGazeDeviationDelayed,
                                       /* deviation of the gaze from the    */
                                       /*   present fixation,               */
                                       /*   iMinimumFixSamples ago          */

                                       /* Fixation data - delayed:          */
       float *pfXFixDelayed,           /* fixation point as estimated       */
       float *pfYFixDelayed,           /*   iMinimumFixSamples ago          */
       int   *piSaccadeDurationDelayed,
                                       /* duration of the saccade           */
                                       /*   preceeding the preset fixation  */
                                       /*   (samples)                       */
       int   *piFixDurationDelayed)    /* duration of the present fixation  */
                                       /*   (samples)                       */

/* RETURN VALUES - Eye Motion State:                                        */
/*                                                                          */
/*  MOVING               0   The eye was in motion, iMinimumFixSamples ago. */
/*  FIXATING             1   The eye was fixating, iMinimumFixSamples ago.  */
/*  FIXATION_COMPLETED   2   A completed fixation has just been detected,   */
/*                             iMinimumFixSamples ago.                      */
/*                             With respect to the sample that reports      */
/*                             FIXATION_COMPLETED, the fixation started     */
/*                             (iMinimumFixSamples +                        */
/*                              *piSaccadeDurationDelayed)                  */
/*                             ago and ended iMinimumFixSamples ago.        */
/*                             Note, however, that because of the           */
/*                             (approximately 2-field) measurement latency  */
/*                             in the eyetracking measurement, the start    */
/*                             and end times occurred (iMinimumFixSamples   */
/*                             + *piSaccadeDurationDelayed + 2) and         */
/*                             (iMinimumFixSamples + 2) ago with respect    */
/*                             to real time now.                            */
/*                                                                          */
/* Include FIXFUNC.H for function prototype and above constant definitions. */
/*                                                                          */
/* SUMMARY                                                                  */
/*                                                                          */
/*    This function converts a series of uniformly-sampled (raw) gaze       */
/* points into a series of variable-duration saccades and fixations.        */
/* Fixation analysis may be performed in real time or after the fact.  To   */
/* allow eye fixation analysis during real-time eyegaze data collection,    */
/* the function is designed to be called once per sample.  When the eye     */
/* is in motion, ie during saccades, the function returns 0 (MOVING).       */
/* When the eye is still, ie during fixations, the function returns 1       */
/* (FIXATING).  Upon the detected completion of a fixation, the function    */
/* returns 2 (FIXATION_COMPLETED) and produces:                             */
/*   a) the time duration of the saccade between the last and present       */
/*      eye fixation (eyegaze samples)                                      */
/*   b) the time duration of the present, just completed fixation           */
/*      (eyegaze samples)                                                   */
/*   c) the average x and y coordinates of the eye fixation                 */
/*      (in user defined units of fXGaze and fYGaze)                        */
/* Note: Although this function is intended to work in "real time", there   */
/* is a delay of iMinimumFixSamples in the filter which detects the         */
/* motion/fixation condition of the eye.                                    */
/*                                                                          */
/* PRINCIPLE OF OPERATION                                                   */
/*                                                                          */
/*    This function detects fixations by looking for sequences of gaze-     */
/* point measurements that remain relatively constant.  If a new gazepoint  */
/* lies within a circular region around the running average of an on-going  */
/* fixation, the fixation is extended to include the new gazepoint.         */
/* (The radius of the acceptance circle is user specified by setting the    */
/* value of the function argument fGazeDeviationThreshold.)                 */
/*    To accommodate noisy eyegaze measurements, a gazepoint that exceeds   */
/* the deviation threshold is included in an on-going fixation if the       */
/* subsequent gazepoint returns to a position within the threshold.         */
/*    If a gazepoint is not found, during a blink for example, a fixation   */
/* is extended if a) the next legitimate gazepoint measurement falls within */
/* the acceptance circle, and b) there are less than iMinimumFixSamples     */
/* of successive missed gazepoints.  Otherwise, the previous fixation       */
/* is considered to end at the last good gazepoint measurement.             */
/*                                                                          */
/* UNITS OF MEASURE                                                         */
/*                                                                          */
/*    The gaze position/direction may be expressed in any units (e.g.       */
/* millimeters, pixels, or radians), but the filter threshold must be       */
/* expressed in the same units.                                             */
/*                                                                          */
/* INITIALIZING THE FUNCTION                                                */
/*                                                                          */
/*    Prior to analyzing a sequence of gazepoint data, the InitFixation     */
/* function should be called to clear any previous, present and new         */
/* fixations and to initialize the ring buffers of prior gazepoint data.    */
/*                                                                          */
/* PROGRAM NOTES                                                            */
/*                                                                          */
/* For purposes of describing an ongoing sequence of fixations, fixations   */
/* in this program are referred to as "previous", "present", and "new".     */
/* The present fixation is the one that is going on right now, or, if a     */
/* new fixation has just started, the present fixation is the one that      */
/* just finished.  The previous fixation is the one immediatly preceeding   */
/* the present one, and a new fixation is the one immediately following     */
/* the present one.  Once the present fixation is declared to be completed, */
/* the present fixation becomes the previous one, the new fixation becomes  */
/* the present one, and there is not yet a new fixation.                    */

/*--------------------------------------------------------------------------*/
{
   int   iPastRingIndex;

   #if defined DIAG_FIXFUNC 
     fPresDr = -0.1F;
     fNewDr  = -0.1F;
   #endif   /* DIAG_FIXFUNC */

/* Make sure the minimum fix time is at least 3 samples.                    */
   if (iMinimumFixSamples < 3) iMinimumFixSamples = 3;

/* Make sure the ring size is large enough to handle the delay.             */
   if (iMinimumFixSamples >= RING_SIZE)
   {
      _tprintf(_TEXT("iMinimumFixSamples %i >= RING_SIZE %i\n"),
                      iMinimumFixSamples,      RING_SIZE);
      _tprintf(_TEXT("Press any key to terminate..."));
      _getch();
      exit(99);
   }

/* Increment the call count, the ring index, and the delayed ring index.    */
   iDFCallCount++;
   iPastRingIndex = iCurrentRingIndex;
   iCurrentRingIndex++;
   if (iCurrentRingIndex >= RING_SIZE) iCurrentRingIndex = 0;
   iRingIndexDelay = iCurrentRingIndex - iMinimumFixSamples;
   if (iRingIndexDelay < 0) iRingIndexDelay += RING_SIZE;

   _ASSERTE((iCurrentRingIndex >= 0) && (iCurrentRingIndex < RING_SIZE));
   _ASSERTE((iRingIndexDelay >= 0) && (iRingIndexDelay < RING_SIZE));

/* Update the ring buffer of past gazepoints.                               */
   stRingBuf[iCurrentRingIndex].iDfCallCount    = iDFCallCount;
   stRingBuf[iCurrentRingIndex].fXGaze          = fXGaze;
   stRingBuf[iCurrentRingIndex].fYGaze          = fYGaze;
   stRingBuf[iCurrentRingIndex].bGazeFound      = bGazepointFound;

/* Initially assume the eye is moving.                                      */
/* Note: These values are updated during the processing of this and         */
/* subsequent gazepoints.                                                   */
   stRingBuf[iCurrentRingIndex].iEyeMotionState = MOVING;
   stRingBuf[iCurrentRingIndex].fXFix           = -0.0F;
   stRingBuf[iCurrentRingIndex].fYFix           = -0.0F;
   stRingBuf[iCurrentRingIndex].fGazeDeviation  = -0.1F;
   stRingBuf[iCurrentRingIndex].iFixDuration    = 0;

/* The following code keeps the saccade duration increasing during non      */
/* fixation periods.                                                        */
/* If the eye was moving during the last sample,                            */
   if (stRingBuf[iPastRingIndex].iEyeMotionState == MOVING)
   {
/*    Increment the saccade count from the last sample.                     */
      stRingBuf[iCurrentRingIndex].iSacDuration = stRingBuf[iPastRingIndex].iSacDuration + 1;
   }
/* Otherwise, if the eye was fixating during the last sample,               */
   else
   {
/*    Reset the saccade count to 1, initially assuming this sample will     */
/*    be the first of sample of an upcoming saccade.                        */
      stRingBuf[iCurrentRingIndex].iSacDuration = 1;
   }

/*- - - - - - - - - - - - - Process Tracked Eye  - - - - - - - - - - - - - -*/

/* A) If the eye's gazepoint was successfully measured this sample,         */
   if (bGazepointFound == TRUE)
   {
/*    A1 B) If a present fixation hypothesis exists,                        */
      if (stFix[PRES_FIX].iNEyeFoundSamples > 0)
      {
/*       B1) Compute the deviation of the gazepoint from the present        */
/*       fixation.                                                          */
         fPresDr = CalcGazeDeviationFromFix(PRES_FIX, fXGaze, fYGaze);

/*       C) If the gazepoint is within the present fixation's accepatance   */
/*       circle,                                                            */
         if (fPresDr <= fGazeDeviationThreshold)
         {
/*          C1) Update the present fixation hypothesis, test if the         */
/*          fixating is real, and if so, designate the appropriate entries  */
/*          in the ring buffer as fixation points.                          */
            UpdateFixationHypothesis(PRES_FIX,
               fXGaze, fYGaze, iMinimumFixSamples);
         }

/*       Otherwise, if the point is outside the present fixation's          */
/*       acceptance circle,                                                 */
         else   // if (fPresDr > fGazeDeviationThreshold)
         {
/*          C2) Increment the number of successive gazepoint samples        */
/*          outside the present fixation's acceptance circle.               */
            iNOutsidePresCircle++;

/*          D) If the number of successive gazepoints outside the present   */
/*          fixation's acceptance circle has not exceeded its maximum,      */
            if (iNOutsidePresCircle <= iMaxOutSamples)
            {
/*             D1) Incorporate this gazepoint into the NEW fixation         */
/*             hypothesis:                                                  */
/*             E) If a new fixation hypothesis has already been started,    */
               if (stFix[NEW_FIX].iNEyeFoundSamples > 0)
               {
/*                E1) Compute the gazepoint's deviation from the new        */
/*                fixation.                                                 */
                  fNewDr = CalcGazeDeviationFromFix(NEW_FIX, fXGaze, fYGaze);

/*                F) If the new gazepoint falls within the new fix,         */
                  if (fNewDr <= fGazeDeviationThreshold)
                  {
/*                   F1) Update the new fixation hypothesis, check if       */
/*                   there are enough samples to declare that the eye is    */
/*                   fixating,and if so, declare the appropriate ring       */
/*                   buffer entries to be fixating.                         */
                     UpdateFixationHypothesis(NEW_FIX,
                        fXGaze, fYGaze, iMinimumFixSamples);
                  }

/*                Otherwise, if the point is outside the new fix,           */
                  else   // if (fNewDr > fGazeDeviationThreshold)
                  {
/*                   F2) Reset the new fixation at this new gazepoint.      */
                     StartFixationHypothesisAtGazepoint(NEW_FIX, fXGaze, fYGaze);
                  }
               }

/*             Otherwise, If a new fix hypothesis has not been started,     */
               else  // if (stFix[NEW_FIX].iNEyeFoundSamples == 0)
               {
/*                E2) Start a new fixation hypothesis at this gazepoint.    */
                  StartFixationHypothesisAtGazepoint(NEW_FIX, fXGaze, fYGaze);
               }
            }

/*          Otherwise, if too many successive gazepoint samples have        */
/*          gone outside the present fixation's acceptance circle,          */
            else  // if (iNOutsidePresCircle > iMaxOutSamples)
            {
/*             D2) The present fixation hypothesis must either be declared  */
/*             complete or rejected:                                        */
/*             G) If the present fixation hypothesis has been verified,     */
               if (stFix[PRES_FIX].bFixationVerified == TRUE)
               {
/*                G1) Declare the present fixation to be completed at the   */
/*                last good sample, move the present fixation to the        */
/*                prior, and move the new fixation to the present.          */
                  DeclarePresentFixationComplete(iMinimumFixSamples);
               }

/*             Otherwise, if the present fixation does not have enough      */
/*             good gaze samples to qualify as a real fixation,             */
               else  // if (stFix[PRES_FIX].bFixationVerified == FALSE)
               {
/*                G2) Reject the present fixation hypothesis by replacing   */
/*                it with the new fixation hypothesis (which may or may     */
/*                not exist at this time).                                  */
                  MoveNewFixToPresFix();
               }

/*             H) If there is a present fixation hypothesis,                */
               if (stFix[PRES_FIX].iNEyeFoundSamples > 0)
               {
/*                H1) Compute the deviation of the gazepoint from the now   */
/*                present fixation.                                         */
                  fPresDr = CalcGazeDeviationFromFix(PRES_FIX, fXGaze, fYGaze);

/*                I) If the gazepoint is within the now present fixation's  */
/*                acceptance circle,                                        */
                  if (fPresDr <= fGazeDeviationThreshold)
                  {
/*                   I1) Update the present fixation data, check if there   */
/*                   are enough samples to declare that the eye is fix-     */
/*                   ating, and if so, declare the appropriate ring buffer  */
/*                   entries to be fixating.                                */
                     UpdateFixationHypothesis(PRES_FIX, 
                        fXGaze, fYGaze, iMinimumFixSamples);
                  }

/*                Otherwise, if the gazepoint falls outside the present     */
/*                fixation,                                                 */
                  else   // if (fPresDr > fGazeDeviationThreshold)
                  {
/*                   I2) Start a new fixation hypothesis at this gazepoint. */
                     StartFixationHypothesisAtGazepoint(NEW_FIX, fXGaze, fYGaze);
                  }
               }

/*             Otherwise, if there is no present fixation hypothesis,       */
               else  // if (stFix[PRES_FIX].iNEyeFoundSamples == 0)
               {
/*                H2) Start a present fixation at this gazepoint.           */
                  StartFixationHypothesisAtGazepoint(PRES_FIX, fXGaze, fYGaze);
               }
            }
         }
      }

/*    Otherwise, if there is not a present fixation hypothesis going,       */
      else  // if (stFix[PRES_FIX].iNEyeFoundSamples == 0)
      {
/*       B2) Start the present fixation hypothesis at the gazepoint and     */
/*       reset the new fixation hypothesis.                                 */
         StartFixationHypothesisAtGazepoint(PRES_FIX, fXGaze, fYGaze);
      }
   }

/*- - - - - - - - - - - - - Process Untracked Eye  - - - - - - - - - - - - -*/

/* Otherwise, if the eye's gazepoint was not successfully measured this     */
/* sample,                                                                  */
   else   //  if (bGazepointFound == FALSE) 
   {
/*    A2 J) If there is still time to update the present fixation, i.e.     */
/*    if the last good sample in the present fixation occurred within       */
/*    the permissible time gap,                                             */
      if (iDFCallCount - stFix[PRES_FIX].iEndCount <= iMaxMissedSamples)
      {
/*       J1) No action is to be taken (no code).                            */
      }

/*    Otherwise, if too much time has passed since the last good gazepoint  */
/*    in the present fixation,                                              */
      else  // if (iDFCallCount - stFix[PRES_FIX].iEndCount > iMaxMissedSamples)
      {
/*       J2) The present fixation hypothesis must be declared complete      */
/*       or rejected:                                                       */
/*       K) If the present fixation hypothesis has been verified,           */
/*       as a real fixation,                                                */
         if (stFix[PRES_FIX].bFixationVerified == TRUE)
         {
/*          K1) Declare the present fixation to be completed at the last    */
/*          good sample, move the present fixation to the prior, and move   */
/*          the new fixation to the present.                                */
            DeclarePresentFixationComplete(iMinimumFixSamples);
         }

/*       Otherwise, if the present fixation hypothesis is not verified,     */
         else  // if (stFix[PRES_FIX].bFixationVerified == FALSE)
         {
/*          K2) Reject the present fixation hypothesis by replacing it      */
/*          with the new fixation hypothesis (which may or may not exist    */
/*          at this time).                                                  */
            MoveNewFixToPresFix();
         }
      }
   }

/*---------------------------- Pass Data Back ------------------------------*/

   _ASSERTE((iRingIndexDelay >= 0) && (iRingIndexDelay < RING_SIZE));

/* Pass the delayed gazepoint data, with the relevant saccade/fixation      */
/* data, back to the calling function.                                      */
   *pfXGazeDelayed          = stRingBuf[iRingIndexDelay].fXGaze;
   *pfYGazeDelayed          = stRingBuf[iRingIndexDelay].fYGaze;
   *pbGazepointFoundDelayed = stRingBuf[iRingIndexDelay].bGazeFound;
   *pfXFixDelayed           = stRingBuf[iRingIndexDelay].fXFix;
   *pfYFixDelayed           = stRingBuf[iRingIndexDelay].fYFix;
   *pfGazeDeviationDelayed  = stRingBuf[iRingIndexDelay].fGazeDeviation;
   *piSaccadeDurationDelayed= stRingBuf[iRingIndexDelay].iSacDuration;
   *piFixDurationDelayed    = stRingBuf[iRingIndexDelay].iFixDuration;

/* Record diagnostic data.                                                  */
   #if defined DIAG_FIXFUNC 
     RecordDiagnosticData(bGazepointFound, fXGaze, fYGaze,
        fGazeDeviationThreshold, iMinimumFixSamples,
        *pfXFixDelayed, *pfYFixDelayed,
        *piSaccadeDurationDelayed, *piFixDurationDelayed);
   #endif   /* DIAG_FIXFUNC */

   #if defined DIAG_FIXFUNC
     fprintf(hfFixFile, "CallCnt %3i %3i   RingI %3i   Dev %5.1f   NFoundSamp pres %2i  new %2i\n",
        iDFCallCount,
        iDFCallCount - iMinimumFixSamples,
        iCurrentRingIndex,
        stRingBuf[iCurrentRingIndex].fGazeDeviation,
        stFix[PRES_FIX].iNEyeFoundSamples,
        stFix[NEW_FIX].iNEyeFoundSamples);
     fflush(stdout);
   #endif

/* Return the eye motion/fixation state for the delayed point.              */
   return stRingBuf[iRingIndexDelay].iEyeMotionState;
}
/****************************************************************************/
void ResetFixationHypothesis(int iNewPresOrPrev)

/* This function resets the argument NewPresOrPrev fixation, i.e.           */
/* declares it nonexistent.                                                 */

{
   stFix[iNewPresOrPrev].iStartCount       = 0;
   stFix[iNewPresOrPrev].iEndCount         = 0;
   stFix[iNewPresOrPrev].iNEyeFoundSamples = 0;  // 0 declares fix hypothesis does not yet exist
   stFix[iNewPresOrPrev].fXSum             = 0.0F;
   stFix[iNewPresOrPrev].fYSum             = 0.0F;
   stFix[iNewPresOrPrev].fX                = 0.0F;
   stFix[iNewPresOrPrev].fY                = 0.0F;
   stFix[iNewPresOrPrev].bFixationVerified = FALSE;

/* If resetting the present fixation,                                       */
   if (iNewPresOrPrev == PRES_FIX)
   {
/*    Reset the number of consecutive gazepoints that have gone outside     */
/*    the present fixation's acceptance circle.                             */
      iNOutsidePresCircle = 0;
   }
}
/****************************************************************************/
void StartFixationHypothesisAtGazepoint(int iNewPresOrPrev, float fXGaze, float fYGaze)

/* This function starts the argument NewPresOrPrev fixation at the          */
/* argument gazepoint and makes sure there is no new fixation hypothesis.   */

{
/* Start the present fixation at the argument gazepoint.                    */
   stFix[iNewPresOrPrev].iNEyeFoundSamples = 1;
   stFix[iNewPresOrPrev].fXSum             = fXGaze;
   stFix[iNewPresOrPrev].fYSum             = fYGaze;
   stFix[iNewPresOrPrev].fX                = fXGaze;
   stFix[iNewPresOrPrev].fY                = fYGaze;
   stFix[iNewPresOrPrev].iStartCount       = iDFCallCount;
   stFix[iNewPresOrPrev].iEndCount         = iDFCallCount;
   stFix[iNewPresOrPrev].bFixationVerified = FALSE;

/* If starting the present fixation,                                        */
   if (iNewPresOrPrev == PRES_FIX)
   {
/*    Reset the number of consecutive gazepoints that have gone outside     */
/*    the present fixation's acceptance circle.                             */
      iNOutsidePresCircle = 0;

/*    Make sure there is no new fixation.                                   */
      ResetFixationHypothesis(NEW_FIX);
   }
}
/****************************************************************************/
void UpdateFixationHypothesis(int iNewPresOrPrev, 
       float fXGaze, float fYGaze, int iMinimumFixSamples)

/* This function updates the argument NewPresOrPrev fixation with the       */
/* argument gazepoint, checks if there are enough samples to declare that   */
/* the eye is now fixating, and if so, declares the appropriate ring        */
/* buffer entries to be fixating.                                           */
/* The function also makes sure there is no hypothesis for a new fixation.  */

{
/* Update the argument fixation with the argument gazepoint.                */
   stFix[iNewPresOrPrev].iNEyeFoundSamples++;
   stFix[iNewPresOrPrev].fXSum    += fXGaze;
   stFix[iNewPresOrPrev].fYSum    += fYGaze;
   stFix[iNewPresOrPrev].fX        = stFix[iNewPresOrPrev].fXSum /
                                     stFix[iNewPresOrPrev].iNEyeFoundSamples;
   stFix[iNewPresOrPrev].fY        = stFix[iNewPresOrPrev].fYSum /
                                     stFix[iNewPresOrPrev].iNEyeFoundSamples;
   stFix[iNewPresOrPrev].iEndCount = iDFCallCount;

/* If updating the present fixation,                                        */
   if (iNewPresOrPrev == PRES_FIX)
   {
/*    Reset the number of consecutive gazepoints that have gone outside     */
/*    the present fixation's acceptance circle.                             */
      iNOutsidePresCircle = 0;

/*    Test if there are enough samples in the present fixation hypothesis   */
/*    to declare that the eye is actually fixating, and if so declare the   */
/*    appropriate ring-buffer samples to be fixating.                       */
      TestPresFixHypAndUpdateRingBuffer(iMinimumFixSamples);

/*    There is no hypothesis for a new fixation.                            */
      ResetFixationHypothesis(NEW_FIX);
   }
}
/****************************************************************************/
void  TestPresFixHypAndUpdateRingBuffer(int iMinimumFixSamples)

/* Each time a gazepoint is added to the present fixation hypothesis,       */
/* this function tests whether the fixation hypothesis can be verified,     */
/* and if the fixation is real, the function updates the ring-buffer        */
/* history with the appropriate values.                                     */
/* Recall: The iEyeMoving value in the ring buffer's current index was      */
/* initialized to MOVING at the beginning of the call to DetectFixation().  */

{
   int  iRingPointOffset;        /* ring index offset with respect to the   */
                                 /*  current ring-buffer index              */
   int  iDumRingIndex;           /* dummy ring index                        */
   static int  siPreviousFixEndCount;

/* If the present fixation hypothesis has not previously been verified,     */
   if (stFix[PRES_FIX].bFixationVerified == FALSE)
   {
/*    Test the hypothesis now.  If there are enough good eye samples to     */
/*    declare a real fixation,                                              */
      if (stFix[PRES_FIX].iNEyeFoundSamples >= iMinimumFixSamples)
      {
         int   iNFixSamples;     /* total number of samples in the          */
                                 /*   fixation to date                      */

/*       Declare the present fixation verified.                             */
         stFix[PRES_FIX].bFixationVerified = TRUE;

/*       Mark the fixation within the ring buffer:                          */
/*       Fill in all the samples in the ring buffer from the start          */
/*       through the end points of the present fixation:                    */
/*       Compute the total number of samples in the present fixation,       */
/*       including the good samples, any no-track samples and any "out"     */
/*       samples now decided to be included in the fixation.                */
/*       Note:  +1 to include both start and end samples.                   */
         iNFixSamples = stFix[PRES_FIX].iEndCount -
                        stFix[PRES_FIX].iStartCount + 1;

/*       Make sure the number of samples does not exceed the ring-buffer    */
/*       size.                                                              */
         _ASSERTE(iNFixSamples >  0);
         _ASSERTE(iNFixSamples <= RING_SIZE);
         if      (iNFixSamples >  RING_SIZE)
                  iNFixSamples =  RING_SIZE;

/*       Loop backwards through the ring buffer, starting with the current  */
/*       ring index, for the total number of fixation samples.              */
         for (iRingPointOffset = 0;
              iRingPointOffset < iNFixSamples;
              iRingPointOffset++)
         {
/*          Compute the ring index of the earlier point.                    */
            iDumRingIndex = iCurrentRingIndex - iRingPointOffset;
            if (iDumRingIndex < 0) iDumRingIndex += RING_SIZE;
            _ASSERTE((iDumRingIndex >= 0) && (iDumRingIndex < RING_SIZE));

/*          Declare the sample to be fixating at the currently computed     */
/*          fixation point.                                                 */
            stRingBuf[iDumRingIndex].iEyeMotionState = FIXATING;
            stRingBuf[iDumRingIndex].fXFix = stFix[PRES_FIX].fX;
            stRingBuf[iDumRingIndex].fYFix = stFix[PRES_FIX].fY;

/*          Set the ring buffer's entry for the saccade duration, i.e.      */
/*          the time from the end of the previous fixation to the start     */
/*          of the present fixation.                                        */
/*          Note: The saccade duration is the same for all ring index       */
/*          samples of this fixation.                                       */
            stRingBuf[iDumRingIndex].iSacDuration =
               stFix[PRES_FIX].iStartCount - stFix[PREV_FIX].iEndCount - 1;

/*          Set the ring buffer's entry for the fixation duration, i.e.     */
/*          the time from the start of the present fixation to the time     */
/*          indicated by the ring index.                                    */
/*          Note: The fixation duration changes (decreases) for earlier     */
/*          restored points.                                                */
            stRingBuf[iDumRingIndex].iFixDuration =
               stFix[PRES_FIX].iEndCount - iRingPointOffset -
               stFix[PRES_FIX].iStartCount + 1;
         }

         #if defined DIAG_FIXFUNC
            fprintf(hfFixFile, "  VerifyFixation    NFoundSamples %2i  iNFixSamples %2i\n",
               stFix[PRES_FIX].iNEyeFoundSamples,
               iNFixSamples);
         #endif

/*       Save the fixation end count for use next time this function is     */
/*       called.                                                            */
         siPreviousFixEndCount = stFix[PRES_FIX].iEndCount;
      }

/*    Otherwise, if there are not enough good eye samples to declare a      */
/*    real fixation,                                                        */
      else  // if (stFix[PRES_FIX].iNEyeFoundSamples < iMinimumFixSamples)
      {
/*       Leave the ring buffer alone.  (no code)                            */
      }
   }

/* Otherwise, if the present fixation hypothesis has been verified          */
/* previously,                                                              */
   else  // if (stFix[PRES_FIX].bFixationVerified == TRUE)
   {
      int   iNNewSamples;     /* number of new samples to be added to the   */
                              /*   ring-buffer's fixation                   */

/*    Extend the fixation within the ring buffer.                           */
/*    Mark all the new samples in the ring buffer since the previous good   */
/*    sample, including any missed or out points, as fixating:              */
/*    Compute the number of new samples that have gone by since the         */
/*    present fixation's last good sample.                                  */
      iNNewSamples = iDFCallCount - siPreviousFixEndCount;

/*    Make sure the number of samples does not exceed the ring-buffer       */
/*    size.                                                                 */
      _ASSERTE(iNNewSamples >  0);
      _ASSERTE(iNNewSamples <= RING_SIZE);
      if      (iNNewSamples >  RING_SIZE)
               iNNewSamples =  RING_SIZE;

/*    Loop backwards through the ring buffer, starting with the current     */
/*    ring index, for the total number of fixation samples.                 */
      for (iRingPointOffset = 0;
           iRingPointOffset < iNNewSamples;
           iRingPointOffset++)
      {
/*       Compute the ring index of the earlier point.                       */
         iDumRingIndex = iCurrentRingIndex - iRingPointOffset;
         if (iDumRingIndex < 0) iDumRingIndex += RING_SIZE;

         _ASSERTE((iDumRingIndex >= 0) && (iDumRingIndex < RING_SIZE));

/*       Declare the sample to be fixating at the currently computed        */
/*       fixation point.                                                    */
         stRingBuf[iDumRingIndex].iEyeMotionState = FIXATING;
         stRingBuf[iDumRingIndex].fXFix = stFix[PRES_FIX].fX;
         stRingBuf[iDumRingIndex].fYFix = stFix[PRES_FIX].fY;
   
/*       Set the ring buffer's entry for the saccade duration, i.e. the     */
/*       time from the end of the previous fixation to the start of the     */
/*       present fixation.                                                  */
/*       Note: The saccade duration is the same for all ring index          */
/*       points.                                                            */
         stRingBuf[iDumRingIndex].iSacDuration =
            stFix[PRES_FIX].iStartCount - stFix[PREV_FIX].iEndCount - 1;
   
/*       Set the ring buffer's entry for the fixation duration, i.e. the    */
/*       time from the start of the present fixation to the time indicated  */
/*       by the ring index.                                                 */
/*       Note: The fixation duration changes (decreases) for earlier        */
/*       restored points.                                                   */
         stRingBuf[iDumRingIndex].iFixDuration =
            stFix[PRES_FIX].iEndCount - iRingPointOffset -
            stFix[PRES_FIX].iStartCount + 1;
      }

      #if defined DIAG_FIXFUNC
         fprintf(hfFixFile, "    ExtendFixation  NFoundSamples %2i  iNNewSamples %2i\n",
            stFix[PRES_FIX].iNEyeFoundSamples,
            iNNewSamples);
      #endif

/*    Save the fixation end count for use next time this function is        */
/*    called.                                                               */
      siPreviousFixEndCount = stFix[PRES_FIX].iEndCount;
   }
}
/****************************************************************************/
float CalcGazeDeviationFromFix(int iNewPresOrPrev, float fXGaze, float fYGaze)

/* This function calculates the deviation of the gazepoint from the         */
/* argument NewPresOrPrev fixation location.                                */

{
   float  fDx, fDy;                /* horizontal and vertical deviations    */
   float  fDr;                     /* total gazepoint deviation from the    */
                                   /*   argument fixation point             */
   double dDrSq;                   /* square of the gazepoint deviation     */

/* Calculate the gazepoint's horizontal and vertical offsets from the       */
/* argument fixation point.                                                 */
   fDx = fXGaze - stFix[iNewPresOrPrev].fX;
   fDy = fYGaze - stFix[iNewPresOrPrev].fY;

/* Compute the sum of the squares.                                          */
   dDrSq = fDx * fDx + fDy * fDy;

/* Calculate the total gazepoint deviation from the argument fixation.      */
   _ASSERTE(dDrSq >= 0.0);
   if      (dDrSq <  0.0)
            dDrSq =  0.0;
   fDr = (float)sqrt(dDrSq);

/* If we're working with the present fixation,                              */
   if (iNewPresOrPrev == PRES_FIX)
   {
/*    Put the deviation in the ring buffer for future reference.            */
      _ASSERTE((iCurrentRingIndex >= 0) && (iCurrentRingIndex < RING_SIZE));
      stRingBuf[iCurrentRingIndex].fGazeDeviation = fDr;
   }

   return fDr;
}
/****************************************************************************/
void TestPresentFixationHypothesis(int iMinimumFixSamples)

/* This function tests to see whether there are enough samples in the       */
/* present fixation hypothesis to declare that the eye is fixating, and     */
/* if there is a true fixation going on, the function updates the           */
/* current and any appropriate earlier ring buffer entries to reflect       */
/* the fixation.                                                            */

{
   int  iEarlierPointOffset;      /* index offset from the current ring-    */
                                  /*   buffer index                         */
   int  iDumRingIndex;            /* dummy ring index                       */

/* If there are enough eye-found gazepoint samples in the present fixation  */
/* hypothesis to qualify it as a real fixation,                             */
   if (stFix[PRES_FIX].iNEyeFoundSamples >= iMinimumFixSamples)
   {
/*    Declare the eye to be fixating.  Loop back through the last           */
/*    iMinimumFixSamples entries of the ring buffer, including the          */
/*    current point.                                                        */
      for (iEarlierPointOffset = 0;
           iEarlierPointOffset < iMinimumFixSamples;
           iEarlierPointOffset++)
      {
/*       Compute the ring index of the earlier point.                       */
         iDumRingIndex = iCurrentRingIndex - iEarlierPointOffset;
         if (iDumRingIndex < 0) iDumRingIndex += RING_SIZE;

         _ASSERTE((iDumRingIndex >= 0) && (iDumRingIndex < RING_SIZE));

/*       Mark the point as fixating at the currently computed fixation      */
/*       point.                                                             */
         stRingBuf[iDumRingIndex].iEyeMotionState = FIXATING;
         stRingBuf[iDumRingIndex].fXFix = stFix[PRES_FIX].fX;
         stRingBuf[iDumRingIndex].fYFix = stFix[PRES_FIX].fY;

/*       Set the ring buffer's entry for the saccade duration, i.e. the     */
/*       time from the end of the previous fixation to the start of the     */
/*       present fixation.                                                  */
/*       Note: The saccade duration is the same for all ring index points.  */
         stRingBuf[iDumRingIndex].iSacDuration =
            stFix[PRES_FIX].iStartCount - stFix[PREV_FIX].iEndCount - 1;

/*       Set the ring buffer's entry for the fixation duration, i.e. the    */
/*       time from the start of the present fixation to the time indicated  */
/*       by the ring index.                                                 */
/*       Note: The fixation duration changes (decreases) for earlier        */
/*       restored points.                                                   */
         stRingBuf[iDumRingIndex].iFixDuration =
            stFix[PRES_FIX].iEndCount - iEarlierPointOffset -
            stFix[PRES_FIX].iStartCount + 1;
      }
   }
}
/****************************************************************************/
void MoveNewFixToPresFix(void)

/* This function copies the new fixation data into the present fixation,    */
/* and resets the new fixation hypothesis.                                  */

{
/* Reset the number of consecutive gazepoints that have gone outside        */
/* the present fixation's acceptance circle.                                */
   iNOutsidePresCircle = 0;

/* Move the new fixation hypothesis to the present fixation hypothesis.     */
   stFix[PRES_FIX] = stFix[NEW_FIX];

   #if defined DIAG_FIXFUNC
      fprintf(hfFixFile, "MoveNewFixToPresFix() NEyeFoundSamples %i\n",
         stFix[PRES_FIX].iNEyeFoundSamples);
   #endif

/* Reset the new, new fixation.                                             */
   ResetFixationHypothesis(NEW_FIX);
}
/****************************************************************************/
void DeclarePresentFixationComplete(int iMinimumFixSamples)

/* This function:                                                           */
/* a) declares the present fixation to be completed back at                 */
/*    stFix[PRES_FIX].iEndCount.                                            */
/* b) moves the present fixation to the prior fixation, and                 */
/* c) moves the new fixation, if any, to the present fixation.              */

{
   int iRingIndexLastFixSample;  /* ring index of the present fixation's    */
                                 /*   final (completed) gaze sample         */
   int iDoneNSamplesAgo;

/* Compute how many samples ago the fixation was completed.                 */
   iDoneNSamplesAgo = iDFCallCount - stFix[PRES_FIX].iEndCount;

   _ASSERTE(iDoneNSamplesAgo <= iMinimumFixSamples);
   if (iDoneNSamplesAgo > iMinimumFixSamples)
       iDoneNSamplesAgo = iMinimumFixSamples;

/* Compute the ring index corresponding to the present fixation's           */
/* completion time.                                                         */
   iRingIndexLastFixSample = iCurrentRingIndex - iDoneNSamplesAgo;
   if (iRingIndexLastFixSample < 0)
       iRingIndexLastFixSample += RING_SIZE;

   _ASSERTE(iRingIndexLastFixSample >= 0);
   _ASSERTE(iRingIndexLastFixSample < RING_SIZE);

/* Declare the present fixation to be completed.                            */
   stRingBuf[iRingIndexLastFixSample].iEyeMotionState = FIXATION_COMPLETED;

/* Move the present fixation to the previous fixation.                      */
   stFix[PREV_FIX] = stFix[PRES_FIX];

/* Move the new fixation data, if any, to the present fixation, reset       */
/* the new fixation, and check if there are enough samples in the new       */ //??
/* (now present) fixation to declare that the eye is fixating.              */
   MoveNewFixToPresFix();
}
/****************************************************************************/
#if defined DIAG_FIXFUNC
void WriteDiagDataFileHeader(
        int   iMinimumFixSamples,
        float fGazeDeviationThreshold)

/* This writes the header lines for the diagnostic data file.               */

{
//   _TCHAR achDate[20];
//   _TCHAR achTime[20];

/* Record the date and time of the file write.                              */
//   lct_systime(achTime, 20, 18);
//   lct_sysdate(achDate, 20,  8);

//   fprintf(hfFixFile, "DIAG.FIX  -  printout from within DetectFixation()  %s  %s\n", achDate, achTime);
   fprintf(hfFixFile, "DIAG.FIX  -  printout from within DetectFixation()\n");
   fprintf(hfFixFile, "iMinimumFixSamples:      %2i\n",   iMinimumFixSamples);
   fprintf(hfFixFile, "fGazeDeviationThreshold: %5.2f\n", fGazeDeviationThreshold);
   fprintf(hfFixFile, "iMaxMissedSamples:       %2i\n",   iMaxMissedSamples);
   fprintf(hfFixFile, "iMaxOutSamples:          %2i\n\n", iMaxOutSamples);

   fprintf(hfFixFile, "                 n      |-----------------pres-----------------|----------------new---------------|----function output----|\n");
   fprintf(hfFixFile, "                sinc            n    fix   fix                              fix   fix              eye             sac fix \n");
   fprintf(hfFixFile, "call  x     y   last  n   dr/  fix  start  end    x     y    n   dr/  fix  start  end    x     y   mtn    x     y  dur dur \n");
   fprintf(hfFixFile, "cnt  gaze  gaze good out thres smpl  cnt   ctn   fix   fix  out thres smpl  cnt   cnt   fix   fix  st    fix   fix cnt cnt \n\n");
}
#endif   /* DIAG_FIXFUNC */
/****************************************************************************/
#if defined DIAG_FIXFUNC

void RecordDiagnosticData(
        BOOL  bGazepointFound,
        float fXGaze,
        float fYGaze,
        float fGazeDeviationThreshold,
        int   iMinimumFixSamples,
        float fXFixation,
        float fYFixation,
        int   iSaccadeDurationDelayed,
        int   iFixDurationDelayed)

/* This function records data for diagnostic purposes.                      */

{
   char achTemp[50];
   char achPrint[136];

/* If this is the first call,                                               */
   if (iDFCallCount == 1)
   {
/*    Open the diag.fix file and write a header to it.                      */
/*    Note: This file is never closed, because DetectFixation() does not    */
/*    when it is called last.  -- This is not elegant --                    */
      hfFixFile = fopen("diag.fix", "w");
      WriteDiagDataFileHeader(iMinimumFixSamples, fGazeDeviationThreshold);
   }

/*----------------------------- Call Count ---------------------------------*/ 

   sprintf_s(achPrint, _countof(achPrint), "%3li ", iDFCallCount);

/*-------------------------- Current Gazepoint -----------------------------*/ 

   if (bGazepointFound == TRUE)
   {
      sprintf_s(achTemp, _countof(achTemp), "%5.1f %5.1f ", fXGaze, fYGaze);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "  -     -   ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

/*------------------------------- NPresOut----------------------------------*/ 

   sprintf_s(achTemp, _countof(achTemp), "%3i ",iNOutsidePresCircle);
   strcat_s(achPrint, _countof(achPrint), achTemp);

/*-------------------------- Present Fixation ------------------------------*/

   if (fPresDr >= 0.0F)
   {
      sprintf_s(achTemp, _countof(achTemp), "%5.1f ", fPresDr / fGazeDeviationThreshold);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "   -  ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

   if (stFix[PRES_FIX].iNEyeFoundSamples > 0)
   {
      sprintf_s(achTemp, _countof(achTemp), "%4i %5i %5i %5.1f %5.1f ",
         stFix[PRES_FIX].iNEyeFoundSamples,
         stFix[PRES_FIX].iStartCount,
         stFix[PRES_FIX].iEndCount,
         stFix[PRES_FIX].fX,
         stFix[PRES_FIX].fY);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "   -     -     -     -     - ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

   if (iNOutsidePresCircle > 0)
   {
      sprintf_s(achTemp, _countof(achTemp), "%3i ", iNOutsidePresCircle);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "  - ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

/*---------------------------- New Fixation --------------------------------*/

   if (fNewDr >= 0.0F)
   {
      sprintf_s(achTemp, _countof(achTemp), "%5.1f ", fNewDr / fGazeDeviationThreshold);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "    - ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

   if (stFix[NEW_FIX].iNEyeFoundSamples > 0)
   {
      sprintf_s(achTemp, _countof(achTemp), "%4i %5li %5li %5.1f %5.1f ",
         stFix[NEW_FIX].iNEyeFoundSamples,
         stFix[NEW_FIX].iStartCount,
         stFix[NEW_FIX].iEndCount,
         stFix[NEW_FIX].fX,
         stFix[NEW_FIX].fY);
   }
   else
   {
      sprintf_s(achTemp, _countof(achTemp), "   -     -     -     -     - ");
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

/*-------------------------- Function Output --------------------------------*/

   if ((stRingBuf[iRingIndexDelay].iEyeMotionState == FIXATION_COMPLETED) ||
       (stRingBuf[iRingIndexDelay].iEyeMotionState == FIXATING))
   {
      sprintf_s(achTemp, _countof(achTemp), "%3i %5.1f %5.1f %3i %3i ",
         stRingBuf[iRingIndexDelay].iEyeMotionState,
         fXFixation,
         fYFixation,
         iSaccadeDurationDelayed,
         iFixDurationDelayed);
   }
   else
   {                      /* MOVING */
      sprintf_s(achTemp, _countof(achTemp), "%3i     -     -   -   - ",
         stRingBuf[iRingIndexDelay].iEyeMotionState);
   }
   strcat_s(achPrint, _countof(achPrint), achTemp);

   strcat_s(achPrint, _countof(achPrint), "\n");

   if (stRingBuf[iRingIndexDelay].iEyeMotionState == FIXATION_COMPLETED)
   {
      strcat_s(achPrint, _countof(achPrint), "\n");
   }

/*-------------------------- Write to File ---------------------------------*/

   #if defined(PRINT_RINGS)
      WriteDiagDataFileHeader(iMinimumFixSamples, fGazeDeviationThreshold);
   #endif  /* PRINT_RINGS */

/* Print the line of diagnostic data.                                      */
   fprintf(hfFixFile, achPrint);

   #if defined(PRINT_RINGS)
   {
/*    Print the ring buffers.                                              */
      int i;
      fprintf(hfFixFile, "      iCurrentRingIndex:%2i  iRingIndexDelay:%2i\n",
                                iCurrentRingIndex,     iRingIndexDelay);
      fprintf(hfFixFile, "    i  x_gaz  y_gaz  fnd mtn x_fix  y_fix    dr  nsac nfix\n");
      for (i = 0; i < RING_SIZE; i++)
      {
         fprintf(hfFixFile, "%5i %6.2f %6.2f %3i %3i %6.2f %6.2f %6.2f %4i %4i\n",
                 i,
                 stRingBuf[i].fXGaze,
                 stRingBuf[i].fYGaze,
                 stRingBuf[i].bGazeFound,
                 stRingBuf[i].iEyeMotionState,
                 stRingBuf[i].fXFix,
                 stRingBuf[i].fYFix,
                 stRingBuf[i].fGazeDeviation,
                 stRingBuf[i].iSacDuration,
                 stRingBuf[i].iFixDuration);
      }
   }
   #endif  /* PRINT_RINGS */
}
#endif   /* DIAG_FIXFUNC */
/****************************************************************************/
