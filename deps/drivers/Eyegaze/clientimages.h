/****************************************************************************

   ClientImages.h

*****************************************************************************/
#ifndef CLIENTIMAGES_H_INCLUDED
#define CLIENTIMAGES_H_INCLUDED
/****************************************************************************/
#define EYE_IMAGE_DIVISOR     4    /* decimation factor for the full eye    */
                                   /*   image display                       */
#define EYE_IMAGE_WIDTH       (640/EYE_IMAGE_DIVISOR)
#define EYE_IMAGE_HEIGHT      (480/EYE_IMAGE_DIVISOR)
                                   /* width and height of the individual    */
                                   /*   image displays (pixels)             */
#define FOCUS_INDICATOR_WIDTH 4    /* width of foucs offset indicator in    */
                                   /*   between the closeup and full        */
                                   /*   camera images (pixels)              */
#define TOTAL_IMAGE_WIDTH     (EYE_IMAGE_WIDTH*2+FOCUS_INDICATOR_WIDTH)
                                   /* width of the full eye display,        */
                                   /*   including the closeup image,        */
                                   /*   focus offset indicator, and full    */
                                   /*   camera image                        */

/****************************************************************************/
struct _stEventEyeImage
{
   int iWidth;
   int iHeight;
   int iVisionSystem;
   unsigned char auchImage[TOTAL_IMAGE_WIDTH * EYE_IMAGE_HEIGHT];
};
/****************************************************************************/
VOID CreateAndSendTinyBitmap(
        void *hfComm,
        int   iField,
        float fFocusOffsetMm,
        BOOL  bEyeFound,
        void *hEyegaze,
        int   iVisionSystem,
        int   iVisOffset,
        int   iQIris,
        int   iQPupil,
        int   iQGlint);
void DisplayEyeImages(HDC hdc, struct _stEventEyeImage *pstEyeImage,
                      int iHorzOffset);

/****************************************************************************/
#endif // CLIENTIMAGES_H_INCLUDED
/****************************************************************************/
