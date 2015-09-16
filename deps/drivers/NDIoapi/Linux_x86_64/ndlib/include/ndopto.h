/***************************************************************

     Name:            NDOPTO.H

     Description:
     Main OPTOTRAK API include file.

	DO NOT MODIFY THIS FILE.  It is generated from component sources.
	
	Internal development should use the non-aggregate form.

Build : 
	3.05.-.7
	
Date  : 
	04/24/2008-09:00

	Copyright (C) 1983-2005, Northern Digital Inc. All rights reserved.

****************************************************************/

#ifndef INC_NDOPTO_H
#define INC_NDOPTO_H


/***************************************************************

 Name:    ndopto_ext.h
    
 macros to redefine external old-style NDLIB OAPI calls to the new style wrapper functions.

 NOTE: this file is restricted to the exported NDOPTO.H only, and should not be included internally.
	
****************************************************************/

/*--------------------------------------------------------------------*/
/* following information is declared in ndfiles.h for internal use. */

/* Constants for raw files which can be converted. */
#define OPTOTRAK_RAW    1
#define ANALOG_RAW      2

/* Constants for modes in which files can be opened. */
#define OPEN_READ       1
#define OPEN_WRITE      2

#define NDFILE_USER_ERROR_CODE 2000 //cloned from OPTO_USER_ERROR_CODE
#define NDFILE_SYSTEM_ERROR_CODE 1000 // cloned from OPTO_SYSTEM_ERROR_CODE

/* Structure embedded in the data file header system comments for analog and digital data files */
#define ANALOG_HEADER_BLOCK (254 * 256)
typedef struct AnalogHeaderStruct
{
    unsigned uBlockId;           /* Set to ANALOG_HEADER_BLOCK */
    char     szName[15];         /* Name to call this device by */
    char     szDescriptFile[9];  /* Desc. file when not just straight analog channels */
    int      nOffset;            /* Offset value to subtract from raw reading */
    float    fScale;             /* Multiply reading by this to convert to volts */
    int      nDigitalChannels;   /* Number of Digital channels */
} AnalogHeaderType;

/* preceding information is declared in ndfiles.h for internal use. */
/*--------------------------------------------------------------------*/


//ndopto_ext.h

/***************************************************************
Name:            APP_PARM.H

Description:
    Header file for APP_PARM C file.
    This module contains routines for reading and saving
    application parameters.

****************************************************************/

#ifndef INCLUDE_APP_PARM_H
#define INCLUDE_APP_PARM_H

#ifdef CC_TRACE
#pragma message ("include "__FILE__)
#endif

#include <ndhost.h>

#define APP_PARM_STRING     0x0001
#define APP_PARM_INT        0x0002
#define APP_PARM_UNSIGNED   0x0004
#define APP_PARM_LONG       0x0008
#define APP_PARM_ULONG      0x0010
#define APP_PARM_FLOAT      0x0020
#define APP_PARM_DOUBLE     0x0040
#define APP_PARM_BOOLEAN    0x0080
#define APP_PARM_SHORT      0x0100
#define APP_PARM_CHAR		0x0200
//flags
#define APP_PARM_NORANGECHK 0x0800
#define APP_PARM_OPTIONAL   0x1000
#define APP_PARM_COUNT      0x3000  /* Always an optional parameter */
#define APP_PARM_HEX		0x4000

// These are used internally by ReadAppParms().  Do not use.
#define APP_PARM_TYPES      0x03ff
#define APP_PARM_NUMERIC    0x037e
#define APP_PARM_UNREAD     0x8000

//size specific aliases.
#define APP_PARM_UINT8		APP_PARM_CHAR
#define APP_PARM_INT16		APP_PARM_SHORT
#define APP_PARM_INT32		APP_PARM_INT
#define APP_PARM_UINT32		APP_PARM_UNSIGNED

typedef struct
{
    char        *pszParam;
    void        *pBuffer;
    unsigned    uBuffLen;
    unsigned    uFlags;
    float       fMinVal;
    float       fMaxVal;
} AppParmInfo;

typedef struct
{
    char        *pszParam;
    void        *pBuffer;
    unsigned    uBuffLen;
    unsigned    uFlags;
    double      fMinVal;
    double      fMaxVal;
} AppParmInfo_d;


CPLUSPLUS_START
boolean ReadAppParms( const char *pszParmFile, const char *pszSection, AppParmInfo *pParmInfo );
boolean ReadAppParms_d( const char *pszParmFile, const char *pszSection, AppParmInfo_d *pParmInfo );
boolean SaveAppParms( char *pszParmFile, char *pszSection, AppParmInfo *pParmInfo );
boolean SaveAppParms_d( char *pszParmFile, char *pszSection, AppParmInfo_d *pParmInfo );
boolean DeleteAppParms( char *pszParmFile, char *pszSection );
CPLUSPLUS_END

#endif


/*****************************************************************
Name:			general.h

Description:
	General	section	describing Optotrak	interface.

Creator:		Christian C. Monterroso		05/2005

Modified:

	EC-05-0116		CCM	06/2005
	Use	new	NDLIB containing new math models, including	new
	camera parameter and rigid body	structures.

Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary.
Not	for	distribution outside of	NDI	without	proper authorization.


*****************************************************************/

#ifndef	INC__GENERAL_H
#define	INC__GENERAL_H

/*
 * Constants defining limits on	certain	objects	in the OPTOTRAK	system.
 */
#define	MAX_SENSORS						 10
#define	MAX_OPTOSCOPES					  4
#define	MAX_RIGID_BODIES				 10
#define	MAX_HOST_RIGID_BODIES			170		/* ( OPTOTRAK_MAX_MARKERS /	3 )	*/
#define	MAX_SENSOR_NAME_LENGTH			 20

#if	defined(PRODUCT_CERTUS)
#define	OPTOTRAK_MAX_MARKERS			512
#else
#define	OPTOTRAK_MAX_MARKERS			256	 /*	3020 */
#endif

#define	OPTOTRAK_INTERNAL_DATA_SOURCES	  8

/* Note: All numbers are reversed (last	character first) */
#define	OPTO_HW_TYPE_SENSOR				0x4D41435FL		/* '_CAM' */
#define	OPTO_HW_TYPE_SU					0x55535953L		/* 'SYSU' */
#define	OPTO_HW_TYPE_ODAU				0x5541444FL		/* 'ODAU' */
#define	OPTO_HW_TYPE_REALTIME			0x4D544C52L		/* 'RLTM' */
#define	OPTO_HW_TYPE_CERTUS_SENSOR		0x43323258L		/* 'X22C' */
#define	OPTO_HW_TYPE_OPTOTRAK_PLUS_PS	0x554C504FL		/* 'OPLU' */
#define	OPTO_HW_TYPE_OPTOTRAK_PRO_PS	0x4F52504FL		/* 'OPRO' */
#define	OPTO_HW_TYPE_CERTUS_SU			0x53323258L		/* 'X22S' */
#define	OPTO_HW_TYPE_IMAGE_SENSOR		0x49323258L		/* 'X22I' */
#define	OPTO_HW_TYPE_ACC_BRD			0x42434341L		/* 'ACCB' */
#define	OPTO_HW_TYPE_STROBER			0x42525453L		/* 'STRB' */

#define OPTO_HW_REV_CERTUS_SENSOR		6
#define OPTO_HW_REV_CERTUS_HD_SENSOR	7
#define OPTO_HW_REV_CATALYST_SENSOR		8
#define OPTO_HW_REV_SERIES_300_SENSOR	300
#define OPTO_HW_REV_SERIES_1200_SENSOR	1200
#define OPTO_HW_REV_SERIES_2000_SENSOR	2000

// This	is needed in many places and not defined elsewhere in the linux	build.
#ifndef	_MAX_FNAME
#define	_MAX_FNAME		1024
#endif

/*****************************************************************
Static Typedefs	and	Structures
*****************************************************************/
struct OptoNodeInfoStruct
{
	unsigned long	ulHWType,
					ulHWRev,
					ulMemSize;
	char			szFreezeId[	80 ],
					szSwDesc[ 80 ],
					szSerialNo[	32 ];
};

/*
 * These are flags and constants used by OPTOTRAK and OPTOSCOPE	for	any	of
 * the buffer messages
 */
#define	OPTO_BUFFER_OVERRUN_FLAG		0x0001
#define	OPTO_FRAME_OVERRUN_FLAG			0x0002
#define	OPTO_NO_PRE_FRAMES_FLAG			0x0004
#define	OPTO_SWITCH_DATA_CHANGED_FLAG	0x0008
#define	OPTO_TOOL_CONFIG_CHANGED_FLAG	0x0010
#define	OPTO_FRAME_DATA_MISSED_FLAG		0x0020

#define	OPTO_SWITCH_CONFIG_CHANGED_MASK	(OPTO_SWITCH_DATA_CHANGED_FLAG | OPTO_TOOL_CONFIG_CHANGED_FLAG)

#endif

/*****************************************************************
Name: 			dataprop.h

Description:
	Details general interface to the data proprietor

Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary. 
Not for distribution outside of NDI without proper authorization.
*/


/*
 * Messages specific to the OPTOTRAK data proprietor
 */
 
#define WAVEFORM_MODE_WAVEFORM_WIDTH	31

struct optotrak_wave_head_struct
{
	float       centroid;       /* Calculated centroid */
	char        peak_value;     /* Peak value at peak pixel */
	char        gain;           /* Gain setting for this collection */
	char        error;          /* Error in centtroid calculation */
	char        dummy;
	unsigned    start_pixel;	/* Pixel number of first pixel in wave */
};

#define WAVEFORM_DATA_WIDTH				31
#define WAVEFORM_DATA_HALF_WIDTH		15


#define OPTO_TRANSFORM_DATA_SIZE           8
struct OptoTransformDataStruct
{
	long int markers;       /* Total # of markers to convert */
	long int FullDataInfo;  /* Data contains peak info as well */
};


/***************************************************************
Name: ODAU       -Section detailing interfacing with the ODAU unit


Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary. 
Not for distribution outside of NDI without proper authorization.


****************************************************************/
#define OPTOSCOPE_MAX_CHANNELS_IN       16
#define OPTOSCOPE_MIN_CHANNEL_NUM        1
#define OPTOSCOPE_MAX_TABLE_ENTRIES     OPTOSCOPE_MAX_CHANNELS_IN + 1

/*
 * SCO-95-0015
 * Added    ODAU_DIGITAL_OFFB_MUXA
 *          ODAU_DIGITAL_INPB_MUXA
 *          ODAU_DIGITAL_OUTPB_MUXA
 * Changed constants to act as 2 4bit flags fields:
 *      Hex     Binary      Meaning
 *      x00     0000        Port off
 *      x01     0001        Port set to Input
 *      x02     0010        Port set to output
 *      x04     0100        Port set for MUXER
 *          
 */
#define ODAU_DIGITAL_PORT_OFF     0x00
#define ODAU_DIGITAL_INPB_INPA    0x11
#define ODAU_DIGITAL_OUTPB_INPA   0x21
#define ODAU_DIGITAL_OUTPB_OUTPA  0x22
#define ODAU_DIGITAL_OFFB_MUXA    0x04
#define ODAU_DIGITAL_INPB_MUXA    0x14
#define ODAU_DIGITAL_OUTPB_MUXA   0x24

/*
 * SCO-97-0020
 * New constants for user timer mode
 */
#define ODAU_TIMER_ONE_SHOT     1
#define ODAU_TIMER_RATE_GEN     2
#define ODAU_TIMER_SQUARE_WAVE  3
#define ODAU_TIMER_RETRIGGER    4

/*
 * SCO-97-0020
 *  New constants for selecting which timer to set
 */
#define ODAU_TIMER2             2

/*
 * Constants used to test ODAU digital port bits
 */
#define ODAU_DGTL_PT_OFF          0x00
#define ODAU_DGTL_PT_IN           0x01
#define ODAU_DGTL_PT_OUT          0x02
#define ODAU_DGTL_PT_MUX          0x04
#define ODAU_DGTL_INPUT           0x11

/* OPTOSCOPE_SETUP_COLL_MESSAGE
 * OPTOSCOPE_SETUP_COLL_COMMAND
 * SCO-95-0015
 * Added ulFlags field
 *      Flag ODAU_DIFFERENTIAL_FLAG defined
 * Added fScanFrequency field.
 */

struct OptoscopeSetupCollStruct
{
    long int        DataId;
    float           fFrameFrequency;
    float           fScanFrequency;
    long int        AnalogChannels;
    long int        AnalogGain;
    long int        DigitalMode;
    float           CollectionTime;
    float           PreTriggerTime;
    long int        StreamData;
    unsigned long   ulFlags;
};

#define ODAU_DIFFERENTIAL_FLAG  0x0001

/*
 * The following is a global structure is passed from the OPTOSCOPE the the
 * OPTOTRAK so that it may record the required collection information.
 */
struct ScopeCollectionParmsStruct
{
    long int    DataId;
    float       fFrequency;
    long int    PacketSize;
};

/* OPTOSCOPE_START_BUFF_MESSAGE
 * OPTOSCOPE_START_BUFF_COMMAND
 *
 * OPTOSCOPE_STOP_BUFF_MESSAGE
 * OPTOSCOPE_STOP_BUFF_COMMAND
 *
 * OPTOSCOPE_SHUTDOWN_MESSAGE
 * OPTOSCOPE_SHUTDOWN_COMMAND
 */

struct OptoscopeSetChnlAllStruct
{
    long int    status;
    long int    gain;
};

struct OptoscopeSetChnlSglStruct
{
    long int    status;
    long int    gain;
    long int    channel;
};

struct OptoscopeSetGainAllStruct
{
    long int    gain;
};

struct OptoscopeSetGainSglStruct
{
    long int    gain;
    long int    channel;
};

/* OPTOSCOPE_STATUS_MESSAGE
 * SCO-95-0015
 * Added ulFlags field
 * Added fScanFrequency field.
 */

struct OptoscopeStatusStruct
{
    long int        DataId;
    float           fFrameFrequency;
    float           fScanFrequency;
    long int        AnalogGain;
    long int        AnalogChannels;
    long int        DigitalMode;
    float           CollectionTime;
    float           PreTriggerTime;
    long int        StreamData;
    long int        ErrorFlags;
    unsigned long   ulFlags;
};

/*
 * SCO-95-0015
 * OPTOSCOPE_ANALOG_OUT_MESSAGE
 */
struct OptoscopeAnalogOutStruct
{
    float           fCh1Volts,
                    fCh2Volts;
    unsigned long   ulUpdateMask;
};

/*
 * SCO-95-0015
 * OPTOSCOPE_DIGITAL_OUT_MESSAGE
 */
struct OptoscopeDigitalOutStruct
{
    unsigned long   ulDigitalOut,
                    ulUpdateMask;
};

/*
 * SCO-97-0020
 * OPTOSCOPE_SET_TIMER_MESSAGE
 */
struct OptoscopeTimerDataStruct
{
    unsigned long   ulTimer,
                    ulMode,
                    ulVal;
};

/*
 * SCO-97-0020
 *  OPTOSCOPE_LOAD_FIFO_MESSAGE
 */
struct OptoscopeFiFoDataStruct
{
    unsigned long   ulEntries,
                    grulEntries[256];
};

/*
 * SCO-97-0020
 *   New data structure for OdauLoadControlFiFo()
 */
struct OdauControlWordStruct
{
    boolean         bDigitalInput,
                    bDifferential;
    unsigned        uGain,
                    uMuxer,
                    uChannel;
};
/***************************************************************

Name: REALTIME
      Details advanced data properitor calls for realtime option

****************************************************************/

/* These are flags which are used with rigid bodies and the determination of their transformations. */
#define OPTOTRAK_UNDETERMINED_FLAG      0x0001
#define OPTOTRAK_STATIC_XFRM_FLAG       0x0002
#define OPTOTRAK_STATIC_RIGID_FLAG      0x0004
#define OPTOTRAK_CONSTANT_RIGID_FLAG    0x0008
#define OPTOTRAK_NO_RIGID_CALCS_FLAG    0x0010
#define OPTOTRAK_DO_RIGID_CALCS_FLAG    0x0020
#define OPTOTRAK_QUATERN_RIGID_FLAG     0x0040	/* MUST always be the same as RB_USE_QUATERN_RIGID_FLAG */
#define OPTOTRAK_ITERATIVE_RIGID_FLAG   0x0080	/* MUST always be the same as RB_USE_ITERATIVE_RIGID_FLAG */
#define OPTOTRAK_SET_QUATERN_ERROR_FLAG 0x0100
#define OPTOTRAK_SET_MIN_MARKERS_FLAG   0x0200
#define OPTOTRAK_RIGID_ERR_MKR_SPREAD   0x0400
#define OPTOTRAK_USE_SCALED_MAX3DERROR	0x0800	/* MUST always be the same as RB_USE_SCALED_MAX3DERROR */
#define OPTOTRAK_RETURN_QUATERN_FLAG    0x1000
#define OPTOTRAK_RETURN_MATRIX_FLAG     0x2000
#define OPTOTRAK_RETURN_EULER_FLAG      0x4000

#define SIZEOF_OPTOTRAK_ADD_RIGID_STRUCT		24

typedef struct OptotrakAddRigidStruct
{
    long int lnRigidId;         /* Unique identifier for rigid body. */
    long int lnStartMarker;     /* Start marker for rigid body. */
    long int lnNumberOfMarkers; /* Number of markers in rigid body. */
    long int lnMinimumMarkers;  /* Minimum number of markers for calcs. */
    float    fMax3dError;       /* Max allowable quaternion error. */
    long int lnFlags;           /* Flags for this rigid body. */
} OptotrakAddRigid;


#define SIZEOF_OPTOTRAK_DEL_RIGID_STRUCT		4

typedef struct OptotrakDelRigidStruct
{
    long int lnRigidId;     /* Unique identifier for rigid body. */
} OptotrakDelRigid;


#define SIZEOF_OPTOTRAK_SET_RIGID_STRUCT		32

typedef struct OptotrakSetRigidStruct
{
    long int lnRigidId;            /* Unique identifier for rigid body. */
    long int lnMarkerAngle;
    long int lnMinimumMarkers;     /* Minimum number of markers for calcs. */
    float    fMax3dError;
    float    fMaxSensorError;
    float    fMax3dRmsError;
    float    fMaxSensorRmsError;
    long int lnFlags;              /* New status flags for this rigid body. */
} OptotrakSetRigid;


#define SIZEOF_OPTOTRAK_ROTATE_RIGIDS_STRUCT	32

typedef struct OptotrakRotateRigidsStruct
{
    long int        lnRotationMethod; /* Flags to control xfrm rotations. */
    long int        lnRigidId;        /* Rigid body to base xfrm rotations on. */
    transformation  dtEulerXfrm;      /* XFRM to base xfrm rotations on. */
} OptotrakRotateRigids;


#define SIZEOF_OPTOTRAK_ADD_NORMALS_STRUCT		4

typedef struct OptotrakAddNormalsStruct
{
    long int lnRigidId;         /* Unique identifier for rigid body. */
} OptotrakAddNormals;


typedef struct OptotrakRigidStatusStruct
{
    long int lnRigidBodies;     /* Number of rigid bodies in use */
} OptotrakRigidStatus;


#define OPTO_RIGID_HEADER_SIZE            16
typedef struct OptotrakRigidHeaderStruct
{
    long int    NumberOfRigids;   /* number of transforms following header */
    long int    StartFrameNumber; /* frame number of 3D data used */
    long int    flags;            /* current flag settings for OPTOTRAK */
	long int 	NumberOfSwitchDataBytes; /* Number of switch data bytes */
} OptotrakRigidHeader;


union TransformationUnion
{
    RotationTransformation          rotation;
    transformation                  euler;
    QuatTransformation              quaternion;
};


typedef struct OptotrakRigidStruct
{
    long int                    RigidId;         /* rigid body id xfrm is for */
    long int                    flags;           /* flags for this rigid body */
    float                       QuaternionError; /* quat rms error for xfrm */
    float                       IterativeError;  /* iter rms error for xfrm */
    union TransformationUnion   transformation;  /* latest calculated xfrm */
} OptotrakRigid;

/*realtime.h*/

/*****************************************************************
Name:			admin.h

Description:
	Defines for commands to pass to the Optotrak Administrator.

Creator:		Christian C. Monterroso		05/2005


Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary. 
Not for distribution outside of NDI without proper authorization.

*****************************************************************/

/*
 * Flag definitions used in the OPTOTRAK_SETUP_COLL_MESSAGE.
 * Note: don't use the flag 0x1000 as it is taken by the constant
 *       OPTOTRAK_REALTIME_PRESENT_FLAG.
 */
#define OPTOTRAK_NO_INTERPOLATION_FLAG  		0x00000001
#define OPTOTRAK_FULL_DATA_FLAG         		0x00000002
#define OPTOTRAK_PIXEL_DATA_FLAG        		0x00000004
#define OPTOTRAK_MARKER_BY_MARKER_FLAG  		0x00000008
#define OPTOTRAK_ECHO_CALIBRATE_FLAG    		0x00000010
#define OPTOTRAK_BUFFER_RAW_FLAG        		0x00000020
#define OPTOTRAK_NO_FIRE_MARKERS_FLAG   		0x00000040
#define OPTOTRAK_STATIC_THRESHOLD_FLAG  		0x00000080
#define OPTOTRAK_WAVEFORM_DATA_FLAG     		0x00000100
#define OPTOTRAK_AUTO_DUTY_CYCLE_FLAG   		0x00000200
#define OPTOTRAK_EXTERNAL_CLOCK_FLAG    		0x00000400
#define OPTOTRAK_EXTERNAL_TRIGGER_FLAG  		0x00000800
/* RESERVED										0x00001000 */
#define OPTOTRAK_GET_NEXT_FRAME_FLAG    		0x00002000
#define OPTOTRAK_SWITCH_AND_CONFIG_FLAG 		0x00004000
#define OPTOTRAK_USE_COLPARMS_ONLY_FLAG			0x00008000
/* RESERVED										0x00010000 */
/* RESERVED										0x00020000 */
/* RESERVED										0x00040000 */
/* RESERVED										0x00080000 */
/* RESERVED										0x00100000 */
/* RESERVED										0x00800000 */
/* RESERVED										0x01000000 */
/* RESERVED										0x02000000 */
/* RESERVED										0x04000000 */
/* RESERVED										0x08000000 */
/* RESERVED										0x10000000 */
/* RESERVED										0x20000000 */

/*
 * The following flags are set by the OPTOTRAK system itself.
 * They indicate (1) if the system has revision D/E Sensors,
 *               (2) if the system can perform real-time rigid bodies.
 *               (3) if the markers are on in the system.
 *               (4) if the system is in wireless mode
 */
#define OPTOTRAK_REVISIOND_FLAG         0x80000000
#define OPTOTRAK_3020_FLAG				0x80000000
#define OPTOTRAK_REVISION_X22_FLAG      0x40000000
#define OPTOTRAK_CERTUS_FLAG			0x40000000
#define OPTOTRAK_RIGID_CAPABLE_FLAG     0x08000000
#define OPTOTRAK_MARKERS_ACTIVE         0x04000000
#define OPTOTRAK_WIRELESS_ENABLED_FLAG	0x00400000

struct OdauSetupStruct
{
	float       fOdauFrequency;
	long int    OdauChannels;
	long int    OdauGain;
	long int    OdauFlags;
};

struct OptotrakSetupCollStruct
{
	long int    DataId;
	long int    NumMarkers;
	float       fFrameFrequency;
	float       CollectionTime;
	float       PreTriggerTime;
	long int    StreamData;
	long int    flags;
	float       fOdauFrequency;
	long int    OdauChannels;
	long int    OdauGain;
	long int    OdauFlags;
};



struct OptotrakStroberTableStruct
{
	long int    Port1;
	long int    Port2;
	long int    Port3;
	long int    Port4;
};


/*admin.h*/
/*****************************************************************

Name:               sensorstatus.h


Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary.
Not for distribution outside of NDI without proper authorization.


*****************************************************************/
#ifndef INC__SENSORSSTATUS_H
#define INC__SENSORSSTATUS_H

#ifdef CC_TRACE
#pragma message ("include "__FILE__)
#endif

#define MAX_BUFFER_SIZE                 512
#define OPTIMAL_PEAK                    200

#ifndef CENTROID_OK
#define CENTROID_OK                     0
#define CENTROID_WAVEFORM_TOO_WIDE      1
#define CENTROID_PEAK_TOO_SMALL         2
#define CENTROID_PEAK_TOO_LARGE         3
#define CENTROID_WAVEFORM_OFF_DEVICE    4
#define CENTROID_FELL_BEHIND            5
#define CENTROID_LAST_CENTROID_BAD      6
#define CENTROID_BUFFER_OVERFLOW        7
#define CENTROID_MISSED_CCD             8
#define CENTROID_BAD_CRC                9
#define CENTROID_INVALID_CORR_REQ      10
#define CENTROID_UNINITIALIZED		   11
#define CENTROID_HAS_REFLECTION        12
#endif

/*****************************************************************
External Variables and Routines
*****************************************************************/
#define SIZEOF_SENSOR_STATUS_STRUCT        ( 4 * PC_CHAR_SIZE )
typedef struct SensorStatusStruct
{
    char chPeak;
    char chDRC;
    char chError;
    char chNibble;
} SensorStatus;

typedef struct  CentroidBufferStruct
{
    float       centroid;             /* Calculated centroid */
    char        Peak;                 /* Peak value 0 to 255 */
    char        gain;                 /* Gain setting for this collection */
    char        ErrorCode;            /* Error in centtroid calculation */
    char        PeakNibble;           /* Low Nibble of Peak value (Revd only) */
} CentroidBuffer;

#endif

/*****************************************************************
Name:               DHANDLES.H

Description:
    This header file contains the defines and structures
	required for device handles.

Modifications:
    EC-02-0548	CCM	11/02


	EC-05-0116		CCM	06/2005
	Use new NDLIB containing new math models, including new
	camera parameter and rigid body structures.

Copyright (C) 1992-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary.
Not for distribution outside of NDI without proper authorization.

*****************************************************************/
#ifndef __OAPI_DHANDLES_H__
#define __OAPI_DHANDLES_H__

/*****************************************************************
Defines
*****************************************************************/
#define DEVICE_MAX_PROPERTY_STRING		256
#define DEVICE_MAX_STROBER_PORTS_CERTUS	3
#define DEVICE_MAX_SWITCHES             4
#define DEVICE_MAX_VLEDS                4
#define DEVICE_MAX_BEEPERS              1
#define DEVICE_MAX_MARKERS              255

#define MAX_SROM_SIZE 		1024

/* DH_PROPERTY_STATE_FLAGS bitfield definitions */
#define DH_STATE_MARKER_CLUSTER_CHANGED_FLAG		0x00000001
#define DH_STATE_BUMP_OCCURRED_FLAG					0x00000002


/* Device type definitions */
#define DH_DEVICE_TYPE_TSCAN_CONTROLLER				0x00
#define DH_DEVICE_TYPE_TARGET_CONTROLLER			0x01
#define DH_DEVICE_TYPE_OPTOTRAK_PROBE_CONTROLLER	0x02
#define DH_DEVICE_TYPE_AUTOSCALE_TARGET_CONTROLLER	0x03
#define DH_DEVICE_TYPE_MULTISIDED_PROBE_CONTROLLER	0x04
#define DH_DEVICE_TYPE_TSCAN2_CONTROLLER			0x05
#define DH_DEVICE_TYPE_SMART_MARKER_CONTROLLER		0x06
#define DH_DEVICE_TYPE_TRANSMITTER_CONTROLLER		0x07
#define DH_DEVICE_TYPE_TOOL							0xFD
#define DH_DEVICE_TYPE_UNKNOWN						0xFF


enum DeviceHandleStatus
{
	DH_STATUS_UNOCCUPIED = 0,
	DH_STATUS_OCCUPIED = 1,
	DH_STATUS_INITIALIZED = 2,
	DH_STATUS_ENABLED = 3,
	DH_STATUS_FREE = 4
}; /* enum DeviceHandleStatus */

enum DeviceHandlePropertyType
{
	DH_PROPERTY_TYPE_INT = 0,
	DH_PROPERTY_TYPE_FLOAT = 1,
	DH_PROPERTY_TYPE_DOUBLE = 2,
	DH_PROPERTY_TYPE_STRING = 3,
	DH_PROPERTY_TYPE_CHAR = 4,
	DH_PROPERTY_TYPE_LONG = 5,
	DH_PROPERTY_TYPE_UNKNOWN = 6
}; /* enum DeviceHandlePropertyType */

enum DeviceHandleProperties
{
	DH_PROPERTY_UNKNOWN = 0,
	DH_PROPERTY_NAME = 1,
	DH_PROPERTY_MARKERSTOFIRE = 2,
	DH_PROPERTY_MAXMARKERS = 3,
	DH_PROPERTY_STARTMARKERPERIOD = 4,
	DH_PROPERTY_SWITCHES = 5,
	DH_PROPERTY_VLEDS = 6,
	DH_PROPERTY_PORT = 7,
	DH_PROPERTY_ORDER = 8,
	DH_PROPERTY_SUBPORT = 9,
	DH_PROPERTY_FIRINGSEQUENCE = 10,
	DH_PROPERTY_HAS_ROM = 11,
	DH_PROPERTY_TOOLPORTS = 12,
	DH_PROPERTY_3020_CAPABILITY = 13,
	DH_PROPERTY_3020_MARKERSTOFIRE = 14,
	DH_PROPERTY_3020_STARTMARKERPERIOD = 15,
	DH_PROPERTY_STATUS = 16,
	DH_PROPERTIES_SROM_FIRST = 17,
	DH_PROPERTY_SROM_RIGIDMARKERS = 18,
	DH_PROPERTY_SROM_RIGIDPOSITIONS = 19,
	DH_PROPERTY_SROM_FIRINGSEQUENCE = 20,
	DH_PROPERTY_SROM_NORMALMARKERS = 21,
	DH_PROPERTY_SROM_NORMALS = 22,
	DH_PROPERTY_SROM_SERIALNUMBER = 23,
	DH_PROPERTY_SROM_PARTNUMBER = 24,
	DH_PROPERTY_SROM_MANUFACTURER = 25,
	DH_PROPTERY_SROM_TOOLTYPE = 26,
	DH_PROPERTIES_SROM_LAST = 27,
	DH_PROPERTY_STATE_FLAGS = 28,
	DH_PROPERTY_TIP_ID = 29,
	DH_PROPERTY_SCAN_MODE_SUPPORT = 30,
	DH_PROPERTY_HAS_BUMP_SENSOR = 31,
	DH_PROPERTY_WIRELESS_ENABLED = 32,  // device is operating wirelessly
	DH_PROPERTY_HAS_BEEPER = 33,
	DH_PROPERTY_DEVICE_TYPE = 34,
	DH_PROPERTY_SERIAL_NUMBER = 35,
	DH_PROPERTY_FEATURE_KEY = 36,
	DH_PROPERTY_MKR_PORT_ENCODING = 37,
	/* 38-49 Reserved for internal use (internal ref#671260)*/
	DH_PROPERTY_BEEP_PATTERN = 50,
	DH_PROPERTY_SWITCH_INDEX = 51,
	DH_PROPERTY_TIME_SINCE_LAST_RESPONSE = 52,  /* Given in seconds */
	DH_PROPERTY_MAX_MARKER_VOLTAGE = 53,
	DH_PROPERTY_SMART_MARKERS = 54
}; /* enum DeviceHandleProperties */


typedef enum VLEDState
{
    VLEDST_OFF = 0,
    VLEDST_ON = 1,
    VLEDST_BLINK = 2
} eVLEDState; /* enum VLEDState */


/*****************************************************************
Structures
*****************************************************************/

/*****************************************************************

Name:		DeviceHandle

Description:
	Basic device handle information.  Contains a device handle ID
	and the current status of the device.

	nID			integer specifying the device handle ID.
	dtStatus	current status of the device associated with this device handle.
				possible device status:
					DH_STATUS_UNOCCUPIED	device has been unplugged and should be freed
					DH_STATUS_OCCUPIED		device has been plugged and should be initialized
					DH_STATUS_INITIALIZED	device has been initialized and should be enabled
					DH_STATUS_ENABLED		device has been enabled and is ready for use
					DH_STATUS_FREE			no device associated with this device handle

*****************************************************************/
typedef struct
{
	int
		nID;
	enum DeviceHandleStatus
		dtStatus;
} DeviceHandle;


/*****************************************************************

Name:		DeviceHandleProperty

Description:
	Information about a single property for a device.

	uPropertyID		unsigned integer identifying the property stored in the structure.
					possible properties:
						DH_PROPERTY_NAME				device name
						DH_PROPERTY_MARKERSTOFIRE		number of markers to fire
						DH_PROPERTY_MAXMARKERS			maximum number of markers on the device.
														these markers can be fired in any order.
						DH_PROPERTY_STARTMARKERPERIOD	index of the first marker period within a frame
														where the first marker for this device will be fired.
						DH_PROPERTY_SWITCHES			number of switches available on the device
						DH_PROPERTY_VLEDS				number of visible LEDs available on the device
						DH_PROPERTY_PORT				strober port into which the device is
														physically located.
						DH_PROPERTY_ORDER				ordering within the strober port
						DH_PROPERTY_SUBPORT				if the device is plugged into a subport of
														another device, the index of the subport into
														which the device is plugged in.
						DH_PROPERTY_FIRINGSEQUENCE		marker firing sequence
						DH_PROPERTY_HAS_ROM				indicates if the device has SROM information
														that describes a rigid body.
						DH_PROPERTY_TOOLPORTS			number of tool ports on the device.
						DH_PROPERTY_3020_CAPABILITY		indicates if the device has 3020 strober
														capability.
						DH_PROPERTY_3020_MARKERSTOFIRE	if the device has 3020 strober capability,
														number of 3020 markers to fire.
						DH_PROPERTY_3020_STARTMARKERPERIOD	index of the marker period within a frame
															where the first 3020 marker for this device
															will be fired.
						DH_PROPERTY_STATUS				status of the device (OCCUPIED, INITIALIZED,
														ENABLED, UNOCCUPIED, FREE)
						DH_PROPERTY_SROM_RIGIDMARKERS
						DH_PROPERTY_SROM_RIGIDPOSITIONS
						DH_PROPERTY_SROM_FIRINGSEQUENCE
						DH_PROPERTY_SROM_NORMALMARKERS
						DH_PROPERTY_SROM_NORMALS
						DH_PROPERTY_SROM_SERIALNUMBER
						DH_PROPERTY_SROM_PARTNUMBER
						DH_PROPTERY_SROM_TOOLTYPE
						DH_PROPERTY_STATE_FLAGS
						DH_PROPERTY_TIP_ID
						DH_PROPERTY_SCAN_MODE_SUPPORT
						DH_PROPERTY_HAS_BUMP_SENSOR		indicates if the device has a bump sensor
						DH_PROPERTY_WIRELESS_ENABLED	indicates if wireless support is enabled
						DH_PROPERTY_HAS_BEEPER			indicates if the device has a beeper
						DH_PROPERTY_DEVICE_TYPE			device type (constant, device name can change!)
						DH_PROPERTY_SERIAL_NUMBER		device serial number
						DH_PROPERTY_FEATURE_KEY
						DH_PROPERTY_MKR_PORT_ENCODING	encoded string describing marker port configuration
														(currently only for Smart Marker Strobers)
						DH_PROPERTY_SMART_MARKERS		indicates if the device supports/uses smart markers
	dtPropertyType	specifies the type of data in which the
					property information is stored.  check this
					value to determine which member of the dtData
					union to use to retrieve the property information.
					possible property types:
						DH_PROPERTY_TYPE_INT		integer property
						DH_PROPERTY_TYPE_FLOAT		float property
						DH_PROPERTY_TYPE_DOUBLE		double property
						DH_PROPERTY_TYPE_LONG		long property
						DH_PROPERTY_TYPE_STRING		string property
						DH_PROPERTY_TYPE_CHAR		character property
	dtData			the property information for the specified property.
					the data is contained in a union.  check the
					dtPropertyType value to determine which member of
					the union should be read.
						nData		contains information for DH_PROPERTY_TYPE_INT property type.
						fData		contains information for DH_PROPERTY_TYPE_FLOAT property type.
						dData		contains information for DH_PROPERTY_TYPE_DOUBLE property type.
						lData		contains information for DH_PROPERTY_TYPE_LONG property type.
						szData		contains information for DH_PROPERTY_TYPE_STRING property type.
						cData		contains information for DH_PROPERTY_TYPE_CHAR property type.

*****************************************************************/
typedef struct
{
	unsigned int
		uPropertyID;
	enum DeviceHandlePropertyType
		dtPropertyType;
	union
	{
		int
			nData;
		float
			fData;
		double
			dData;
		long
			lData;
		char
			cData;
		char
			szData[DEVICE_MAX_PROPERTY_STRING];
	} dtData;
} DeviceHandleProperty;


/*****************************************************************

Name:		DeviceHandleInfo

Description:
	Complete device handle information.  Contains the current information
	associated with a device handle.

	pdtHandle		pointer to a DeviceHandle structure, which identifies the
					device handle ID and the status of the device.
	grProperties	array of device handle properties.  each element of the
					array contains information about a single device
					property in a DeviceHandleProperty structure
	nProperties		size of the device handle properties array.
	bRigidBody		indicates if the device can be tracked as a rigid body.

*****************************************************************/
typedef struct
{
	DeviceHandle
		*pdtHandle;
	DeviceHandleProperty
		*grProperties;
	int
		nProperties;
	boolean
		bRigidBody;
} DeviceHandleInfo;


#endif /* __OAPI_DHANDLES_H__ */
/*****************************************************************
Name:			regalg.h

Description:
	Definitions of classes and functions used to register
	and align the Optotrak system.

Creator:		Christian C. Monterroso	03/2003

Changes:
EC-02-0458
- Created regalg.h

*****************************************************************/
#ifndef __NDOPTO_REGALG_H__
#define __NDOPTO_REGALG_H__

/****************************************************************
  Defines
****************************************************************/
#define REG_NO_LOG_FILE				0
#define REG_SUMMARY_LOG_FILE		1	
#define REG_DETAILED_LOG_FILE		2

#define ALIGN_NO_LOG_FILE			0
#define ALIGN_DETAILED_LOG_FILE		2

#define CALIBRIG_NO_LOG_FILE			0
#define CALIBRIG_DETAILED_LOG_FILE		2

#define ALIGN_SUCCESS						0
#define ALIGN_ERROR_PERFORMING_ALIGNMENT	1
#define ALIGN_ERROR_LOADING_INPUT_FILES		2
#define ALIGN_ERROR_ALLOCATING_MEMORY		3
#define ALIGN_ERROR_PROCESSING_DATA			4
#define ALIGN_ERROR_PARAMETERS				5

#define REG_SUCCESS							0
#define REG_ERROR_PERFORMING_REGISTRATION	1
#define REG_ERROR_LOADING_INPUT_FILES		2
#define REG_ERROR_ALLOCATING_MEMORY			3
#define REG_ERROR_PROCESSING_DATA			4
#define REG_ERROR_WORKING_FILES				5

#define CALIBRIG_SUCCESS						0
#define CALIBRIG_ERROR_PERFORMING_CALIBRATION	1
#define CALIBRIG_ERROR_LOADING_INPUT_FILES		2
#define CALIBRIG_ERROR_PARAMETERS				3

#define CAMFILE_STANDARD		"standard.cam"
#define CAMFILE_ALIGN			"align_.cam"
#define CAMFILE_REGISTER		"register_.cam"
#define CAMFILE_CALIBRIG		"calibrig_.cam"


/****************************************************************
 Global Structures
****************************************************************/
typedef struct AlignParametersStruct
{
	char 
		szDataFile[256],
		szRigidBodyFile[256],
		szInputCamFile[256],
		szOutputCamFile[256],
		szLogFileName[256];
	int
		nLogFileLevel;
	boolean
		bInputIsRawData,
		bVerbose;
} AlignParms;

typedef struct RegisterParametersStruct
{
	char 
		szRawDataFile[256],
		szRigidBodyFile[256],
		szInputCamFile[256],
		szOutputCamFile[256],
		szLogFileName[256];

	float    
		fXfrmMaxError,
		fXfrm3dRmsError,
		fSpread1,
		fSpread2,
		fSpread3;
	int	  
		nMinNumberOfXfrms,
		nLogFileLevel;
	boolean  
		bCheckCalibration,
		bVerbose;
} RegisterParms;

typedef struct CalibrigParametersStruct
{
	char 
		szRawDataFile[256],
		szRigidBodyFile[256],
		szInputCamFile[256],
		szOutputCamFile[256],
		szLogFileName[256];
	boolean  
		bVerbose;
	int
		nLogFileLevel;
} CalibrigParms;


#endif /* __NDOPTO_REGALG_H__ */
/*****************************************************************
Name:			optoappl.h

Description:
	Definitions and prototypes used by Optotrak applications.

Modified:

Copyright (C) 1994-2005, Northern Digital Inc. All rights reserved.
Confidential and Proprietary.
Not for distribution outside of NDI without proper authorization.


*****************************************************************/

#ifdef CC_TRACE
#pragma message ("include "__FILE__)
#endif


/**************************************************************************
  Defines
**************************************************************************/

//----------------------------------------------------------------------------------------------
// Generic error codes returned from all OAPI functions
#define OPTO_NO_ERROR_CODE          	0
#define OPTO_SYSTEM_ERROR_CODE      	1000
#define OPTO_USER_ERROR_CODE        	2000
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// OAPI Extended Error Codes (OEE_*)   returned from OAPIGetExtendedErrorCode()
#define OEE_COMPONENTVERSIONMISMATCH	10001	// versioning error (i.e. firmare != OAPI)
#define OEE_CONNECTFAILURE				10002	// generic connect error (i.e. no tracker)
#define OEE_MESSAGEFAULT				10003	// generic messaging error (send/receive)
#define OEE_REQUESTFAILURE				10004	// a request to the system failed.  (i.e. Get...())
#define OEE_FUNCTIONNOTFOUND			10005	// the function executed was unknown.  Typically an inter-release firmware/software mismatch.
#define OEE_FUNCTIONNOTCOMPLETE			10006	// function was unable to complete
//----------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------
// table to provide string version of error codes, provided through OapiLookupErrorCode()
#ifdef __OAPI_LOGGING__
// this tag used to control inclusion of this structure only in logging.c.
// the intent is to keep the table close to the defines, to keep them in sync.
struct OPTO_ERROR_STRINGS
{
	int id;
	char* id_str;
} OptoErrorStrings[] =
{
	OPTO_NO_ERROR_CODE, 			"OPTO_NO_ERROR_CODE",
	OPTO_SYSTEM_ERROR_CODE, 		"OPTO_SYSTEM_ERROR_CODE",
	OEE_COMPONENTVERSIONMISMATCH,	"OEE_COMPONENTVERSIONMISMATCH",
    OEE_CONNECTFAILURE,  			"OEE_CONNECTFAILURE",
    OEE_MESSAGEFAULT,  				"OEE_MESSAGEFAULT",
	OEE_FUNCTIONNOTFOUND,			"OEE_FUNCTIONNOTFOUND",
	OEE_FUNCTIONNOTCOMPLETE,		"OEE_FUNCTIONNOTCOMPLETE",
    OPTO_USER_ERROR_CODE,       	"OPTO_USER_ERROR_CODE",
	-1
};
#endif
//----------------------------------------------------------------------------------------------


/*
 * Flags for controlling the setup of the message passing layer on the
 * PC side.
 */
#define OPTO_LOG_ERRORS_FLAG          0x0001
#define OPTO_SECONDARY_HOST_FLAG      0x0002
#define OPTO_ASCII_RESPONSE_FLAG      0x0004
#define OPTO_LOG_MESSAGES_FLAG        0x0008
#define OPTO_LOG_DEBUG_FLAG           0x0010
#define OPTO_LOG_CONSOLE_FLAG		  0x0020
#define OPTO_LOG_VALID_FLAGS		  0x0039

/*
 * Minimum and maximum values for various system settings
 */
#define FLOAT_COMP_EPS						( (float)1E-10 )
#define OPTO_MIN_STROBERPORT_MARKERS		0
#define OPTO_MAX_STROBERPORT_MARKERS		256
#define OPTO_MAX_STROBERPORT_MARKERS_CERTUS	512
#define OPTO_MIN_MARKERS					1
#define OPTO_MAX_MARKERS					256
#define OPTO_MAX_MARKERS_CERTUS				512
#define OPTO_MIN_MARKERFREQ					( 1.0f - FLOAT_COMP_EPS )
#define OPTO_MAX_MARKERFREQ					( 3500.0f + FLOAT_COMP_EPS )
#define OPTO_MAX_MARKERFREQ_CERTUS			( 4600.0f + FLOAT_COMP_EPS )
#define OPTO_MIN_FRAMEFREQ					OPTO_MIN_MARKERFREQ
#define OPTO_MAX_FRAMEFREQ					( OPTO_MAX_MARKERFREQ / ( OPTO_MIN_MARKERS + 1 ) )
#define OPTO_MAX_FRAMEFREQ_CERTUS			( OPTO_MAX_MARKERFREQ_CERTUS / ( OPTO_MIN_MARKERS + 1.3f ) + FLOAT_COMP_EPS )
#define OPTO_MIN_THRESHOLD					0
#define OPTO_MAX_THRESHOLD					255
#define OPTO_MIN_MINGAIN					0
#define OPTO_MAX_MINGAIN					255
#define OPTO_MIN_DUTYCYCLE					( 0.10f - FLOAT_COMP_EPS )
#define OPTO_MAX_DUTYCYCLE					( 0.85f + FLOAT_COMP_EPS )
#define OPTO_MIN_VOLTAGE					( 5.5f - FLOAT_COMP_EPS )
#define OPTO_MAX_VOLTAGE					( 12.0f + FLOAT_COMP_EPS ) //Default maximum voltage. The higher is possible if app confirms all devices support it.
#define OPTO_MIN_COLLTIME					0.1f
#define OPTO_MAX_COLLTIME					99999

/*
 * Constants for raw files which can be converted.
 */
#define OPTOTRAK_RAW    1
#define ANALOG_RAW      2

/*
 * Constants for modes in which files can be opened.
 */
#define OPEN_READ       1
#define OPEN_WRITE      2

/*
 * Maximum constants for the processes we will have to keep track
 * of in the optoappl software.  Make room for one address per node,
 * and an extra for our data proprietor.
 */
#define MAX_OPTOTRAKS         1
#define MAX_DATA_PROPRIETORS  1
#define MAX_ODAUS             4
#define MAX_PROCESS_ADDRESSES (MAX_NODES + 1)

/*
 * Constants for the node HW Revisions
 */
#define OPTO_NODE_HWREV_SCU_3020			0
#define OPTO_NODE_HWREV_SCU_CERTUS			0
#define OPTO_NODE_HWREV_PS_3020				3
#define OPTO_NODE_HWREV_PS_CERTUS			4

/*
 * Constants for keeping track of whick process the application wants to
 * communicate with.
 */
#define OPTOTRAK                            0
#define DATA_PROPRIETOR                     1
#define ODAU1                               2
#define ODAU2                               3
#define ODAU3                               4
#define ODAU4                               5
#define SENSOR_PROP1                        6
#define OPTOTRAK_DATA_BUFFER_OVERWRITE_FLAG 0x8000

/*
 * Pointer definitions for spooling to memory.
 */
typedef void *              SpoolPtrType;

/*
 * Flag assignments for controlling blocking in
 * real-time data retrieval routines and for on-host 3D and 6D
 * conversions
 */
#define OPTO_LIB_POLL_REAL_DATA							0x0001
#define OPTO_CONVERT_ON_HOST							0x0002
#define OPTO_RIGID_ON_HOST								0x0004
#define OPTO_USE_INTERNAL_NIF						    0x0008
#define OPTO_USE_PRIMITIVE_RIGID_BODY_ALGORITHM			0x0010
#define OPTO_RIGID_NO_UPDATE3DDATA_MISSINGIFOFFANGLE	0x0020
#define OPTO_RIGID_NO_UPDATE3DDATA_MISSINGIFMAXERROR	0x0040

/*
 * String length
 */
#define MAX_ERROR_STRING_LENGTH 2047






/**************************************************************************
 Routine Definitions
**************************************************************************/

CPLUSPLUS_START

// Function to retrieve OAPI version
NDI_DECL1 int	NDI_DECL2 OAPIGetVersionString( char* szVersionString, int nBufferSize );
NDI_DECL1 int	NDI_DECL2 OAPILoggingStart( unsigned int deprecated );
NDI_DECL1 int	NDI_DECL2 OAPILoggingStop( );
NDI_DECL1 void 	NDI_DECL2 OAPILog( char* message, unsigned level );
NDI_DECL1 int   NDI_DECL2 OAPIGetLastError( char* buf, int len );
NDI_DECL1 int 	NDI_DECL2 OAPIGetExtendedErrorCode();


// System initialization routines
NDI_DECL1 int	NDI_DECL2 TransputerLoadSystem( char *pszNifFile );
NDI_DECL1 int	NDI_DECL2 TransputerInitializeSystem( unsigned uFlags );
NDI_DECL1 int	NDI_DECL2 TransputerShutdownSystem( void );
NDI_DECL1 int	NDI_DECL2 TransputerDetermineSystemCfg( char *pszInputLogFile );

NDI_DECL1 int	NDI_DECL2 OptotrakLoadCameraParameters( char *pszCamFile );
NDI_DECL1 int	NDI_DECL2 OptotrakLoadAutoScale( char *pszAutoScaleFile );
NDI_DECL1 int	NDI_DECL2 OptotrakLockTemperatures( );
NDI_DECL1 int	NDI_DECL2 OptotrakSetCollectionFile( char *pszCollectFile );

NDI_DECL1 int	NDI_DECL2 OptotrakSetupCollectionFromFile( char *pszCollectFile );
NDI_DECL1 int	NDI_DECL2 OptotrakSetupCollection( int nMarkers, float fFrameFrequency, float fMarkerFrequency, int nThreshold, int nMinimumGain, int nStreamData,
													float fDutyCycle, float fVoltage, float fCollectionTime, float fPreTriggerTime, int nFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakActivateMarkers( void );
NDI_DECL1 int	NDI_DECL2 OptotrakDeActivateMarkers( void );
NDI_DECL1 int	NDI_DECL2 OptotrakGetStatus( int *pnNumSensors, int *pnNumOdaus, int *pnNumRigidBodies, int *pnMarkers, float *pfFrameFrequency, float *pfMarkerFrequency,
											int *pnThreshold, int *pnMinimumGain, int *pnStreamData, float *pfDutyCycle, float *pfVoltage, float *pfCollectionTime,
											float *pfPreTriggerTime, int *pnFlags );

NDI_DECL1 int	NDI_DECL2 OptotrakChangeCameraFOR( char *pszInputCamFile, int nNumMarkers, Position3d *pdtMeasuredPositions, Position3d *pdtAlignedPositions,
 				char *pszAlignedCamFile, Position3d *pdt3dErrors, float *pfRmsError );
NDI_DECL1 int	NDI_DECL2 OptotrakSetStroberPortTable( int nPort1, int nPort2, int nPort3, int nPort4 );
NDI_DECL1 int	NDI_DECL2 OptotrakSaveCollectionToFile( char *pszCollectFile );
NDI_DECL1 int	NDI_DECL2 OptotrakGetErrorString( char* szErrorString, int nBufferSize );
NDI_DECL1 int	NDI_DECL2 OptotrakSetProcessingFlags( unsigned uFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakGetProcessingFlags( unsigned* pFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertRawTo3D( unsigned *pElems, void *pSensorReadings, Position3d *p3Ds );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertTransforms( unsigned *pElems, struct OptotrakRigidStruct *pDataDest6D, Position3d *pDataSource3D );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertFullRawTo3D( unsigned *pElems, void *pDataSourceFullRaw, Position3d *pdtDataDest3d );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertFullRawTo6D( unsigned *pElems3d, unsigned *pElems6d, void *pSrcFullRaw, Position3d *pDst3d, struct OptotrakRigidStruct *pdtDataDest6d );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertCentroidsAndStatusTo3D( unsigned *pElems3d, float *pSourceCent, void *pSourceStatus, Position3d *pdtDataDest3d );
NDI_DECL1 int	NDI_DECL2 OptotrakConvertCentroidsAndStatusTo6D( unsigned *pElems3d, unsigned *pElems6d, float *pSrcCent, void *pSrcStatus, Position3d *p3d, struct OptotrakRigidStruct *p6d );
NDI_DECL1 int	NDI_DECL2 OptotrakSplitFullRaw( int nMarkers, int nSensors, void *pDataSourceFullRaw, void *pCentroids, void *pSensorsStatus );
NDI_DECL1 int	NDI_DECL2 OptotrakGetNodeInfo( int nNodeId, struct OptoNodeInfoStruct *pdtNodeInfo );
NDI_DECL1 int	NDI_DECL2 OptotrakGetSubNodeInfo( int nNodeId, int nSubNodeId, struct OptoNodeInfoStruct *pdtSubNodeInfo );
NDI_DECL1 int	NDI_DECL2 OptotrakSetCameraParameters( int nMarkerType, int nWaveLength, int nModelType );
NDI_DECL1 int	NDI_DECL2 OptotrakGetCameraParameterStatus( int *pCurMarkerType, int *pnCurWaveLength, int *pnCurModelType, char *szStatus, int nStatusLength );
NDI_DECL1 int	NDI_DECL2 OptotrakReadAppParms( char *pszParmFile, char *pszSection, AppParmInfo *pParmInfo );
NDI_DECL1 int	NDI_DECL2 OptotrakSaveAppParms( char *pszParmFile, char *pszSection, AppParmInfo *pParmInfo );
NDI_DECL1 int	NDI_DECL2 OptotrakDeleteAppParms( char *pszParmFile, char *pszSection );
NDI_DECL1 int	NDI_DECL2 OdauSaveCollectionToFile( char *pszCollectFile );
NDI_DECL1 int	NDI_DECL2 OdauSetupCollectionFromFile( char *pszCollectFile );
NDI_DECL1 int	NDI_DECL2 OdauSetTimer( int nOdauId, unsigned uTimer, unsigned uMode, unsigned long ulVal );
NDI_DECL1 int	NDI_DECL2 OdauLoadControlFiFo( int nOdauId, unsigned uEntries, struct OdauControlWordStruct *pCtlWords );
NDI_DECL1 int	NDI_DECL2 OdauSetAnalogOutputs( int nOdauId, float *pfVoltage1, float *pfVoltage2, unsigned uChangeMask );
NDI_DECL1 int	NDI_DECL2 OdauSetDigitalOutputs( int nOdauId, unsigned *puDigitalOut, unsigned uUpdateMask );
NDI_DECL1 int	NDI_DECL2 OdauSetupCollection( int nOdauId, int nChannels, int nGain, int nDigitalMode, float fFrameFreq, float fScanFreq, int nStreamData,
												float fCollectionTime, float fPreTriggerTime, unsigned uFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakStopCollection( void );
NDI_DECL1 int	NDI_DECL2 OdauGetStatus( int nOdauId, int *pnChannels, int *pnGain, int *pnDigitalMode, float *pfFrameFrequency, float *pfScanFrequency,
										int *pnStreamData, float *pfCollectionTime, float *pfPreTriggerTime, unsigned *puCollFlags, int *pnFlags );
NDI_DECL1 int	NDI_DECL2 RigidBodyAdd( int nRigidBodyId, int nStartMarker, int nNumMarkers, float *pRigidCoordinates, float *pNormalCoordinates, int nFlags );
NDI_DECL1 int	NDI_DECL2 RigidBodyAddFromFile( int nRigidBodyId, int nStartMarker, char *pszRigFile, int nFlags );
NDI_DECL1 int	NDI_DECL2 RigidBodyChangeSettings( int nRigidBodyId, int nMinMarkers, int nMaxMarkersAngle, float fMax3dError, float fMaxSensorError,
													float fMax3dRmsError, float fMaxSensorRmsError, int nFlags );
NDI_DECL1 int	NDI_DECL2 RigidBodyDelete( int nRigidBodyId );
NDI_DECL1 int	NDI_DECL2 RigidBodyChangeFOR( int nRigidId, int nRotationMethod );
NDI_DECL1 int	NDI_DECL2 DataGetLatest3D( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetLatestCentroid( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetLatestRaw( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetLatestTransforms( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetLatestOdauRaw( int nOdauId, unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 RequestLatest3D( void );
NDI_DECL1 int	NDI_DECL2 RequestLatestRaw( void );
NDI_DECL1 int	NDI_DECL2 RequestLatestCentroid( void );
NDI_DECL1 int	NDI_DECL2 RequestLatestTransforms( void );
NDI_DECL1 int	NDI_DECL2 RequestLatestOdauRaw( int nOdauId );

NDI_DECL1 int NDI_DECL2 DataIsReady( void );
NDI_DECL1 int NDI_DECL2 DataIsReadyExt( unsigned retryCount );
NDI_DECL1 int NDI_DECL2 ReceiveLatestData( uint *uFrameNumber, uint *uElements, uint *uFlags, void *pDataDest );
NDI_DECL1 int NDI_DECL2 DataReceiveLatest3D( uint *pnFrame, uint *pElems, uint *pFlags, Position3d *pDataDest );
NDI_DECL1 int NDI_DECL2 DataReceiveLatestRaw( uint *pnFrame, uint *pElems, uint *pFlags, void *pDataDest );

NDI_DECL1 int	NDI_DECL2 DataReceiveLatestCentroid( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataBuff );
NDI_DECL1 int	NDI_DECL2 DataReceiveLatestTransforms( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataReceiveLatestOdauRaw( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, int *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetLatestTransforms2( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, struct OptotrakRigidStruct *pDataDest6D, Position3d *pDataDest3D );
NDI_DECL1 int	NDI_DECL2 DataReceiveLatestTransforms2( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, struct OptotrakRigidStruct *pDataDest6D, Position3d *pDataDest3D );
NDI_DECL1 int	NDI_DECL2 RetrieveSwitchData( int nNumSwitches, boolean *pbSwitchData );

// Device Handle functions for Optotrak Certus
NDI_DECL1 int	NDI_DECL2 OptotrakGetNumberDeviceHandles( int *pnDeviceHandles );
NDI_DECL1 int	NDI_DECL2 OptotrakGetDeviceHandles( DeviceHandle *pdtDeviceHandles, int nDeviceHandles, unsigned *pFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleFree( int dhID );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleGetNumberProperties( int dhID, int *pnProperties );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleGetProperties( int dhID, DeviceHandleProperty *pProps, int nProperties );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleGetProperty( int dhID, DeviceHandleProperty *pProp, unsigned uProperty );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleGetSROMProperty( int dhID, DeviceHandleProperty *pProp, unsigned uProperty, void* pData, unsigned uDataSize );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleSetProperties( int dhID, DeviceHandleProperty* grdtProperties, int nProperties );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleEnable( int dhID );
NDI_DECL1 int	NDI_DECL2 RigidBodyAddFromDeviceHandle( int dhID, int nRigidBodyId, unsigned uFlags );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleSetVisibleLED( int dhID, int nLED, enum VLEDState dtState );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleSetBeeper( int dhID, int nDuration_ms );

NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleSROMRead( int dhID, unsigned long addr, void* pData, int datalen );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleSROMWrite( int dhID, unsigned long addr, void* pData, int datalen );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleUserSROMRead( int dhID, unsigned long addr, void* pData, int datalen );
NDI_DECL1 int	NDI_DECL2 OptotrakDeviceHandleUserSROMWrite( int dhID, unsigned long addr, void* pData, int datalen );

NDI_DECL1 int	NDI_DECL2 OptotrakEnableWireless( int nDevice, boolean bEnable );

NDI_DECL1 int	NDI_DECL2 RequestNext3D( void );
NDI_DECL1 int	NDI_DECL2 RequestNextCentroid( void );
NDI_DECL1 int	NDI_DECL2 RequestNextRaw( void );
NDI_DECL1 int	NDI_DECL2 RequestNextTransforms( void );
NDI_DECL1 int	NDI_DECL2 RequestNextOdauRaw( int nOdauId );
NDI_DECL1 int	NDI_DECL2 DataGetNext3D( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetNextCentroid( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetNextRaw( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetNextTransforms( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );
NDI_DECL1 int	NDI_DECL2 DataGetNextTransforms2( unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, struct OptotrakRigidStruct *pDataDest6D,
													Position3d *pDataDest3D );
NDI_DECL1 int	NDI_DECL2 DataGetNextOdauRaw( int nOdauId, unsigned *pnFrame, unsigned *pElems, unsigned *pFlags, void *pDataDest );

NDI_DECL1 int	NDI_DECL2 DataBufferInitializeFile( unsigned uDataId, const char *pszFileName );
NDI_DECL1 int	NDI_DECL2 DataBufferInitializeMem( unsigned uDataId, SpoolPtrType pMemory );
NDI_DECL1 int	NDI_DECL2 DataBufferStart( void );
NDI_DECL1 int	NDI_DECL2 DataBufferStop( void );
NDI_DECL1 int	NDI_DECL2 DataBufferSpoolData( unsigned *puSpoolStatus );
NDI_DECL1 int	NDI_DECL2 DataBufferWriteData( unsigned *puRealtimeData, unsigned *puSpoolComplete, unsigned *puSpoolStatus, unsigned long *pulFramesBuffered );

NDI_DECL1 int	NDI_DECL2 DataBufferAbortSpooling( void );

NDI_DECL1 int NDI_DECL2 FileConvert( char *pszInputFilename, char *pszOutputFilename, unsigned uFileType );

NDI_DECL1 int	NDI_DECL2 nOptotrakAlignSystem( AlignParms dtAlignParms, float* fRMSError );
NDI_DECL1 int	NDI_DECL2 nOptotrakRegisterSystem( RegisterParms dtRegisterParms, float* fRMSError );
NDI_DECL1 int	NDI_DECL2 nOptotrakCalibrigSystem( CalibrigParms dtCalibrigParms, float * fRMSError );

NDI_DECL1 void NDI_DECL2 OAPICombineXfrms( transformation *xfrm1_ptr, transformation *xfrm2_ptr, transformation *NewXfrmPtr );
NDI_DECL1 void NDI_DECL2 OAPICvtQuatToRotationMatrix( struct QuatRotationStruct *QuatPtr, RotationMatrixType r );
NDI_DECL1 void NDI_DECL2 OAPICvtRotationMatrixToQuat( struct QuatRotationStruct *QuatPtr, RotationMatrixType r );
NDI_DECL1 void NDI_DECL2 OAPIDetermineEuler( RotationMatrixType r, rotation *RotationAnglePtr );
NDI_DECL1 void NDI_DECL2 OAPIDetermineR( rotation *RotationAnglePtr, float ( *RotationMatrix )[ 3] );
NDI_DECL1 void NDI_DECL2 OAPIInverseXfrm( transformation *InputXfrmPtr, transformation *OutputXfrmPtr );
NDI_DECL1 void NDI_DECL2 OAPITransformPoint(	float ( *RotationMatrix )[ 3], Position3d *translation, Position3d *OriginalPositionPtr,
												Position3d *RotatedPositionPtr );
NDI_DECL1 int NDI_DECL2 OAPIFileOpen(	char* pszFilename, UINT uFileId, UINT uFileMode, int* pnItems, int* pnSubItems, long int *plnFrames,
										float* pfFrequency, char* pszComments, void** pFileHeader );
NDI_DECL1 int NDI_DECL2 OAPIFileRead( UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataDest );
NDI_DECL1 int NDI_DECL2 OAPIFileWrite( UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataSrc );
NDI_DECL1 int NDI_DECL2 OAPIFileClose( UINT uFileId );
NDI_DECL1 int NDI_DECL2 OAPIFileOpenAll(	char *pszFilename, UINT uFileId, UINT uFileMode, int *pnItems, int *pnSubItems, int *pnCharSubItems,
											int *pnIntSubItems, int *pnDoubleSubItems, long int *plnFrames, float *pfFrequency, char *pszComments,
											void **pFileHeader );
NDI_DECL1 int NDI_DECL2 OAPIFileReadAll(	UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataDestFloat, void *pDataDestChar,
											void *pDataDestInt, void *pDataDestDouble );
NDI_DECL1 int NDI_DECL2 OAPIFileWriteAll(	UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataSrcFloat, void *pDataSrcChar,
											void *pDataSrcInt, void *pDataSrcDouble );
NDI_DECL1 int NDI_DECL2 OAPIFileCloseAll( UINT uFileId );


CPLUSPLUS_END
// these calls included for external apps only.  Otherwise, the naming conflicts with NDLIB.
#ifndef INC_NDMATH_H
NDI_DECL1 void NDI_DECL2 CombineXfrms( transformation *xfrm1_ptr, transformation *xfrm2_ptr, transformation *NewXfrmPtr );
NDI_DECL1 void NDI_DECL2 CvtQuatToRotationMatrix( struct QuatRotationStruct *QuatPtr, RotationMatrixType r );
NDI_DECL1 void NDI_DECL2 CvtRotationMatrixToQuat( struct QuatRotationStruct *QuatPtr, RotationMatrixType r );
NDI_DECL1 void NDI_DECL2 DetermineEuler( RotationMatrixType r, rotation *RotationAnglePtr );
NDI_DECL1 void NDI_DECL2 DetermineR( rotation *RotationAnglePtr, float ( *RotationMatrix )[ 3] );
NDI_DECL1 void NDI_DECL2 InverseXfrm( transformation *InputXfrmPtr, transformation *OutputXfrmPtr );
NDI_DECL1 void NDI_DECL2 TransformPoint(	float ( *RotationMatrix )[ 3], Position3d *translation, Position3d *OriginalPositionPtr, 
												Position3d *RotatedPositionPtr );
#endif
#ifndef INC_NDFILES_H
NDI_DECL1 int NDI_DECL2 FileOpen(	char* pszFilename, UINT uFileId, UINT uFileMode, int* pnItems, int* pnSubItems, long int *plnFrames, 
										float* pfFrequency, char* pszComments, void** pFileHeader );
NDI_DECL1 int NDI_DECL2 FileRead( UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataDest );
NDI_DECL1 int NDI_DECL2 FileWrite( UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataSrc );
NDI_DECL1 int NDI_DECL2 FileClose( UINT uFileId );
NDI_DECL1 int NDI_DECL2 FileOpenAll(	char *pszFilename, UINT uFileId, UINT uFileMode, int *pnItems, int *pnSubItems, int *pnCharSubItems, 
											int *pnIntSubItems, int *pnDoubleSubItems, long int *plnFrames, float *pfFrequency, char *pszComments,
											void **pFileHeader );
NDI_DECL1 int NDI_DECL2 FileReadAll(	UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataDestFloat, void *pDataDestChar, 
											void *pDataDestInt, void *pDataDestDouble );
NDI_DECL1 int NDI_DECL2 FileWriteAll(	UINT uFileId, long int lnStartFrame, UINT uNumberOfFrames, void *pDataSrcFloat, void *pDataSrcChar, 
											void *pDataSrcInt, void *pDataSrcDouble );
NDI_DECL1 int NDI_DECL2 FileCloseAll( UINT uFileId );
#endif

#endif /* INC_NDOPTO_H */




