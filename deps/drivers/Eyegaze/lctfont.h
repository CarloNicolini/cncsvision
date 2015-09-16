
/* These fonts are used in the typing area in kbd and on screen labels in   */
/* phrases, etc.                                                            */
#define LCT_FONT_0     0
#define LCT_FONT_8X14  0

#define LCT_FONT_8X16  1
#define LCT_FONT_1     1

/* This the large font we use for program screen titles, etc.               */
#define LCT_FONT_ROMAN 2
#define LCT_FONT_2     2

/* This is the medium-sized font we use on the keyboard, button labels, etc.*/
#define LCT_FONT_SBO   3
#define LCT_FONT_3     3


struct lctFontInfo
{
   int iWidth;
   int iHeight;
   int iDescender;
};

