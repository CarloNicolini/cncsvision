/****************************************************************************

File Name:       WINSER.C
Program Name:    Serial port support Library for the Eyegaze System

Company:         LC Technologies, Inc.
                 10363 Democracy Lane
                 Fairfax, VA 22030
                 (703) 385-7133

Date Created:    06/09/99

*****************************************************************************

These LCT Support Functions allow developers to use serial port support
functions from third-party sources of their choice, thus not being restricted 
to the packages generally used by LC Technologies.  These support functions
provide an intermediate mapping layer between utility function calls within 
Eyegaze programs and functions provided by specific vendors.  Developers 
can use the serial port support packages of their choice with all their
Eyegaze programs simply by substituting the function calls of their support 
packages into these functions.  

*****************************************************************************/
/*NOTE: lct_socket_open() passes in a pointer to an error string but does
         not provide a string length.  Here we use ERROR_MSG_LEN to limit
         that string length.                                                */


/****************************************************************************/
/* Required Function, Variable and Constant Definitions:                    */

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <lctsupt.h>
#include <winsock.h>
#include <lctascii.h>

#define   RXLEN          4096        /* length of recieve buffer            */
#define   TXLEN          2048        /* length of transmit buffer           */

#define   NO_DEBUG
#define   ERROR_MSG_LEN  120  // fill in for missing string length

/****************************************************************************/
void *lct_serial_open(_TCHAR *achPortName, long lPortSpeedBps, _TCHAR *achErrorMessage, int iMsgStrLen)
{
   HANDLE hfComm;
   DCB    dcb;

/* Open the serial port, set the IOCtl parameters.                          */
   hfComm = CreateFile(achPortName, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                       OPEN_EXISTING, 0, NULL);
   if (hfComm == INVALID_HANDLE_VALUE)
   {
      swprintf_s(achErrorMessage, iMsgStrLen, _TEXT("Cannot Open %s\n"), achPortName);
      return NULL;
   }

/* Set the baud rate by reading the port state, changing the speed and      */
/* setting it.                                                              */
   if (GetCommState(hfComm, &dcb) == 0)
   {
      _tprintf(_TEXT("Error reading Comm State\n"));
   }

   dcb.BaudRate = lPortSpeedBps;
   dcb.ByteSize = 8;
   dcb.Parity   = NOPARITY;
   dcb.StopBits = ONESTOPBIT;

   
   if (SetCommState(hfComm, &dcb) == 0)
   {
	   _tprintf(_TEXT("Error setting Comm State\n"));
   }

/* With this call, we can configure the receive and transmit buffers for    */
/* the serial port.  Commenting it out, we use the defaults already in the  */
/* device driver.                                                           */
//   SetupComm(hfComm, RXLEN, TXLEN);

   return (void *)hfComm;
}
/****************************************************************************/
int  lct_serial_close(void *hfPortHandle)
{
   CloseHandle((HANDLE)hfPortHandle);
   return 0;
}
/****************************************************************************/
int  lct_serial_send_char(void *hfPortHandle, int chr)
{
   ULONG ulBytesWritten;

   if (WriteFile(hfPortHandle, &chr, 1, &ulBytesWritten, NULL) == 0)
   {
	   _tprintf(_TEXT("Error writing character (lct_serial_send_char)\n"));
   }

   if (ulBytesWritten == 1)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_serial_send_string(void *hfPortHandle, char *string)
{
   ULONG ulBytesWritten;

   if (WriteFile(hfPortHandle, string, (DWORD)strlen(string), &ulBytesWritten, NULL) == 0)
   {
/*    A return value == 0 indicates a port write error.                       */
      LPVOID lpMsgBuf;
      DWORD rc = GetLastError();

      _tprintf(_TEXT("Error %i writing string (lct_serial_send_string)\n"), rc);
      FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER |
          FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          rc,
          0, // Default language
          (LPTSTR) &lpMsgBuf,
          0,
          NULL);
      _tprintf(_TEXT("%s\n"), lpMsgBuf);
   }


   if (ulBytesWritten == strlen(string))
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_serial_send_buffer(void *hfPortHandle, char *string, ULONG uLen)
{
   ULONG ulBytesWritten;

   if (WriteFile(hfPortHandle, string, uLen, &ulBytesWritten, NULL) == 0)
   {
	   _tprintf(_TEXT("Error writing character (lct_serial_send_buffer)\n"));
   }

   if (ulBytesWritten == uLen)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_serial_read_char(void *hfPortHandle)
{
   ULONG  cbBytesRead;
   ULONG  status;
   INT    ch;

   for (;;)
   {
      status = ReadFile(hfPortHandle,&ch,1,&cbBytesRead, NULL);

/*    If we successfully read a byte, break.                              */
      if (cbBytesRead == 1)
      {
         ch = ch & 0x000000ff;
         break;
      }
      if (cbBytesRead >1) _tprintf(_TEXT("BytesRead = %i!\n"),cbBytesRead);

/*    A return value == 0 indicates a port read error.                    */
      if (status == 0)
      {
         _tprintf(_TEXT("Error reading character (lct_serial_read_char)\n"));
         break;
      }

/*    call Sleep() to give the serial port a chance to respond, and to    */
/*    avoid consuming all available CPU time.                             */
      Sleep(20);
   }

   return ch;
}
/****************************************************************************/
int  lct_serial_read_string(void *hfPortHandle, CHAR *buffer, unsigned long ulMaxLen)
{
   ULONG  cbBytesRead;
   ULONG  status;
   INT    index;

   for (index = 0; index < (int)ulMaxLen; index++)
   {
      status = ReadFile(hfPortHandle,&buffer[index],1,&cbBytesRead, NULL);

/*    If the character read is the "opt" char, break out of the loop.       */
      if (buffer[index] == 0) break;

/*    A return value == 0 indicates a port read error.                    */
      if (status == 0)
	  {
		  _tprintf(_TEXT("Error reading character (lct_serial_read_string)\n"));
		  break;
	  }
   }

   return index-1;
}
/****************************************************************************/
int  lct_serial_read_buffer(void *hfPortHandle, CHAR *buffer, unsigned long ulMaxLen)
{
   ULONG  cbBytesRead;
   ULONG  status;
   INT    index;

   for (index = 0; index < (int)ulMaxLen; index++)
   {
      status = ReadFile(hfPortHandle,&buffer[index],1,&cbBytesRead, NULL);

/*    A return value == 0 indicates a port read error.                    */
      if (status == 0)
      {
         _tprintf(_TEXT("Error reading character (lct_serial_read_buffer)\n"));
         break;
      }
   }

   return index-1;
}
/****************************************************************************/
int  lct_serial_get_input_q_count(void *hfPortHandle)
{
   COMSTAT cs;
   long lPErrors;

   if (ClearCommError((HANDLE)hfPortHandle, &lPErrors, &cs) == 0)
   {
	   _tprintf(_TEXT("Error obtaining port status in lct_serial_get_input_q_count\n"));
   }

   return cs.cbInQue;
}
/****************************************************************************/
int lct_serial_set_dtr(void *hfPort, unsigned bState)
{
   if (bState == FALSE)
   {
      EscapeCommFunction((HANDLE)hfPort, CLRDTR);
   }
   else
   {
      EscapeCommFunction((HANDLE)hfPort, SETDTR);
   }
   return 0;
}
/****************************************************************************/
int lct_serial_set_rts(void *hfPort, unsigned bState)
{
   if (bState == FALSE)
   {
      EscapeCommFunction((HANDLE)hfPort, CLRRTS);
   }
   else
   {
      EscapeCommFunction((HANDLE)hfPort, SETRTS);
   }
   return 0;
}
/****************************************************************************/
int lct_serial_send_break(void *hfPort, unsigned uLen)
{
/* The length of the break signal is in terms of 55 ms clock ticks.         */

   SetCommBreak((HANDLE)hfPort);

/* Sleep for the duration of the break;                                     */
   Sleep(uLen*55);

   ClearCommBreak((HANDLE)hfPort);
   return 0 ;
}
/****************************************************************************/
int lct_serial_get_output_q_count(void *hfPortHandle)
{
   COMSTAT cs;
   long lPErrors;

   if (ClearCommError((HANDLE)hfPortHandle, &lPErrors, &cs) == 0)
   {
      _tprintf(_TEXT("Error obtaining port status in lct_serial_get_output_q_count\n"));
   }

   return cs.cbOutQue;
}
/****************************************************************************/
/* "Modem" functions:                                                       */
static CHAR chModemDialMode = 'T';

/****************************************************************************/
int lct_modem_hmdialrate(void *hfPortHandle, int rate)
{
   CHAR achDummy[40];

   sprintf_s(achDummy, _countof(achDummy), "ATS11=%i\r", rate);
   lct_serial_send_string(hfPortHandle, achDummy);
   return 0;
}
/****************************************************************************/
int lct_modem_hmdialmode(void *modem_port, unsigned dial_mode)
{
/* Set the default dial mode to 'T' (Touch Tone) unless dial_mode is        */
/* non-zero, in which case set the default dial mode to pulse.  This is a   */
/* static variable used in the hmdial() function.                           */
   chModemDialMode = 'T';
   if (dial_mode != 0)
   {
      chModemDialMode = 'P';
   }
   return 0;
}
/****************************************************************************/
int lct_modem_hmdial(void *modem_port, char *achDialBuffer)
{
   CHAR *pachDummy;
   int iStringLen;

/* Allocate memory for the string to send to the modem.                     */
   iStringLen = strlen(achDialBuffer) + 6;
   pachDummy = malloc(iStringLen);

   if (pachDummy == NULL) return -1;

   sprintf_s(pachDummy, iStringLen, "ATD%c%s\r", chModemDialMode, achDialBuffer);
   lct_serial_send_string(modem_port, pachDummy);

   free (pachDummy);

   return 0;
}
/****************************************************************************/
int lct_modem_hmanswer(void *modem_port)
{
   CHAR achDummy[40];

   sprintf_s(achDummy, _countof(achDummy), "ATA\r");
   lct_serial_send_string(modem_port, achDummy);
   return 0;
}
/****************************************************************************/
int lct_modem_hmhook(void *modem_port, unsigned state)
{
   CHAR achDummy[40];

   sprintf_s(achDummy, _countof(achDummy), "ATH%i\r", state);
   lct_serial_send_string(modem_port, achDummy);
   return 0;
}
/****************************************************************************/
/****************************************************************************/
/* SOCKET FUNCTIONS:                                                        */
/****************************************************************************/
/****************************************************************************/
SOCKET passiveTCP(const char *service, int qlen);
SOCKET connectTCP(const char *host, const char *service);
SOCKET passivesock(const char *service, const char *transport, int qlen);
SOCKET connectsock(const char *host, const char *service, const char *transport);
void   SocketErrorLog(char *achText);
void   DisplayError(_TCHAR *achText);

#define QLEN 5
#define SOCKET_SERVER 1
#define SOCKET_CLIENT 0

SOCKET  hSocket, hServerSocket, hClientSocket;
_TCHAR  achLogText[250];

/****************************************************************************/
void *lct_socket_open(int iServerClient, char *pszPort,
                      wchar_t *pszRemoteAddress, _TCHAR *achErrorMessage, int iMaxErrMsgStrLen)
// NOTE: pszRemoteAddress is not used when in SERVER mode.
{
   WSADATA wsadata;
   int     iRC;
   struct  sockaddr_in fsin;
   int     alen;
   static  BOOL bInitialized = FALSE;

   if (bInitialized == FALSE)
   {
/*    Using Winsock version 2.0                                                */
      WORD wVersion = MAKEWORD(2,0);  // winsock version

      if ((iRC = WSAStartup(wVersion, &wsadata)) != 0)
      {
         if (achErrorMessage != NULL)
         {
            swprintf_s(achErrorMessage, iMaxErrMsgStrLen,
                       _TEXT("Windows Sockets error: WSAStartup failed: WSA ERROR= %i\n"),
                       iRC);
         }
         swprintf_s(achLogText, _countof(achLogText),
                    _TEXT("Windows Sockets error: WSAStartup failed: WSA ERROR= %i\n"),
                    iRC);
         DisplayError(achLogText);
         return NULL;
      }

/*    If we're running as a server, wait here for a connection on the port.    */
      if (iServerClient == SOCKET_SERVER)
      {
         hSocket = passiveTCP(pszPort, QLEN);
      }
      bInitialized = TRUE;
   }

#if defined _DEBUG
{
            int iSend, iRecv;
            int iSize = sizeof(int);
            getsockopt(hSocket, IPPROTO_TCP, SO_SNDBUF, (char *)&iSend, &iSize);
            getsockopt(hSocket, IPPROTO_TCP, SO_RCVBUF, (char *)&iRecv, &iSize);
            printf("Socket send: %i  recv: %i\n", iSend, iRecv);
            fflush(stdout);

            iSend = 1024;
            iRecv = 1024;
            setsockopt(hSocket, IPPROTO_TCP, SO_SNDBUF, (char *)&iSend, iSize);
            setsockopt(hSocket, IPPROTO_TCP, SO_RCVBUF, (char *)&iRecv, iSize);

            getsockopt(hSocket, IPPROTO_TCP, SO_SNDBUF, (char *)&iSend, &iSize);
            getsockopt(hSocket, IPPROTO_TCP, SO_RCVBUF, (char *)&iRecv, &iSize);
            printf("Socket send: %i  recv: %i\n", iSend, iRecv);
            fflush(stdout);
}
#endif


   if (iServerClient == SOCKET_SERVER)
   {
      alen = sizeof(struct sockaddr);
      hServerSocket = accept(hSocket, (struct sockaddr *)&fsin, &alen);
      if (hServerSocket == INVALID_SOCKET)
      {
         swprintf_s(achLogText, _countof(achLogText),
            _TEXT("Accept failed: error number %i\n"), GetLastError());
         DisplayError(achLogText);
      }

      {
/*       NOTE: by default, socket sends are aggregated before they're       */
/*       transmitted  across the network.  Without setting this option      */
/*       (TCP_NODELAY), sends are accumulated and transmitted only about    */
/*       every 200 ms.                                                      */
         int i = TRUE;
         int rc;

         rc = setsockopt(hServerSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&i, sizeof(i));
         if (rc == SOCKET_ERROR)
         {
            swprintf_s(achLogText, _countof(achLogText),
               _TEXT("setsockopt failed: error number %i\n"), GetLastError());
            DisplayError(achLogText);
         }

      }


      return (void *)hServerSocket;
   }
   else
   {
      char achText[260];
	   lctWideCharToAscii(pszRemoteAddress, achText);
      hClientSocket = connectTCP(achText, pszPort);

      return (void *)hClientSocket;
   }
/* we can't get here.                                                       */
}
/****************************************************************************/
int  lct_socket_close(void *hfSocketHandle)
{
/* Disable sends and received on this socket.                               */
   #define SD_RECEIVE 0x00
   #define SD_SEND 0x01
   #define SD_BOTH 0x02
   shutdown((SOCKET)hfSocketHandle, SD_BOTH);

   closesocket((SOCKET)hfSocketHandle);
   return 0;
}
/****************************************************************************/
int  lct_socket_send_char(void *hfSocketHandle, int chr)
{
   ULONG ulBytesWritten;
   char ch = (char)chr;

   if ((ulBytesWritten = send((SOCKET)hfSocketHandle, &ch, 1, 0)) == SOCKET_ERROR)
   {
   }

   if (ulBytesWritten == 1)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_socket_send_string(void *hfSocketHandle, char *string)
{
   ULONG ulBytesWritten;

   if ((ulBytesWritten = send((SOCKET)hfSocketHandle, string, (int)strlen(string), 0)) == SOCKET_ERROR)
   {
   }

   if (ulBytesWritten == strlen(string))
   {
      return  0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_socket_send_buffer(void *hfSocketHandle, char *string, ULONG uLen)
{
   ULONG ulBytesWritten;

   if ((ulBytesWritten = send((SOCKET)hfSocketHandle, string, uLen, 0)) == SOCKET_ERROR)
   {
   }

   if (ulBytesWritten == uLen)
   {
      return  0;
   }
   else
   {
      return -1;
   }
}
/****************************************************************************/
int  lct_socket_read_char(void *hfSocketHandle)
{
   int  iBytesRead;
   char ch;

   for (;;)
   {
      iBytesRead = recv((SOCKET)hfSocketHandle,&ch,1,0);

/*    If we successfully read a byte, break out of the forever loop.        */
      if (iBytesRead == 1)
      {
         break;
      }

      if (iBytesRead >1)
      {
      }

/*    A return value == 0 indicates a port read error.                      */
      if (iBytesRead == SOCKET_ERROR)
      {
         break;
      }
   }

   return ch;
}
/****************************************************************************/
int  lct_socket_read_string(void *hfSocketHandle, CHAR *buffer, unsigned long ulMaxLen)
{
   int    iBytesRead;
   INT    index;

   for (index = 0; index < (int)ulMaxLen; index++)
   {
      iBytesRead = recv((SOCKET)hfSocketHandle,&buffer[index],1,0);

/*    If a null character was read from the socket, break out of the loop.  */
/*    This function is designed to read null-terminated strings.            */
      if (buffer[index] == 0) break;

/*    A return value == 0 indicates a port read error.                      */
      if (iBytesRead == SOCKET_ERROR)
      {
         break;
      }
   }

   return index-1;
}
/****************************************************************************/
int  lct_socket_read_buffer(void *hfSocketHandle, CHAR *buffer, unsigned long ulMaxLen)
{
   int    iBytesRead, iTotalRead;
   char   *bptr;
   int    buflen;

   iTotalRead = 0;

/* Store a pointer to the buffer in bptr.                                   */
   bptr   = buffer;

/* Record the initial length of data we want in buflen.                     */
   buflen = ulMaxLen;

/* Read the entire buffer from the socket.                                  */
   iBytesRead = recv((SOCKET)hfSocketHandle, bptr, buflen, 0);
   iTotalRead += iBytesRead;

   while((iBytesRead != SOCKET_ERROR) && (iBytesRead != 0) && (buflen != iBytesRead))
   {
      bptr   += iBytesRead;
      buflen -= iBytesRead;
      iBytesRead = recv((SOCKET)hfSocketHandle, bptr, buflen, 0);
      iTotalRead += iBytesRead;
   }

/* A return value == 0 indicates a port read error.                         */

   return iTotalRead;
}
/****************************************************************************/
int  lct_socket_get_input_q_count(void *hfSocketHandle)
{
   int iInQueue;
   unsigned long ulInQueue;

   iInQueue = ioctlsocket((SOCKET)hfSocketHandle, FIONREAD, &ulInQueue);

   if (iInQueue == SOCKET_ERROR)
   {
   }
   iInQueue = (int)ulInQueue;

   return iInQueue;
}
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* passiveTCP - create a passive socket for use in a TCP server             */
SOCKET passiveTCP(const char *service, int qlen)
{
   return passivesock(service, "tcp", qlen);
}
/****************************************************************************/
/* connectTCP - connect to a specified TCP service on a specified host      */
SOCKET connectTCP(const char *host, const char *service)
{
	return connectsock( host, service, "tcp");
}
/****************************************************************************/
/* passivesock - allocate & bind a server socket using TCP or UDP           */
SOCKET passivesock(const char *service, const char *transport, int qlen)
{
   struct servent  *pse;     /* pointer to service information entry        */
   struct protoent *ppe;     /* pointer to protocol information entry       */
   struct sockaddr_in sin;   /* an Internet endpoint address                */
   SOCKET          s;        /* socket descriptor                           */
   int             type;     /* socket type (SOCK_STREAM, SOCK_DGRAM)       */

   memset(&sin, 0, sizeof(sin));
   sin.sin_family      = AF_INET;
   sin.sin_addr.s_addr = INADDR_ANY;

/* Map service name to port number                                          */
   pse = getservbyname(service, transport);
   if (pse != 0)
   {
      sin.sin_port = htons(ntohs((u_short)pse->s_port));
   }
   else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't get \"%s\" service entry\n"), service);
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Map protocol name to protocol number                                     */
   if ((ppe = getprotobyname(transport)) == 0)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't get \"%s\" protocol entry\n"), transport);
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Use protocol to choose a socket type                                     */
   if (strcmp(transport, "udp") == 0)
      type = SOCK_DGRAM;
   else
      type = SOCK_STREAM;

/* Allocate a socket                                                        */
   s = socket(PF_INET, type, ppe->p_proto);
   if (s == INVALID_SOCKET)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't create socket: %d\n"), GetLastError());
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Bind the socket                                                          */
   if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't bind to %s port: %d\n"), service, GetLastError());
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }
   if (type == SOCK_STREAM && listen(s, qlen) == SOCKET_ERROR)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't listen on %s port: %d\n"), service, GetLastError());
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }
   return s;
}
/****************************************************************************/
/* connectsock - allocate & connect a socket using TCP or UDP               */
SOCKET connectsock(const char *host, const char *service, const char *transport)
{
	struct hostent	*phe;	/* pointer to host information entry	*/
	struct servent	*pse;	/* pointer to service information entry	*/
	struct protoent *ppe;	/* pointer to protocol information entry*/
	struct sockaddr_in sin;	/* an Internet endpoint address		*/
	int	s, type;	/* socket descriptor and socket type	*/


	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

/* Map service name to port number                                          */
   pse = getservbyname(service, transport);
   if (pse != 0)
		sin.sin_port = pse->s_port;
   else if ((sin.sin_port = htons((u_short)atoi(service))) == 0)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't get \"%s\" service entry\n"), service);
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Map host name to IP address, allowing for dotted decimal                 */
   phe = gethostbyname(host);
   if (phe != 0)
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
   else if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't get \"%s\" host entry\n"), host);
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Map protocol name to protocol number                                     */
   if ((ppe = getprotobyname(transport)) == 0)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't get \"%s\" protocol entry\n"), transport);
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }
/* Use protocol to choose a socket type                                     */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

/* Allocate a socket                                                        */
	s = (int)socket(PF_INET, type, ppe->p_proto);
	if (s == INVALID_SOCKET)
   {
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't create socket: %d\n"), GetLastError());
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }

/* Connect the socket                                                       */
   if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
   {
#if defined UNICODE
      wchar_t wHost[80];
      wchar_t wService[80];

	   lctAsciiToWideChar((char *)host, wHost);
	   lctAsciiToWideChar((char *)service, wService);
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't connect to Host: %s, Port: %s, Error: %d\n"), wHost, wService, GetLastError());
#else
      swprintf_s(achLogText, _countof(achLogText),
         _TEXT("Can't connect to Host: %s, Port: %s, Error: %d\n"), host, service, GetLastError());
#endif
      DisplayError(achLogText);
      return (SOCKET)NULL;
   }
	return s;
}
/****************************************************************************/
void DisplayError(_TCHAR *achLogText)
{
   MessageBox(NULL, achLogText,
              _TEXT("Sockets Error"), MB_ICONEXCLAMATION | MB_OK);

   WSACleanup();

   return;
}
/****************************************************************************/
#include <time.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <sys\timeb.h>
void SocketErrorLog(char *achText)
{
   FILE *hf;

   fopen_s(&hf, "socket.err", "a");
   if (hf == NULL)
   {
	   Beep(1500,100);
	   return;
   }

   if (hf != NULL)
   {
      struct _timeb tstruct;
      struct tm     today;

      _ftime_s(&tstruct);
      localtime_s(&today, &tstruct.time);

      fprintf(hf,"%02i/%02i/%4i %02i:%02i:%02i.%03i  %s",
                  today.tm_mon+1, today.tm_mday, today.tm_year+1900,
                  today.tm_hour, today.tm_min, today.tm_sec, tstruct.millitm,
                  achText);
      fclose(hf);
   }
}
/****************************************************************************/

