/****************************************************************************

File Name:		  LCTSUPT.H

Company: 		  LC Technologies, Inc.
					  10363 Democracy Lane
                 Fairfax, VA 22030
					  (703) 385-7133

*****************************************************************************/
#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/****************************************************************************/
#include <tchar.h>

/****************************************************************************/
#define FALSE 0
#define TRUE  1

/* Font Functions:																			 */
int	lct_fontinit(void);
int	lctFontLoad(int iFontNumber);
int	lctFontLoadSpecific(int iFontNumber, _TCHAR *pchFontName,
								  int iDeciPtHeight, int iFontAttributes);
void	lct_font_unload(int iFontNumber);
int	lct_fontpixn(int kk,_TCHAR *str,int dir,int len);
int	lct_fontheight(int iFontNumber, _TCHAR *pchText);
int	lct_fontch(int kk, _TUCHAR ch, int tc, int tr, int color, int rot);
int	lct_fontsprt(int kk, _TCHAR *s, int tc, int tr, int dir, int color, int rot);
void	lct_FontInfo(int iFontNumber, void *p);
void	read_font_names(void);

/* Graphics Primitives: 																	 */
void	lct_dot(int x, int y,int color);
int	lct_readdot(int x, int y);
void	lct_line(int x1,int y1,int x2,int y2,int color);
void	lct_dashed_line(int x1,int y1,int x2,int y2,int color, int iLineStyle);
int	lct_rectangle(int x0,int y0,int x1,int y1,int color,int fill,int hatch);
int	lct_circle(int x, int y, int radius, int color, int fill);
int	lct_polygon(int *poly_x,int *poly_y,int sides,int close,int BorderColor,int fill,int FillColor,int type);
void	lct_ReadGraphicsArea(int ulx,int uly,int lrx,int lry, char *array);
void	lct_WriteGraphicsArea(int ulx, int uly, char *array, int type);
void  lct_RedrawScreen(int x1, int y1, int x2, int y2);

/* Graphics Mode Functions:																 */
void	lct_typedot(int i);
int	lct_GetXORMode(void);

/* Misc. Functions:																			 */
void	lct_sound(int frequency, unsigned int duration);
int	lct_syspause(int hours,int min,int sec,int hsec);
int	lct_lprtstat(int);
int	lct_sysdate(_TCHAR *date, int iMaxDateStrLen, int format);
int	lct_systime(_TCHAR *time, int iMaxTimeStrLen, int format);
int	lct_sysitime(int *hours, int *min, int *sec, int *hsec);
int	lct_copyfile(_TCHAR *,_TCHAR *);
void	lct_curget(int *x,int *y);
int	lct_curlocat(int x,int y);
int	lct_scrndump(int printer,int dir,int size,int horzspac,int vertspac,int formfeed,int copies,int colors);
int	lct_screendump(int iHorzDim, int iVertDim, int bFlipBlackToWhite);
void	lct_clearscreen(int color);
long	lct_timer(void);
long	lct_MStimer(void);
int	lct_FindFirstFileName(_TCHAR *wildcard, _TCHAR *achFileName, int iMaxFileNameLen);
int	lct_FindNextFileName(_TCHAR *achFileName, int iMaxFileNameLen);
void	lct_set_printer_shade(int color, int shade);
int   lctScreenWidthPix(void);
int   lctScreenHeightPix(void);
//void lctSetWindowHandle(HWND hwnd, HDC memdc, HDC hdc, _TCHAR *sz, int iAppNameStringLen);

/****************************************************************************/
/* Serial port open and close:															 */
void *lct_serial_open(_TCHAR *achPortName, long lPortSpeedBps, _TCHAR *achErrorMessage, int iMsgStrLen);
int lct_serial_close(void *PortHandle);

/* Serial data read and write functions:												 */
int lct_serial_read_char(void *PortHandle);
int lct_serial_read_string(void *hfPortHandle, char *string, unsigned long ulMaxLen);
int lct_serial_read_buffer(void *hfPortHandle, char *string, unsigned long ulMaxLen);

int lct_serial_send_char(void *PortHandle, int chr);
int lct_serial_send_string(void *PortHandle, char *string);
int lct_serial_send_buffer(void *PortHandle, char *string, unsigned long uLen);

/* Hayes compatible modem control functions: 										 */
int lct_modem_hmdialrate(void *hfPortHandle, int rate);
int lct_modem_hmdialmode(void *hfPortHandle, unsigned dial_mode);
int lct_modem_hmdial(void *hfPortHandle, char *dial_buffer);
int lct_modem_hmanswer(void *hfPortHandle);
int lct_modem_hmhook(void *hfPortHandle, unsigned state);

/* Handshaking line control functions: 												 */
int lct_serial_set_dtr(void *hfPortHandle, unsigned state);
int lct_serial_set_rts(void *hfPortHandle, unsigned state);
int lct_serial_send_break(void *hfPortHandle, unsigned len);

/* Input and output buffer status functions: 										 */
int lct_serial_get_input_q_count(void *hfPortHandle);
int lct_serial_get_output_q_count(void *hfPortHandle);


/* Sockets */
void *lct_socket_open(int iServerClient, char *pszPortNumber,
                      wchar_t *pszRemoteAddress, _TCHAR *achErrorMessage, int iMaxErrMsgStrLen);
int lct_socket_close(void *PortHandle);

/* Serial data read and write functions:												 */
int lct_socket_read_char(void *PortHandle);
int lct_socket_read_string(void *hfPortHandle, char *string, unsigned long ulMaxLen);
int lct_socket_read_buffer(void *hfPortHandle, char *string, unsigned long ulMaxLen);

int lct_socket_send_char(void *PortHandle, int chr);
int lct_socket_send_string(void *PortHandle, char *string);
int lct_socket_send_buffer(void *PortHandle, char *string, unsigned long uLen);

/* Input and output buffer status functions: 										 */
int lct_socket_get_input_q_count(void *hfPortHandle);
int lct_socket_get_output_q_count(void *hfPortHandle);

/****************************************************************************/
/* Functions used in Windows: 															 */
void EyegazeScreenUpdatesActive(int bActive);
int  lctNVisionSystems(void);

/****************************************************************************/
#ifdef __cplusplus
}			  /* Assume C declarations for C++ */
#endif  /* __cplusplus */
/****************************************************************************/

