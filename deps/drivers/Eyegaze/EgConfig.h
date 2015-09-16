/****************************************************************************/
// EgConfig.h - include file for EgGetConfig function
// 09/02/2011

/****************************************************************************/
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */
/****************************************************************************/
struct _stEgConfig
{
   int    iNVis;                 /* Total number of vision systems          */
   BOOL   bEyefollower;          /* Indicates EF (TRUE) or not (FALSE)      */

};

/****************************************************************************/
/* Function Prototypes:                                                     */

int  EgGetConfig(struct _stEgControl *pstEgControl,
                 struct _stEgConfig *pstEgConfig,
                 int    stEgConfigStructSize);

/****************************************************************************/
#ifdef __cplusplus
}          /* Assume C declarations for C++ */
#endif  /* __cplusplus */
/****************************************************************************/
