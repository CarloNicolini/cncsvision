/***************************************************************
Name:			 NDPACK.H

Description:
	Macros and prototypes for  Packing/unpacking of	data
	structures between different host platforms

****************************************************************/

#ifndef	NDPACK_H_INCLUDED
#define	NDPACK_H_INCLUDED


#ifdef CC_TRACE
#pragma	message	("include "__FILE__)
#endif

/*
 * There is	an appropriate version of ndhost.h for each	platform!
 */
#include <ndtypes.h>


#ifndef	HOSTDEFINED
#error You must	define the type	of system you are using...
#endif

/****************************************************************
  Defines
****************************************************************/


#define	PC_CHAR_SIZE		1
#define	PC_SHORT_SIZE		2
#define	PC_INT_SIZE			2
#define	PC_LONG_SIZE		4
#define	PC_FLOAT_SIZE		4
#define	PC_DOUBLE_SIZE		8


/****************************************************************
  Macros
****************************************************************/

/*** char's	****************************************************/
#define	PackUChar( src,	dst, s )	((*(dst))=(*(src)),	1)
#define	UnPackUChar( dst, src, s )	((*(dst))=(*(src)),	1)

#define	UnPackChar(	dst, src, s	)	UnPackUChar( (uchar	*)(dst), (src),	(s)	)
#define	PackChar( src, dst,	s )		PackUChar( (uchar *)(src), (dst), (s) )

#define	bChar_Pack(	  pch, ppuch, nCharsToPack )   \
	(memcpy(*(ppuch), (pch),	(nCharsToPack)), \
	*(ppuch) +=	(nCharsToPack),	\
	1)

#define	bChar_UnPack( pch, ppuch, nCharsToUnPack ) \
	(memcpy((pch),	  *(ppuch),	(nCharsToUnPack)), \
	*(ppuch) +=	(nCharsToUnPack), \
	1)

#define	Char_PackSize(p,num)		(PC_CHAR_SIZE *	(num))

int	  String_Pack(	   const char* pSrc,		uchar **ppuch, uint	maxSize);
int	  String_UnPack(		 char* pDest, const	uchar **ppuch, uint	maxSize);
ulong String_PackSize( const char* pData, uint maxSize);

/*** Integers **************************************************/
#define	PackUnsigned( src, dst,	s )						  \
	(((uchar *)(dst))[1] = (*(src) >> 8) & 0xff,  \
	 ((uchar *)(dst))[0] = *(src) &	0xff,		  \
	2 )

#define	UnPackUnsigned(	dst, src, s	)				 \
	( \
	  *(dst) =							 \
		(unsigned)((uchar *)(src))[1] << 8 | \
		*((uchar *)(src)),					\
	2 )

#define	PackInt( src, dst, s )	   \
	PackUnsigned( (unsigned	*)(src), (dst),	(s)	)

#ifdef SHARC
#define	UnPackInt( dst,	src, s )			  \
	( \
	  *(dst) =						  \
		((((unsigned char *)(src))[1] &	0x80) ?	0xFFFF0000 : 0)	| \
		((unsigned)((unsigned char *)(src))[1])	<< 8 | \
		(unsigned)(*((uchar	*)(src))),	   \
	2 )
#else
#define	UnPackInt( dst,	src, s )			  \
	( \
	  *(dst) =						  \
		((int)((signed char	*)(src))[1]) <<	8 |	\
		(int)(*((uchar *)(src))),	  \
	2 )
#endif

#define	PackULong( src,	dst, s )									\
	(((uchar *)(dst))[3] = (uchar)((*(src) >> 24) &	0xff),	\
	 ((uchar *)(dst))[2] = (uchar)((*(src) >> 16) &	0xff),	\
	 ((uchar *)(dst))[1] = (uchar)((*(src) >> 8) & 0xff),	\
	 ((uchar *)(dst))[0] = (uchar)(*(src) &	0xff),			\
	4 )

#define	UnPackULong( dst, src, s )							\
	( \
	  *((ulong *)(dst))	=							 \
		(ulong)((uchar *)(src))[3] << 24 |	\
		(ulong)((uchar *)(src))[2] << 16 |	\
		(ulong)((uchar *)(src))[1] << 8	 |	\
		(ulong)*((uchar	*)(src)),			\
	4 )

#define	PackLong( src, dst,	s )		PackULong( (src), (dst), (s) )

#define	UnPackLong(	dst, src, s	)							\
	( \
	  *((long *)(dst)) =									 \
		(long)(((signed	char *)(src))[3]) << 24	|				\
		(ulong)(((uchar	*)(src))[2]) <<	16 |  \
		(ulong)(((uchar	*)(src))[1]) <<	8 |	  \
		(ulong)(*((uchar *)(src))),		   \
	4 )

#define	PackUInt32(	src, dst, s	)	PackULong( (src), (dst), (s) )

#define	UnPackUInt32( dst, src,	s )					\
	(*((uint32_t *)(dst)) =							  \
		(uint32_t)(((uchar *)(src))[3])	<< 24 |	\
		(uint32_t)(((uchar *)(src))[2])	<< 16 |	\
		(uint32_t)(((uchar *)(src))[1])	<< 8  |	\
		(uint32_t)(*((uchar	*)(src))),		 \
	4 )


int	UInt32_Pack(  const	uint32_t* pfSrc,		uchar **ppuch, uint	NumElements);
int	UInt32_UnPack(		uint32_t* pfDest, const	uchar **ppuch, uint	NumElements);
#define	UInt32_PackSize(p,num)		 (PC_LONG_SIZE * (num))

#define	PackShort( src,	dst, s )	(NDASSERT(PC_SHORT_SIZE==PC_INT_SIZE),	\
									 PackInt( (src), (dst),	(s)	))

#define	UnPackShort( dst, src, s )				\
	( \
	  *((unsigned short	*)(dst)) =						  \
		((unsigned short)((signed char *)(src))[1])	<< 8 | \
		(unsigned short)(*((uchar *)(src))),	 \
	2 )

int	Int_Pack(	const int *pSrc,		uchar **ppBuffer, uint numElements);
int	Int_UnPack(		  int *pDest, const	uchar **ppBuffer, uint numElements);
#define	Int_PackSize(p,num)			(PC_INT_SIZE * (num))

int	UInt8_Pack(	  const	uint8_t	*pSrc,		  uchar	**ppBuffer,	uint numElements);
int	UInt8_UnPack(		uint8_t	*pDest,	const uchar	**ppBuffer,	uint numElements);
#define	UInt8_PackSize(p,num)		  (PC_CHAR_SIZE	* (num))

int	UInt16_Pack(   const uint16_t *pSrc,		uchar **ppBuffer, uint numElements);
int	UInt16_UnPack(		 uint16_t *pDest, const	uchar **ppBuffer, uint numElements);
#define	UInt16_PackSize(p,num)		   (PC_INT_SIZE	* (num))

#define	Short_PackSize(p,num)		(PC_SHORT_SIZE * (num))
#define	Long_PackSize(p,num)		(PC_LONG_SIZE *	(num))

/*** Floats	****************************************************/
#ifdef NDI_BIG_ENDIAN

#define	PackFloat( src,	dst, s ) \
	(((uchar *)(dst))[0] = ((uchar *)(src))[3],	\
	 ((uchar *)(dst))[1] = ((uchar *)(src))[2],	\
	 ((uchar *)(dst))[2] = ((uchar *)(src))[1],	\
	 ((uchar *)(dst))[3] = ((uchar *)(src))[0],	\
	4 )

#define	UnPackFloat( dst, src, s )	\
	(((uchar *)(dst))[0] = (src)[3], \
	 ((uchar *)(dst))[1] = (src)[2], \
	 ((uchar *)(dst))[2] = (src)[1], \
	 ((uchar *)(dst))[3] = (src)[0], \
	4 )

#else

#ifdef SHARC21161

#define	PackFloat( src,	dst, s )	PackULong( (ulong *)(src), (dst), (s) )

#define	UnPackFloat( dst, src, s )	UnPackULong( (ulong	*)(dst), (src),	(s)	)

#else  /* SHARC	*/

#define	PackFloat( src,	dst, s )	\
	(((uchar *)(dst))[0] = ((uchar *)(src))[0],	\
	 ((uchar *)(dst))[1] = ((uchar *)(src))[1],	\
	 ((uchar *)(dst))[2] = ((uchar *)(src))[2],	\
	 ((uchar *)(dst))[3] = ((uchar *)(src))[3],	\
	4 )

#define	UnPackFloat( dst, src, s )	\
	(((uchar *)(dst))[0] = ((uchar *)(src))[0],	\
	 ((uchar *)(dst))[1] = ((uchar *)(src))[1],	\
	 ((uchar *)(dst))[2] = ((uchar *)(src))[2],	\
	 ((uchar *)(dst))[3] = ((uchar *)(src))[3],	\
	4 )

#endif
#endif /* NDI_BIG_ENDIAN */

boolean	bFloat_Pack(  const	float* pfSrc,  uchar **ppuch, uint NumElements);
boolean	bFloat_UnPack(		float* pfDest, const uchar **ppuch,	uint NumElements);
#define	Float_PackSize(p,num)		(PC_FLOAT_SIZE * (num))

/****************************************************************
 Global	Structures
****************************************************************/

/****************************************************************
 External Variables
****************************************************************/

/****************************************************************
 Routine Definitions
****************************************************************/

/*** Doubles ***************************************************/
CPLUSPLUS_START

uint PackDouble(   const double	*pDbl,		 uchar *pchBuff, uint uBufSize );
uint UnPackDouble(		 double	*pDbl, const uchar *pchBuff, uint uBufSize );

/* Packs 1 or more doubles into	the	given buffer. */
boolean	bDouble_Pack( const	double*	pdSrc, uchar **ppuch, uint NumElements);
/* Unpacks 1 or	more doubles from the given	buffer into	the	given destination. */
boolean	bDouble_UnPack(		  double* pdDest, const	uchar **ppuch, uint	NumElements);
CPLUSPLUS_END

/* Returns the number of bytes need	to pack	the	given number of	doubles	*/
#define	Double_PackSize(p,num)		(PC_DOUBLE_SIZE	* (num))

/*** RealType **************************************************/
#ifdef	REAL64

#define	PackRealType( p, q,	s )		PackDouble(	(p), (q), (s) )
#define	UnPackRealType(	p, q, s	)	UnPackDouble( (p), (q),	(s)	)

#else

#define	PackRealType( p, q,	s )		PackFloat( (p),	(q), (s) )
#define	UnPackRealType(	p, q, s	)	UnPackFloat( (p), (q), (s) )

#endif	/* REAL64 */

CPLUSPLUS_START
boolean	bRealType_Pack		 (const	float*	prtSrc,	 uchar **ppuch,	uint NumElements);
boolean	bRealType_Pack_d	 (const	double*	prtSrc,	 uchar **ppuch,	uint NumElements);
boolean	bRealType_UnPack	 (		float*	prtDest, const uchar** ppuch, uint NumElements);
boolean	bRealType_UnPack_d	 (		double*	prtDest, const uchar** ppuch, uint NumElements);
ulong	ulRealType_PackSize	 (const	float*	prt, uint NumElements);
ulong	ulRealType_PackSize_d(const	double*	prt, uint NumElements);
CPLUSPLUS_END
/*** ND	Types ***********************************************/
CPLUSPLUS_START
ulong ulPackRotationMatrix(	  const	RotationMatrixType *p,		   uchar *pchBuff, uint	uBuffSize );
ulong ulPackRotationMatrix_d( const	RotationMatrixType_d *p,	   uchar *pchBuff, uint	uBuffSize );
ulong ulUnPackRotationMatrix(		RotationMatrixType *p,	 const uchar *pchBuff, uint	uBuffSize );
ulong ulUnPackRotationMatrix_d(		RotationMatrixType_d *p, const uchar *pchBuff, uint	uBuffSize );

ulong ulPackComplex( const Complex *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackComplex(	   Complex *p, const uchar *pchBuff, uint uBuffSize	);

ulong ulPackRotation(		 const rotation	*p,			uchar *pchBuff,	uint uBuffSize );
ulong ulPackRotation_d(		 const rotation_d *p,		uchar *pchBuff,	uint uBuffSize );
ulong ulUnPackRotation(			   rotation	*p,	  const	uchar *pchBuff,	uint uBuffSize );
ulong ulUnPackRotation_d(		   rotation_d *p, const	uchar *pchBuff,	uint uBuffSize );
ulong ulRotation_PackSize(	 const rotation	*p );
ulong ulRotation_PackSize_d( const rotation_d *p );

ulong ulPackTransformation(		   const transformation	*p,			uchar *pchBuff,	uint uBuffSize );
ulong ulPackTransformation_d(	   const transformation_d *p,		uchar *pchBuff,	uint uBuffSize );
ulong ulUnPackTransformation(			 transformation	*p,	  const	uchar *pchBuff,	uint uBuffSize );
ulong ulUnPackTransformation_d(			 transformation_d *p, const	uchar *pchBuff,	uint uBuffSize );
ulong ulTransformation_PackSize(   const transformation	*p );
ulong ulTransformation_PackSize_d( const transformation_d *p );

ulong ulPackRotationTransformation(		const RotationTransformation *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulPackRotationTransformation_d(	const RotationTransformation_d *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackRotationTransformation(		  RotationTransformation *p,   const uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackRotationTransformation_d(		  RotationTransformation_d *p, const uchar *pchBuff, uint uBuffSize	);

ulong ulPackQuatRotation(	  const	QuatRotation *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulPackQuatRotation_d(	  const	QuatRotation_d *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackQuatRotation(			QuatRotation *p,   const uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackQuatRotation_d(		QuatRotation_d *p, const uchar *pchBuff, uint uBuffSize	);

ulong ulPackQuatTransformation(		const QuatTransformation *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulPackQuatTransformation_d(	const QuatTransformation_d *p,		 uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackQuatTransformation(		  QuatTransformation *p,   const uchar *pchBuff, uint uBuffSize	);
ulong ulUnPackQuatTransformation_d(		  QuatTransformation_d *p, const uchar *pchBuff, uint uBuffSize	);

CPLUSPLUS_END


#endif


