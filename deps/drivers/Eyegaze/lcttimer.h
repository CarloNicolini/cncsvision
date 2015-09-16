/*****************************************************************************
  LCTTIMER.H
*****************************************************************************/
#ifndef  LCTTIMER_H_INCLUDED
#define  LCTTIMER_H_INCLUDED
/****************************************************************************/
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/****************************************************************************/
/* Function Prototypes:                                                     */

#define UINT unsigned int

double lct_TimerRead(unsigned int *puiProcessorSpeedMHz);
int ReadProcSpeed(void);

/****************************************************************************/
#ifdef __cplusplus
}          /* Assume C declarations for C++ */
#endif  /* __cplusplus */
/****************************************************************************/
#endif // LCTTIMER_H_INCLUDED
/****************************************************************************/
