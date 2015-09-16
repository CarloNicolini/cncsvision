/***************************************************************************** 

File Name:       GAZEVERGENCE.H 
                 Gaze Vergence Header File:

Company:         LC Technologies, Inc.
                 3919 Old Lee Highway
                 Fairfax, VA 22031
                 (703) 385-7133

*****************************************************************************/
#ifndef GAZEVERGENCE_H_INCLUDED
#define GAZEVERGENCE_H_INCLUDED
/****************************************************************************/
/* Gaze Vergence Data Structure:                                            */

struct _stGazeVergence
{
   float  afXCornCentWldMm[2];      /* two eyes' cornea-center locations    */
   float  afYCornCentWldMm[2];      /*   within the user World frame of     */
   float  afZCornCentWldMm[2];      /*   reference (mm)                     */
   float  afXGazeVectWld[2];        /* two eyes' gaze vectors within the    */
   float  afYGazeVectWld[2];        /*   user World frame                   */
   float  afZGazeVectWld[2];        /*   (unit vector)                      */
          // index 0 -> user's left eye
          // index 1 -> user's right eye

   BOOL   bGazeVergenceFound;       /* flag indicating whether a valid      */
                                    /*   binocular gaze vergence and a      */
                                    /*   corresponding 3D gazepoint were    */
                                    /*   measured                           */
   int    iVis;                     /* The vision system that produced the  */
                                    /*   most recent data.                  */

   ULONG  ulCameraFieldCount;       /* number of camera fields, i.e. 60ths  */
                                    /*   of a second, that have occurred    */
                                    /*   since the starting reference time  */
                                    /*   (midnight January 1, this year)    */

   // the following data are not valid if bGazeVergenceFound is not TRUE

   float  fTotDistBetweenEyesMm;    /* total distance between the two       */
                                    /*   eyes (mm)                          */
   float  fGazeVergenceAngleRad;    /* gaze vergence angle, i.e. the        */
                                    /*   angle between the two eyes'        */
                                    /*   gaze lines (rad)                   */

   float  fX3DGazePointWldMm;       /* millimeter location of the 3D        */
   float  fY3DGazePointWldMm;       /*   gazepoint within the World         */
   float  fZ3DGazePointWldMm;       /*   frame of reference (mm)            */
                                    /*   X positive right                   */
                                    /*   Y positive up                      */
                                    /*   Z positive toward user             */
                                    /*   X,Y = 0 at screen center           */
                                    /*   Z = 0 at 2D screen surface         */

   float  fI3DGazePointPix;         /* pixel location of the 3D gaze        */
   float  fJ3DGazePointPix;         /*   point                              */
   float  fK3DGazePointPix;         /*   I(x) positive right                */
                                    /*   J(-y) positive downward            */
                                    /*   K(z) positive toward user          */
                                    /*   I,J = 0 at upper left corner       */
                                    /*   K = 0 at 2D screen surface         */
};
/****************************************************************************/
/* Function Prototype:                                                      */

BOOL bGetVergenceAnd3DGazePoint(struct _stGazeVergence *pstGazeVergence);

/****************************************************************************/
#endif // defined GAZEVERGENCE_H_INCLUDED
/****************************************************************************/
