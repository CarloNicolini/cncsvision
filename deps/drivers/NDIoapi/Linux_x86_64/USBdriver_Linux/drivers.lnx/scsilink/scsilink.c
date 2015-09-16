/****************************************************************
Name:               scsilink.c

Description:        OPTOTRAK Link Routines for the SCSI interface

Library Name:

Include Files:

Modifications:

*****************************************************************/

/*
 * SCO-00-0001: Added typedef to overcome compilation errors when
 * building on a Slackware installation.
 */
typedef unsigned char u_char;

/*****************************************************************
C Library Files Included
*****************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>
/*****************************************************************
ND Library Files Included
*****************************************************************/
#include <ndtypes.h>
#include <ndgenlib.h>

#define HOST_BOARD              0
#define TARGET_ID               4
#define LUN                     0

#define SCSI_INPUT_STATUS_CMD   0x06
#define SCSI_RECEIVE_CMD        0x08
#define SCSI_RECEIVE_PADDED_CMD 0x09
#define SCSI_SEND_CMD           0x0A
#define SCSI_SEND_PADDED_CMD    0x0C
#define SCSI_INQUIRY_CMD        0x12
#define SCSI_INITIALIZE_CMD     0x0D
#define SCSI_RESET_CMD          0x02
#define SCSI_INQUIRY_REPLY_LEN  127
#define SCSI_INQUIRY_VENDOR     8
#define SG_HDR_SIZE  sizeof(struct sg_header)

/*
 *  SCSI device id
 */
static char
    szScsiPath[ 512 ];

static AppParmInfo grSCSIParms[] =
{
	{ "ScsiPath", szScsiPath, sizeof(szScsiPath), APP_PARM_STRING, 0, 0 },
    { NULL,       NULL,       0,                  0,               0, 0 }
};

static int ScsiFd = -1;
static int nTimeOut = 3000;
static boolean bSetTimeOut = FALSE;

boolean LinkOpen( void );
unsigned LinkRead( void *pData, unsigned uBytes );
boolean LinkStatus( boolean *pbReadable, boolean *pbWriteable );
boolean LinkClose( void );
unsigned LinkWrite( void *pData, unsigned uBytes );
boolean LinkReset( void );

/***************************************************************************

Routine:           DoScsiReq

Inputs:
    unsigned cmd_len      : The length of the command description block
    unsigned char *cdb    : Command Desciptor Block
    unsigned input_size   : The length of the input data
    unsigned char *input_buff : The input buffer, contains the sg_header
                                the CDB and the input data
    unsigned output_size  : The length of the output buffer
	unsigned char *output_buff : The output buffer

Returns:
    boolean               : TRUE if not successful

Description:
    Performs the SCSI operation indicated by *pReq and checks the
    SCSI status.  If the SCSI operation was successful, TRUE is returned.

***************************************************************************/
static int DoScsiReq(     unsigned cmd_len,   /* Length of the command */
						  unsigned char *cdb,
						  unsigned input_size, /* Input buffer size */
                          unsigned char *input_buff, /* input buffer */
                          unsigned output_size, /* output buffer size */
                          unsigned char *output_buff /* output buffer */)
{
	int nStatus;
	struct sg_header grSgHdr;
	struct sg_header grSgHdrIn;
	unsigned char *pDataBuffer;

	/* saftey checks */
	if ( (cmd_len == 0) || !cdb )
		return  -1;

	if( !output_buff)
		output_size = 0;

    if (!bSetTimeOut)
    {
		/* Time out of 30 seconds */
		if( ioctl( ScsiFd, SG_SET_TIMEOUT, &nTimeOut) < 0)
		{
        	perror( "Setting time out failed: ");
      	}
      	else
			bSetTimeOut = TRUE;
	}

	/* Generate generic scsi command */
	memset( &grSgHdr, 0, sizeof(struct sg_header));
	grSgHdr.reply_len = SG_HDR_SIZE + output_size;
	grSgHdr.twelve_byte = cmd_len == 12;
	grSgHdr.result = 0;

	pDataBuffer = (unsigned char *)NdMalloc( SG_HDR_SIZE + cmd_len+input_size );
	memcpy(pDataBuffer, &grSgHdr, SG_HDR_SIZE);
	memcpy(&pDataBuffer[SG_HDR_SIZE] , cdb, cmd_len);

	if( input_size  )
		memcpy(&pDataBuffer[SG_HDR_SIZE+cmd_len], input_buff, input_size);

	nStatus = write(ScsiFd, pDataBuffer, SG_HDR_SIZE + input_size + cmd_len);

	memcpy(&grSgHdr, pDataBuffer, SG_HDR_SIZE);
	if( (nStatus < 0) || ((unsigned)nStatus != SG_HDR_SIZE + input_size + cmd_len) ||
		( grSgHdr.result) )
	{
		fprintf(stderr, "\nError sending SCSI command\n");
		perror("");
		return nStatus;
	}

	/* read the result */
	if (output_size > input_size + cmd_len)
	{
		FreePointer( &pDataBuffer );
		pDataBuffer = (unsigned char *)NdMalloc( SG_HDR_SIZE + output_size );
	}

	nStatus = read( ScsiFd, pDataBuffer, SG_HDR_SIZE + output_size);
	memcpy(&grSgHdr, pDataBuffer, SG_HDR_SIZE);
if( (nStatus < 0) || ((unsigned)nStatus != SG_HDR_SIZE + output_size) ||
		( grSgHdr.result ) )
	{
		fprintf(stderr, "Error reading result of last SCSI command\n");
		if( nStatus < 0 )
			perror("");
	}
	else
	{
		/* We got what we expected to get */
		memcpy(output_buff, &pDataBuffer[SG_HDR_SIZE], output_size);
		nStatus = 0;
	}
	FreePointer( &pDataBuffer );
	return nStatus;
}


/***************************************************************************

Routine:           LoadSCSIParams

Inputs:
    None

Returns:
    boolean               : TRUE if successful

Description:
    Reads the SCSI device parameters from the optotrak.ini file

***************************************************************************/
static boolean LoadSCSIParams( void )
{
    return ReadAppParms( "optotrak", "SCSI", grSCSIParms );
}


/***************************************************************************

Routine:           Pause

Inputs:
    None

Returns:
    void

Description:
    Pauses for a brief amount of time.

***************************************************************************/
static void Pause( void )
{
    sleep( 1 );
}

/***************************************************************************

Routine:           LinkOpen

Inputs:
    None

Returns:
    boolean               : TRUE if successful

Description:
    Opens the /dev/scsi device for communication to the OPTOTRAK
    SCSI-to-link board.
    Initializes the OPTOTRAK SCSI-to-Link.

***************************************************************************/
boolean LinkOpen( void )
{
	unsigned char CDB[6];
    /*
     * Read SCSI device file name from optotrak.ini
     */
    if( !LoadSCSIParams() )
        return FALSE;

    /*
     * Open the appropriate /dev/scsi device
     */
    if( (ScsiFd = open( szScsiPath, O_RDWR )) == -1 )
        return FALSE;


	CDB[0] = SCSI_INITIALIZE_CMD;
	CDB[1] = 0;
	CDB[2] = 0;
	CDB[3] = 0;
	CDB[4] = 0;
	CDB[5] = 0;

	    if( DoScsiReq( 6, CDB, 0, NULL, 0, NULL ) )
		return FALSE;


	    return TRUE;
	}

	/***************************************************************************

	Routine:           LinkClose

	Inputs:
	    None

	Returns:
	    boolean             : return FALSE if an error occurs.

	Description:
    Closes the /dev/scsi driver to the OPTOTRAK SCSI-to-link board.

***************************************************************************/
boolean LinkClose( void )
{
    /*
     * Was the device opened?
     */
    if( ScsiFd >= 0 )
    {
        close( ScsiFd );
        ScsiFd = -1;
    } /* if */

    return TRUE;
}

/***************************************************************************

Routine:           LinkRead

Inputs:
    void *pData           : Buffer for data
    unsigned uBytes       : Number of bytes to read

Returns:
    unsigned              : Number of bytes actually read

Description:
    LinkRead attempts to rececive "uBytes" of data from the OPTOTRAK
    SCSI-to-link interface.  The data is read into the specified buffer.

***************************************************************************/
unsigned LinkRead( void *pData, unsigned uBytes )
{
    unsigned char
        *puch;
	unsigned char
		CDB[6];
    if( uBytes > 3 )
    {
		unsigned unReceiveSize;

    	CDB[0] = SCSI_RECEIVE_CMD;
    	CDB[1] = 0;
    	CDB[2] = (uBytes >> 16 ) & 0xff;
    	CDB[3] = (uBytes >> 8 ) & 0xff;
    	CDB[4] = uBytes & 0xfc;
    	CDB[5] = 0;

		unReceiveSize = uBytes & 0xfffffc;
        if( DoScsiReq(6, CDB, 0, NULL,unReceiveSize, pData) )
            return 0;

		/* memcpy(pData, grchReadBuffer + SG_HDR_SIZE, unReceiveSize);*/
    } /* if */

    if( uBytes & 3 )
    {
        unsigned char gruchBuffer[4];
        puch = pData;
        puch += (uBytes & 0xfffffc );


    	CDB[0] = SCSI_RECEIVE_PADDED_CMD;
    	CDB[1] = 0;
    	CDB[2] = 0;
    	CDB[3] = 0;
    	CDB[4] = uBytes & 0x03;
    	CDB[5] = 0;

        if( DoScsiReq(6,CDB, 0, NULL,/*uBytes&0x03*/ 4 , gruchBuffer) )
            return 0;

        /* Success we need to copy in the bytes read */
        memcpy(puch, gruchBuffer, uBytes & 0x03);

    } /* if */

    return uBytes;
}

/***************************************************************************

Routine:           LinkWrite

Inputs:
    void *pData           : Data buffer to be written to the link
    unsigned uBytes       : Number of bytes to write

Returns:
    unsigned              : Number of bytes actually written.

Description:
    LinkWrite attempts to send a block of "uBytes" of data to the
    OPTOTRAK SCSI-to-link interface.

***************************************************************************/
unsigned LinkWrite( void *pData, unsigned uBytes )
{
    unsigned char
        *puch;
	unsigned char CDB[6];

    if( uBytes > 3 )
    {
		/* Generate the SCSI command */
    	CDB[0] = SCSI_SEND_CMD;
    	CDB[1] = 0;
    	CDB[2] = (uBytes >> 16 ) & 0xff;
    	CDB[3] = (uBytes >> 8 ) & 0xff;
    	CDB[4] = uBytes & 0xfc;
    	CDB[5] = 0;


        if( DoScsiReq( 6,CDB, uBytes & 0xfffffc, pData, 0, NULL ) )
            return 0;
    } /* if */

    if( uBytes & 3 )
    {
		unsigned char gruchBuff[4];
        puch = pData;
        puch += (uBytes & 0xfffffc );


		/* Generate the SCSI command */
    	CDB[0] = SCSI_SEND_PADDED_CMD;
    	CDB[1] = 0;
    	CDB[2] = 0;
    	CDB[3] = 0;
    	CDB[4] = uBytes & 0x03;
    	CDB[5] = 0;

 		memset(gruchBuff, 0, 4);
		memcpy(gruchBuff, puch, uBytes & 0x03 );

        if( DoScsiReq( 6,CDB, 4, gruchBuff, 0, NULL ) )
            return 0;

    } /* if */

    return uBytes;
}

/***************************************************************************

Routine:           LinkStatus

Inputs:
    boolean *pbReadable   : Pointer to readable flag
    boolean *pbWriteable  : Pointer to writeable flag

Returns:
    boolean               : Returns FALSE if an error occurs

Description:
    LinkStatus determines whether or not the SCSI-to-link interface
    has any data available for the host computer, and if so sets
    *pbReadable to TRUE (else FALSE)

    It also determines if the OPTOTRAK is ready to receive more data,
    and if so, sets *pbWriteable to TRUE (else FALSE).

***************************************************************************/
boolean LinkStatus( boolean *pbReadable, boolean *pbWriteable )
{
    unsigned char
        grchBuff[4];
	unsigned char CDB[6];

    CDB[0] = SCSI_INPUT_STATUS_CMD;
    CDB[1] = 0;
    CDB[2] = 0;
    CDB[3] = 0;
    CDB[4] = 4;
    CDB[5] = 0;

    if( DoScsiReq( 6,CDB, 0, NULL, 4, grchBuff) )
    {
        *pbWriteable = *pbReadable = FALSE;
        return FALSE;
    }
    else
    {
        *pbWriteable = grchBuff[3] ? TRUE : FALSE;
        *pbReadable = (grchBuff[0] ||
						grchBuff[1]) ? TRUE : FALSE;
    }
    return TRUE;
}

/***************************************************************************

Routine:           LinkReset

Inputs:
    None

Returns:
    void

Description:
    LinkReset performs a RESET of the OPTOTRAK system.  The LinkReset
    routine will prepare the OPTOTRAK for downloading.

***************************************************************************/
boolean LinkReset( void )
{
	unsigned char CDB[6];

	CDB[0] = SCSI_RESET_CMD;
	CDB[1] = 0;
	CDB[2] = 0;
	CDB[3] = 0;
	CDB[4] = 0;
	CDB[5] = 0;


    if( DoScsiReq( 6, CDB, 0, NULL, 0, NULL) )
        return FALSE;

    Pause();


	CDB[0] = SCSI_INITIALIZE_CMD;
	CDB[1] = 0;
	CDB[2] = 0;
	CDB[3] = 0;
	CDB[4] = 0;
	CDB[5] = 0;


    if( DoScsiReq( 6,CDB,  0, NULL, 0, NULL) )
        return FALSE;

    Pause();

    return TRUE;
}


/***************************************************************************

Routine:           ScsiInquiry

Inputs:
    char *szScsi		: the SCSI device to open for the inqury
						  NOTE: if there is already a SCSI device open
							    then that device is used.

Returns:
    char *				: result of the inquiry command.

Description:
    Used to find version info from the scsi device
***************************************************************************/
char *ScsiInquiry(void)
{
	char szInqBuffer[SCSI_INQUIRY_REPLY_LEN];
	char *szReturnBuffer;
	unsigned char CDB[6];

	CDB[0] = SCSI_INQUIRY_CMD;
	CDB[1] = 0;
	CDB[2] = 0;
	CDB[3] = 0;
	CDB[4] = SCSI_INQUIRY_REPLY_LEN;
	CDB[5] = 0;

	if( DoScsiReq(6,CDB, 0, NULL, SCSI_INQUIRY_REPLY_LEN, szInqBuffer) )
	{
		fprintf(stderr, "Inquiry Command failed\n");
		return NULL;
	}

	szReturnBuffer = (char *)NdMalloc( SCSI_INQUIRY_REPLY_LEN +1 );
	memset(szReturnBuffer, 0, SCSI_INQUIRY_REPLY_LEN +1);

	memcpy(szReturnBuffer, szInqBuffer, SCSI_INQUIRY_REPLY_LEN);
	return szReturnBuffer;
}
