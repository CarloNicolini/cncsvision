/* Suprt2pc.h */

int LctInitComm(struct _stEgControl *pstEgControl);
int LctGetData(struct _stEgControl *pstEgControl);
int LctGetEvent(void *pv);
DWORD WINAPI LctSupportRemoteCalibration(struct _stEgControl *pstEgControl,
                                         float fClientMonWidthMm,
                                         float fClientMonHeightMm,
                                         int   iClientMonWidthPix,
                                         int   iClientMonHeightPix,
                                         int   iWindowWidthPix,
                                         int   iWindowHeightPix,
                                         int   iWindowHorzOffset,
                                         int   iWindowVertOffset,
                                         int   iCommType);

int  QueryEyegazeSystem(void *hfEyegazePort, int iCommType);
BOOL ReceiveGazepointDataAscii(void *hfEyegazePort,
                               int *i_gaze, int *j_gaze,
                               float *fPupilRadiusMm, int *g_p_vector_found,
                               int *iDeltaCameraFieldCount,
                               int iCommType);
BOOL ReceiveGazepointDataBinary(void *hfEyegazePort,
                                struct _stEgData *stEgData,
                               int iCommType, int *iVis);
void LctExit(struct _stEgControl *pstEgControl);


void MousePosition(int iX, int iY);
void LeftClick(void);
void LeftDoubleClick(void);
void LeftClickAndHold(void);
void LeftRelease(void);
void RightClick(void);
void RightDoubleClick(void);
void RightClickAndHold(void);
void RightRelease(void);

void LctLogFileOpen(struct _stEgControl *pstEgControl, char *pszFileName, char *pszMode);
void LctLogWriteColumnHeader(struct _stEgControl *pstEgControl);
void LctAppendText(struct _stEgControl *pstEgControl, char *pszText);
void LctLogStart(struct _stEgControl *pstEgControl);
void LctLogStop(struct _stEgControl *pstEgControl);
void LctLogMark(struct _stEgControl *pstEgControl);
void LctLogFileClose(struct _stEgControl *pstEgControl);
void LctSendMessage(void *hfPort, int iMessage, char *pszBuffer, unsigned int uiBufferLen, int iCommType);
int  LctReadMessage(void *hfPort, int *iMessage, char *pszBuffer, unsigned int *uiBufferLen, int iCommType);

#define LEFTCLICK          0
#define LEFTDOUBLECLICK    1
#define LEFTCLICKANDHOLD   2
#define LEFTRELEASE        3
#define RIGHTCLICK         4
#define RIGHTDOUBLECLICK   5
#define RIGHTCLICKANDHOLD  6
#define RIGHTRELEASE       7

/****************************************************************************/
struct _stEventMousePosition
{
   int iX;
   int iY;
};
struct _stEventMouseRelative
{
   int iMouseRelative;
};
struct _stEventMouseButton
{
   int iMouseEvent;
   int iX;
   int iY;
};
struct _stEventKeyboardCommand
{
   char chKey;
};

/****************************************************************************/

