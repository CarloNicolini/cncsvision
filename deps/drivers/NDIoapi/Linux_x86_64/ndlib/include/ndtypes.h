/*************************************************************************
*	NDTYPES.H															 *
*************************************************************************/
#ifndef	INC_NDTYPES_H
#define	INC_NDTYPES_H

#include <ndhost.h>

#ifdef CC_TRACE
#pragma	message	("include "__FILE__)
#endif

/* do not include anything ABOVE this line.	*/

typedef	unsigned int	   UINT;
typedef	unsigned char	   UCHAR;
#undef ULONG
typedef	unsigned long int  ULONG;
/* generic abbreviations.*/
typedef	unsigned char	   boolean;
typedef	unsigned int	   uint;
typedef	unsigned char	   uchar;
typedef	unsigned long int  ulong;
typedef	unsigned short int ushort;
typedef	unsigned char	   ndbyte;

#ifdef PLATFORM_LINUX
typedef	long long			__int64;
typedef	unsigned long long	__uint64;
#endif

// PC/MSC specific sized types.	 Need to vet these on other	platforms.
// long	types are used in app_parm.
#ifndef	_STDINT_H
#define	_STDINT_H
typedef	unsigned char	   uint8_t;
typedef	unsigned short int uint16_t;
#endif

#ifndef	__uint32_t_defined
#define	__uint32_t_defined
typedef	unsigned int	   uint32_t;
typedef	signed int			int32_t;
#endif



#if	!defined( AMIGA	) &	!defined( __WINDOWS_H )	& !defined(_INC_WINDOWS)
typedef	char	BYTE;
#endif

// ------------------------------------------------------
// flags for boolean (t/f) and char	(t/f/none) variables.
#ifndef	FALSE
#define	FALSE	0
#define	TRUE	1
#endif
#define	NOSTATE	 2
// ------------------------------------------------------

#define	ERR		(-1)

typedef	enum _ndTypes
{
	TYPE_STRUCT	= 0,
	TYPE_INT16,
	TYPE_CHAR,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_INT32,
	TYPE_UINT8,
	TYPE_DATE
} NDtypes;

#ifdef REAL64
#define	TYPE_REALTYPE TYPE_DOUBLE
#else
#define	TYPE_REALTYPE TYPE_FLOAT
#endif

#define	BAD_FLOAT	 (float)-3.697314E28
#define	MAX_NEGATIVE (float)-3.0E28

#define	MIN_U8		0
#define	MAX_U8		255
#define	MIN_S8		-128
#define	MAX_S8		128

#define	MIN_U16		0
#define	MAX_U16		65536u
#define	MIN_S16		(-32768)
#define	MAX_S16		32767

#define	MIN_U32		0
#define	MAX_U32		(uint32_t)(4294967295u)
#define	MIN_S32		(int32_t)(-2147483647-1)
#define	MAX_S32		(int32_t)(2147483647)

#define	MIN_FLT		(-1.0e37)
#define	MAX_FLT		(3.402823466e+38F)
#if	defined	(PLATFORM_X86WIN32)	|| defined (PLATFORM_X86CONSOLE) ||	defined	(PLATFORM_X86)
#define	MIN_DBL		(-1.0e300)
#define	MAX_DBL		(1.7976931348623158e+308)
#else
#define	MIN_DBL		MIN_FLT
#define	MAX_DBL		MAX_FLT
#endif


#define	isValidFloat( x	)	 ( (x) > MAX_NEGATIVE )
#define	isBadFloat(	x )		 ( (x) <= MAX_NEGATIVE )
#define	isMissing( x )		 ( (x) < MAX_NEGATIVE )



typedef	struct ComplexStruct
{
	double	re;
	double	im;
} Complex;

typedef	struct RotationStruct
{
	float	roll;			/* rotation	about the object's z-axis */
	float	pitch;			/* rotation	about the object's y-axis */
	float	yaw;			/* rotation	about the object's x-axis */
} rotation;

typedef	struct _position3d
{
	float	x;
	float	y;
	float	z;
} Position3d;
#define	Position3dStruct	_position3d

typedef	struct _position3d_d
{
	double	 x;
	double	 y;
	double	 z;
} Position3d_d;
#define	Position3dStruct_d	_position3d_d

typedef	struct TransformationStruct
{
	struct RotationStruct rotation;
	Position3d translation;
} transformation;

typedef	float
	RotationMatrixType[	3 ][ 3 ];

typedef	struct RotationTransformationStruct
{
	RotationMatrixType		matrix;
	Position3d				translation;
} RotationTransformation;

typedef	struct QuatRotationStruct
{
	float	q0;
	float	qx;
	float	qy;
	float	qz;
} QuatRotation;

typedef	struct QuatTransformationStruct
{
	QuatRotation	 rotation;
	Position3d		 translation;
} QuatTransformation;

typedef	struct RotationTransformationErrorStruct
{
	RotationMatrixType		matrix;
	Position3d				translation;
	float					   error;
} RotationErrorTransformation;

typedef	struct QuatTransformationErrorStruct
{
	QuatRotation	 rotation;
	Position3d		 translation;
	float			 error;
} QuatErrorTransformation;

typedef	struct TransformationErrorStruct
{
	 struct RotationStruct rotation;
	 Position3d	translation;
	 float		  error;
} ErrorTransformation;

typedef	struct PhiThetaRotationStruct
{
	float phi;			 /*	azimuthal rotation about the z-axis	*/
	float theta;		 /*	angle from the z-axis */
	float gamma;		 /*	optional twist angle about the vector specified	by phi
							  and theta	(for 6D) */
} PhiThetaRotation;

typedef	struct PhiThetaTransformationStruct
{
	PhiThetaRotation rotation;
	Position3d		 translation;
} PhiThetaTransformation;

typedef	struct RotationStruct_d
{
	double	 roll;			 /*	rotation about the object's	z-axis */
	double	 pitch;			 /*	rotation about the object's	y-axis */
	double	 yaw;			 /*	rotation about the object's	x-axis */
} rotation_d;

typedef	struct TransformationStruct_d
{
	rotation_d	 rotation;
	Position3d_d translation;
} transformation_d;

typedef	double
	RotationMatrixType_d[ 3	][ 3 ];

typedef	struct RotationTransformationStruct_d
{
	RotationMatrixType_d	  matrix;
	Position3d_d			  translation;
} RotationTransformation_d;

/*
 * Structure defined in	quatern.h.
 */
typedef	struct QuatRotationStruct_d
{
	double	 q0;
	double	 qx;
	double	 qy;
	double	 qz;
} QuatRotation_d;

typedef	struct QuatTransformationStruct_d
{
	QuatRotation_d	   rotation;
	Position3d_d	   translation;
} QuatTransformation_d;

typedef	struct PhiThetaRotationStruct_d
{
	double phi;
	double theta;
	double gamma;

} PhiThetaRotation_d;

typedef	struct PhiThetaTransformationStruct_d
{
	PhiThetaRotation_d rotation;
	Position3d_d	   translation;
} PhiThetaTransformation_d;

/**********************************************************************/
#endif /* INC_NDTYPES_H	*/


