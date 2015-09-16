#include <windows.h>

BOOL WINAPI DllMain( HANDLE hInstance,
                     DWORD  ul_reason_for_call,
                     LPVOID lpReserved )
{
	static int
		bWinsockInit = 0;
	int
		nRet = 0;
	WORD 
		wVersionRequested;
	WSADATA
		wsaData;
	int
		err;
 
 
	switch( ul_reason_for_call )
	{
		case DLL_PROCESS_ATTACH:		/* LoadLibrary */
			wVersionRequested = MAKEWORD( 2, 2 );
			err = WSAStartup( wVersionRequested, &wsaData );
			if( err != 0 )
			{
#ifdef MESSAGE_BOX_OUTPUT
				MessageBox( 0, "UsbLink", "Unable to initialize", MB_OK );
#endif
				goto cleanup;
			}

			/* Confirm that the WinSock DLL supports 2.2.*/
			/* Note that if the DLL supports versions greater    */
			/* than 2.2 in addition to 2.2, it will still return */
			/* 2.2 in wVersion since that is the version we      */
			/* requested.                                        */
			if( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
			{
#ifdef MESSAGE_BOX_OUTPUT
				MessageBox( 0, "UsbLink", "Invalid Version of Winsock", MB_OK );
#endif
				WSACleanup( );
				goto cleanup;
			} /* if */
			bWinsockInit = 1;
			nRet = 1;
#ifdef MESSAGE_BOX_OUTPUT
			MessageBox( 0, "Got PROCESS_ATTACH", "UsbLink", MB_OK );
#endif
			break;
		case DLL_PROCESS_DETACH:		/* FreeLibrary */
			if( bWinsockInit )
			{
				WSACleanup();
				bWinsockInit = 0;
			} /* if */
#ifdef MESSAGE_BOX_OUTPUT
			MessageBox( 0, "Got PROCESS_DETACH", "ETHERLINK", MB_OK );
#endif
			nRet = 1;
			break;
		case DLL_THREAD_ATTACH:			/* New Thread - Should not occur */
#ifdef MESSAGE_BOX_OUTPUT
			MessageBox( 0, "Unexpected THREAD_ATTACH", "UsbLink", MB_OK );
#endif
			nRet = 1;
			break;
		case DLL_THREAD_DETACH:			/* Thread exited - should not occur */
#ifdef MESSAGE_BOX_OUTPUT
			MessageBox( 0, "Unexpected THREAD_DETACH", "UsbLink", MB_OK );
#endif
			nRet = 1;
			break;
		default:
#ifdef MESSAGE_BOX_OUTPUT
			MessageBox( 0, "Unexpected reason", "UsbLink", MB_OK );
#endif
			break;
	} /* switch */

cleanup:
    return nRet;
}
