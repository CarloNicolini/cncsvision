// ************************************************************************************************
// * File : SMIDefs.h
// * Project : IntegMotorInterface
// * Version : 2.201
// * Last Update : 12/16/2009
// * Copyright © 2000-2009 Animatics Corp.
// * Author : 
// ************************************************************************************************
#ifndef __SMIDEFS_H_
#define __SMIDEFS_H_

#define MAX_STRLEN				256
#define MAX_HOST_AXIS			6
#define PI						((double)3.14159265358979323846)
#define EPSILON					0.0000000001

#define MAX_PRG_LENGTH			0x00007FFF
#define MAX_PRG_LENGTH9			0x0000FFFE
#define MAX_PRG_LENGTH6			0x0000FA96


// ************************************************************************************************
// *** CommError and Motor Detection Return Values:
// ************************************************************************************************
#define	CER_SUCCESS			0x00000001		// Motors Found (=CER_MOTORSFOUND)
#define	CER_MOTORSFOUND		0x00000001		// Motors Found (=CER_SUCCESS)
#define	CER_SOMEADDRESSED	0x00000002		// Some Motors are addressed
#define	CER_BIGADDRESS		0x00000004		// The address of some motors is bigger than the specified MaxAddress
#define	CER_DUPLICATEADDR	0x00000008		// Duplicate address
#define	CER_NOTADDRESSED	0x00000010		// No motor is addressed
#define	CER_COMMERROR		0x00000020		// Other communication problem
#define	CER_NOMOTOR			0x00000040		// Failed to find motors
#define	CER_NETERROR		0x00000080		// Network error
#define	CER_USBERROR		0x00000100		// USB error
#define	CER_CANOPENERROR	0x00000200		// CANOpen error
#define	CER_TRYHIGHBAUD		0x00000400		// no motors found at lower baud rates try higher rates

#define	CER_PORTNOTOPEN		0x00010000		// Port Not Open
#define	CER_CANCELED		0x00040000		// Canceled

// CommError and Motor Detection Actions
#define	CMA_THROWX			0x00100000		// Throw exception if an error occurs
#define	CMA_READDRESS232	0x00400000		// Re-address RS232 chain if they are not addressed
#define	CMA_USECURRENTBAUD	0x00800000		// only try at current baud rate
#define	CMA_NOENDPRG		0x04000000		// Do not Stop the user program before addressing/detecting motors
#define	CMA_NOESTABLISH		0x08000000		// Do try to establish the communication (it is already established)


//#define	CER_CHANGED			0x00010000		// The pointers have been changed.
//#define	CER_CONTINUE		0x00020000		// The caller can continue
//#define	CER_CANCELED		0x00040000		// Canceled


// ************************************************************************************************
// *** Planes
// ************************************************************************************************
#define	PLN_XY					0
#define PLN_XZ					1
#define PLN_YZ					2

// ************************************************************************************************
// *** LogFlags
// ************************************************************************************************
#define LOG_COMM				0x00000001
#define	LOG_CMOTION				0x00000002
#define LOG_PATH				0x00000004
#define LOG_SCAN				0x00000008
#define LOG_MOTOR				0x00000010
#define LOG_ALL					0x0000001F
#define LOG_APPEND				0x00010000
#define LOG_NEW					0x00020000		// Do not set this flag. It is used internally

// ************************************************************************************************
// *** Notification Codes:
// ************************************************************************************************
#define SMNOT_STARTDOWNLOAD		0x00000001
#define	SMNOT_PROGRESS			0x00000002
#define SMNOT_END				0x00000003
#define SMNOT_STARTUPLOAD		0x00000004
#define SMNOT_STARTADDRESS		0x00000005
#define SMNOT_STARTDETECT		0x00000006
#define SMNOT_STARTCOLISION		0x00000007
#define SMNOT_STARTGETINFO		0x00000008

// ************************************************************************************************
// *** Motor Download Flags:
// ************************************************************************************************
#define	MDF_TURNOFF				0x00000001		// Turn it off if it is on
#define	MDF_BLIND				0x00000010		// Blind Download
#define	MDF_CAL					0x00000080		// Download Calibration Program
#define	MDF_FORCEDOWNLOAD		0x00000100		// Download even if motor has no EEPROM

#define MDF_SKIPPRGINFO			0x00000200

// ************************************************************************************************
// *** Error Codes for IntegMotorInterface exceptions match resourec numbers for SMI and IntegMotorInterface
// ************************************************************************************************
#define EID_NOERROR					4000
#define EID_CX						4001
#define EID_OPEN					4002
#define EID_SETTIMEOUT				4003
#define EID_CONNECTION				4004
#define EID_TIMEOUT					4005
#define EID_ECHOTIMEOUT				4006
#define EID_BADECHO					4007
#define EID_PORTNOTOPEN				4008
#define EID_OVERRUN					4009
#define EID_RXOVER					4010
#define EID_RXPARITY				4011
#define EID_FRAME					4012
#define EID_BREAK					4013
#define EID_IOE						4014
#define EID_TXFULL					4015
#define EID_COMMUNKNOWN				4016
#define EID_READ					4017
#define EID_WRITE					4018
#define EID_INVALIDDATA				4019
#define EID_INVALIDTIME				4020
#define EID_INVALIDTIMEDELTA		4021
#define EID_INVALIDPOSDELTA			4022
#define EID_INTDATASPACE			4023
#define EID_BUFFOVERFLOW			4024
#define EID_BUFFUNDERFLOW			4025
#define EID_HMFAILED				4026
#define EID_GETSTATUS				4027
#define EID_SAMPLERATES				4028
#define EID_NOTSUPPORTED			4029
#define EID_NOTINIT					4030
#define EID_NOMEM					4031
#define EID_GETALL					4032
#define EID_NOMORE					4033
#define EID_OPENFILE				4034
#define EID_OPENFILEWRITE			4035
#define EID_NOEEPROM				4036
#define EID_INVALIDMODE				4037
#define EID_RS485					4038
#define EID_FILETOOLONG				4039
#define EID_CORRUPTEEPROM			4040
#define EID_DOWNLOADON				4041
#define EID_VERSIONMISMATCH			4042
#define EID_SOCKET					4043
#define EID_DOWNLOADRCKS			4044
#define EID_NETERROR				4045
#define EID_USBERROR				4046
#define EID_BIGADDRESS				4047
#define EID_CALWARNING				4048
#define EID_CANCELED				4049
#define EID_SOMEADDRESSED			4050
#define EID_COMMUNICATION			4051
#define EID_DUPLICATEADDR			4052
#define EID_COMMRESET				4053
#define EID_STOP					4054
#define EID_NOTFOUND				4055
#define EID_LANGFILE				4056
#define EID_INVALID_PROJ			4057
#define EID_READFILE				4058
#define EID_CHANGEBAUD				4059
#define EID_CANERROR				4060
#define EID_NOCANDWLD				4061
#define EID_NOLEGOACK				4062
#define EID_NOMODBUSDWLD			4063
#define EID_INCORRECTMICRO			4064
#define EID_FILEFORMAT				4065
#define EID_DLD_BUFFEREXCEED		4066
#define EID_DLD_HEXCHKSUM			4067
#define EID_DLD_HEXADDRESS			4068
#define EID_HEXTYPE					4069
#define EID_DATAINSERT				4070
#define EID_CMDNOSUPPORT			4071
#define EID_NOTIMEDELTA				4072
#define EID_CANINVTIMEDELTA			4073
#define EID_CANVOLTAGEFAULT			4074
#define EID_CANEXCESSTEMP  			4075
#define EID_CANEXCESSPOS   			4076
#define EID_CANVELOCITYLIMIT		4077
#define EID_CANDEDTLIMIT   			4078
#define EID_CANHWPOSHWLIMIT  		4079
#define EID_CANHWNEGHWLIMIT 		4080
#define EID_CANSWPOSLIMIT  			4081
#define EID_CANSWNEGLIMIT  			4082
#define EID_FIRMVERMISSMATCH		4083
#define EID_BADMODEL				4084

// scanner errors match the resource numbers in SMI and IntegMotorInterface
#define ER_CANNOT_OPEN				8672
#define ER_ERROR_CLINE				8673
#define ER_LINE_TOKENS				8674
#define ER_NOLANGFILE				8675		
#define ER_UNKNOWNERROR				8676

#define ER_2DEFAULTS				8700
#define ER_2ELSE					8701
#define ER_2MANYINCLUDES			8702
#define ER_ABSEXPECTED				8703
#define ER_ASSIGNECXPTD				8704
#define ER_BADBREAK					8705
#define ER_CASEAFTERDEFAULT			8706
#define ER_CASEWOSWITCH				8707
#define ER_CASEXPTD					8708
#define ER_CBRACKETXPTD				8709
#define ER_CLOSEPXPTD				8710
#define ER_CMPWOIFX					8711
#define ER_COMMAXPTD				8712
#define ER_DEFAULTWOSWITCH			8713
#define ER_DESTINATION				8714
#define ER_DIRECTIVECLOSE			8715
#define ER_DOTEXPTD					8716
#define ER_DUPLICATELABELS			8717
#define ER_ELSEIFAFTERELSE			8718
#define ER_ELSEIFWOIF				8719
#define ER_ELSEWOIF					8720
#define ER_ENDIFWOIF				8721
#define ER_ENDSWOSWITCH				8722
#define ER_ENDXPTD					8723
#define ER_IFFWOCMP					8724
#define ER_IFTWOCMP					8725
#define ER_IFWOENDIF				8726
#define ER_IFXWOCMP					8727
#define ER_INVALIDEOP				8728
#define ER_INVALIDEXPTERM			8729
#define ER_INVALIDNOPARAM			8730
#define ER_INVALIDPARAM				8731
#define ER_LANGFILE					8732
#define ER_LOOPWOWHILE				8733
#define ER_LVALUE					8734
#define ER_NEWLINESTRING			8735
#define ER_NOTINPROGRAM				8736
#define ER_NUMSIGNXPTD				8737
#define ER_NUMXPTD					8738
#define ER_OBRACKETXPTD				8739
#define ER_OPENINCLUDE				8740
#define ER_OPENPXPTD				8741
#define ER_QUOTXPTD					8742
#define ER_REDEF					8743
#define ER_RESERVED					8744
#define ER_SPACEXPTD				8745
#define ER_STRXPTD					8746
#define ER_SWITCHWOENDS				8747
#define ER_SYNTAX					8748
#define ER_UNDEFINED				8749
#define ER_UPARAMXPTD				8750
#define ER_WHILEWOLOOP				8751
#define ER_DIVIDEBYZERO				8752
#define ER_FLT_NOTALLOW				8753
#define ER_NOFLOATVARMATH			8754
#define ER_PEERTOPEERTYPE			8755
#define	ER_MISSINGSEMI				8756
#define	ER_MISSINGCOLON				8757
#define	ER_INVALIDADDRESS			8758
#define	ER_INVALIDAXIS				8759
#define	ER_REPORTNOTALLOWED			8760

#define WR_BAUDRATE					8800
#define WR_DATARANGE				8801
#define WR_DEFAULTLANGUAGE			8802
#define WR_MISSINGPOUND				8803
#define WR_NUMRANGE					8804
#define WR_NUMRANGEMM				8805
#define WR_NUMRANGEN				8806
#define WR_NUMRANGENN				8807
#define WR_NUMRANGENP				8808
#define WR_NUMRANGEP				8809
#define WR_FLTDECIMALS				8810
#define WR_FLTRANGE					8811
#define WR_FLTOVERFLOW				8812
#define WR_LONGOVERFLOW				8813
#define WR_FLTDECIMALSPT			8814

#define FATAL_ER_2MANYERRORS		8900
#define FATAL_ER_2MANYWARNINGS      8901
#define FATAL_ER_NESTING            8902

// warning range
#define IS_WARNING_ID(x)	(((x) >= WR_BAUDRATE) && ((x) <= (WR_BAUDRATE + 99)))

// fatal error range (stop scanning)
#define IS_FATAL_ID(x)		(((x) >= FATAL_ER_2MANYERRORS) && ((x) <= (FATAL_ER_2MANYERRORS + 99)))

// ************************************************************************************************
// used when warnings replaces errors for keywords (no strings associated) only used for ER_UNDEFINED
#define ERRTOWARN_START		(FATAL_ER_2MANYERRORS + 100)							// set start area of converted errors 9000
#define ERRTOWARN_END		(ERRTOWARN_START + 99) 									// set start area of converted errors 9099
#define IS_ERRTOWWARNING(x)	(((x) >= ERRTOWARN_START) && ((x) <= ERRTOWARN_END))	
#define MAKEWE(x)			((Errors2Warnings)?(ERRTOWARN_START + (x) % 100):(x))	// convert error to warning if Errors2Warnings
#define WE2E(x)				(IS_ERRTOWWARNING(x)?(ER_2DEFAULTS + (x % 100)):(x))	// convert error to warning back to orignal error


#endif //__SMIDEFS_H_

