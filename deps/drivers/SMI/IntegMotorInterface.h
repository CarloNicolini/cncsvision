

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Oct 03 14:05:49 2012
 */
/* Compiler settings for .\IntegMotorInterface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IntegMotorInterface_h__
#define __IntegMotorInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISMIMotor_FWD_DEFINED__
#define __ISMIMotor_FWD_DEFINED__
typedef interface ISMIMotor ISMIMotor;
#endif 	/* __ISMIMotor_FWD_DEFINED__ */


#ifndef __ISMIComm_FWD_DEFINED__
#define __ISMIComm_FWD_DEFINED__
typedef interface ISMIComm ISMIComm;
#endif 	/* __ISMIComm_FWD_DEFINED__ */


#ifndef __ISMICMotion_FWD_DEFINED__
#define __ISMICMotion_FWD_DEFINED__
typedef interface ISMICMotion ISMICMotion;
#endif 	/* __ISMICMotion_FWD_DEFINED__ */


#ifndef __ISMIPath_FWD_DEFINED__
#define __ISMIPath_FWD_DEFINED__
typedef interface ISMIPath ISMIPath;
#endif 	/* __ISMIPath_FWD_DEFINED__ */


#ifndef __ISMIScan_FWD_DEFINED__
#define __ISMIScan_FWD_DEFINED__
typedef interface ISMIScan ISMIScan;
#endif 	/* __ISMIScan_FWD_DEFINED__ */


#ifndef __ISMIErrors_FWD_DEFINED__
#define __ISMIErrors_FWD_DEFINED__
typedef interface ISMIErrors ISMIErrors;
#endif 	/* __ISMIErrors_FWD_DEFINED__ */


#ifndef __SMIHost_FWD_DEFINED__
#define __SMIHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class SMIHost SMIHost;
#else
typedef struct SMIHost SMIHost;
#endif /* __cplusplus */

#endif 	/* __SMIHost_FWD_DEFINED__ */


#ifndef __SMIMotor_FWD_DEFINED__
#define __SMIMotor_FWD_DEFINED__

#ifdef __cplusplus
typedef class SMIMotor SMIMotor;
#else
typedef struct SMIMotor SMIMotor;
#endif /* __cplusplus */

#endif 	/* __SMIMotor_FWD_DEFINED__ */


#ifndef __SMIScan_FWD_DEFINED__
#define __SMIScan_FWD_DEFINED__

#ifdef __cplusplus
typedef class SMIScan SMIScan;
#else
typedef struct SMIScan SMIScan;
#endif /* __cplusplus */

#endif 	/* __SMIScan_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_IntegMotorInterface_0000 */
/* [local] */ 

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_IntegMotorInterface_0000_0001
    {	smi_none	= 0,
	smi_odd	= 1,
	smi_even	= 2,
	smi_mark	= 3,
	smi_space	= 4
    } 	enum_parity;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_IntegMotorInterface_0000_0002
    {	smi_counterclockwise	= 0,
	smi_clockwise	= 1
    } 	enum_arcdirection;



extern RPC_IF_HANDLE __MIDL_itf_IntegMotorInterface_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IntegMotorInterface_0000_v0_0_s_ifspec;

#ifndef __ISMIMotor_INTERFACE_DEFINED__
#define __ISMIMotor_INTERFACE_DEFINED__

/* interface ISMIMotor */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMIMotor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0FBA614D-B688-4D74-9D8A-E592DA3F65D3")
    ISMIMotor : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPosition( 
            /* [retval][out] */ long *Pos) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_P( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_P( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_V( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_V( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_A( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_A( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSpeed( 
            /* [retval][out] */ long *Speed) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAcceleration( 
            /* [retval][out] */ long *Acceleration) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Go( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteCommand( 
            /* [string][in] */ BSTR Command) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResponseOf( 
            /* [string][in] */ BSTR Command,
            /* [string][retval][out] */ BSTR *Response) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPositionMode( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetVelocityMode( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SampleRate( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitForStop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( 
            /* [in] */ VARIANT_BOOL Decelerate) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GoTo( 
            /* [in] */ long Position,
            /* [in] */ long Velocity,
            /* [in] */ long Acceleration) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Download( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Upload( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DownloadEx( 
            /* [in] */ BSTR FileName,
            /* [in] */ long Flags,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UploadEx( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearProgram( 
            /* [in] */ long Flags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorInfo( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSerialNumber( 
            /* [string][retval][out] */ BSTR *SN) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetModelStr( 
            /* [string][retval][out] */ BSTR *ModelStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEncoderResolution( 
            /* [retval][out] */ long *EncoderResolution) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorByte( 
            /* [in] */ long Address,
            /* [retval][out] */ BYTE *TheByte) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorWord( 
            /* [in] */ long Address,
            /* [retval][out] */ WORD *TheWord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorDWord( 
            /* [in] */ long Address,
            /* [retval][out] */ DWORD *TheDWord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMotorByte( 
            /* [in] */ long Address,
            /* [in] */ BYTE TheByte) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMotorWord( 
            /* [in] */ long Address,
            /* [in] */ WORD TheWord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMotorDWord( 
            /* [in] */ long Address,
            /* [in] */ DWORD TheDWord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsServo( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DownloadProgramStr( 
            /* [in] */ BSTR ProgramStr,
            /* [in] */ long Flags,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UploadProgramStr( 
            /* [in] */ VARIANT_BOOL CalPrg,
            /* [string][retval][out] */ BSTR *ProgramStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpProgramStr( 
            /* [in] */ VARIANT_BOOL CalPrg,
            /* [string][retval][out] */ BSTR *ProgramStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorInt( 
            /* [in] */ long Address,
            /* [retval][out] */ short *TheInt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotorLong( 
            /* [in] */ long Address,
            /* [retval][out] */ long *TheLong) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMotorInt( 
            /* [in] */ long Address,
            /* [in] */ short TheInt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMotorLong( 
            /* [in] */ long Address,
            /* [in] */ long TheLong) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AT( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AT( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DT( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DT( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ADT( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PT( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PT( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VT( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_VT( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsClass5( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsClass9( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsClass5or9( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetIPAddressStr( 
            /* [string][retval][out] */ BSTR *IPAddressStr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStatusWord( 
            /* [in] */ SHORT DesiredWord,
            /* [retval][out] */ WORD *StatusWord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadLongSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ LONG *pValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadWordSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ WORD *pValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadByteSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ BYTE *pValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteLongSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ LONG Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteWordSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ WORD Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteByteSDO( 
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ BYTE Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProgramInfo( 
            /* [string][retval][out] */ BSTR *ProgramInfo) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KP( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KP( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KI( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KI( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KD( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KD( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KL( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KL( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KS( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KS( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KV( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KV( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KA( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KA( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_KG( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_KG( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteFilters( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadUserVariable( 
            /* [in] */ BSTR VariableName,
            /* [retval][out] */ LONG *Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteUserVariable( 
            /* [in] */ BSTR VariableName,
            /* [in] */ LONG newValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadOnboardIO( 
            /* [retval][out] */ LONG *pOnboardIO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadPositionError( 
            /* [retval][out] */ LONG *pPositionError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteMaxPositionError( 
            /* [in] */ LONG MaxPositionError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadMaxPositionError( 
            /* [retval][out] */ LONG *pMaxPositionError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProgramChecksum( 
            /* [retval][out] */ LONG *pProgramCheckSum) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetFaults( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProgramRun( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProgramEnd( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DriveDisable( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConfigurePort( 
            /* [in] */ LONG PortNumber,
            /* [in] */ LONG DesiredType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WritePositionOrigin( 
            /* [in] */ LONG PositionOrigin) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FollowAddress( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FollowAddress( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GoAndFollower( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopAndFollower( 
            /* [in] */ VARIANT_BOOL Decelerate) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PC( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCommandedSpeed( 
            /* [retval][out] */ LONG *CommandedSpeed) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadExpandedIO( 
            /* [retval][out] */ LONG *pExpandedIO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsClass6( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsClass5or6or9( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProgramInfoBytes( 
            /* [in] */ BYTE *pProgramInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMIMotorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMIMotor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMIMotor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMIMotor * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMIMotor * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMIMotor * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMIMotor * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMIMotor * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPosition )( 
            ISMIMotor * This,
            /* [retval][out] */ long *Pos);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_P )( 
            ISMIMotor * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_P )( 
            ISMIMotor * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_V )( 
            ISMIMotor * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_V )( 
            ISMIMotor * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_A )( 
            ISMIMotor * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_A )( 
            ISMIMotor * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSpeed )( 
            ISMIMotor * This,
            /* [retval][out] */ long *Speed);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAcceleration )( 
            ISMIMotor * This,
            /* [retval][out] */ long *Acceleration);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Go )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteCommand )( 
            ISMIMotor * This,
            /* [string][in] */ BSTR Command);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResponseOf )( 
            ISMIMotor * This,
            /* [string][in] */ BSTR Command,
            /* [string][retval][out] */ BSTR *Response);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetPositionMode )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetVelocityMode )( 
            ISMIMotor * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            ISMIMotor * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SampleRate )( 
            ISMIMotor * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitForStop )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            ISMIMotor * This,
            /* [in] */ VARIANT_BOOL Decelerate);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoTo )( 
            ISMIMotor * This,
            /* [in] */ long Position,
            /* [in] */ long Velocity,
            /* [in] */ long Acceleration);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Download )( 
            ISMIMotor * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Upload )( 
            ISMIMotor * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DownloadEx )( 
            ISMIMotor * This,
            /* [in] */ BSTR FileName,
            /* [in] */ long Flags,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UploadEx )( 
            ISMIMotor * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearProgram )( 
            ISMIMotor * This,
            /* [in] */ long Flags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorInfo )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSerialNumber )( 
            ISMIMotor * This,
            /* [string][retval][out] */ BSTR *SN);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetModelStr )( 
            ISMIMotor * This,
            /* [string][retval][out] */ BSTR *ModelStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEncoderResolution )( 
            ISMIMotor * This,
            /* [retval][out] */ long *EncoderResolution);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorByte )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [retval][out] */ BYTE *TheByte);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorWord )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [retval][out] */ WORD *TheWord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorDWord )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [retval][out] */ DWORD *TheDWord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMotorByte )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [in] */ BYTE TheByte);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMotorWord )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [in] */ WORD TheWord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMotorDWord )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [in] */ DWORD TheDWord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsServo )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DownloadProgramStr )( 
            ISMIMotor * This,
            /* [in] */ BSTR ProgramStr,
            /* [in] */ long Flags,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UploadProgramStr )( 
            ISMIMotor * This,
            /* [in] */ VARIANT_BOOL CalPrg,
            /* [string][retval][out] */ BSTR *ProgramStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpProgramStr )( 
            ISMIMotor * This,
            /* [in] */ VARIANT_BOOL CalPrg,
            /* [string][retval][out] */ BSTR *ProgramStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorInt )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [retval][out] */ short *TheInt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotorLong )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [retval][out] */ long *TheLong);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMotorInt )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [in] */ short TheInt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMotorLong )( 
            ISMIMotor * This,
            /* [in] */ long Address,
            /* [in] */ long TheLong);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AT )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AT )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DT )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DT )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ADT )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PT )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PT )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_VT )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_VT )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsClass5 )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsClass9 )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsClass5or9 )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetIPAddressStr )( 
            ISMIMotor * This,
            /* [string][retval][out] */ BSTR *IPAddressStr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStatusWord )( 
            ISMIMotor * This,
            /* [in] */ SHORT DesiredWord,
            /* [retval][out] */ WORD *StatusWord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLongSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ LONG *pValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadWordSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ WORD *pValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadByteSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [retval][out] */ BYTE *pValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteLongSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ LONG Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteWordSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ WORD Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteByteSDO )( 
            ISMIMotor * This,
            /* [in] */ LONG index,
            /* [in] */ LONG subindex,
            /* [in] */ BYTE Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProgramInfo )( 
            ISMIMotor * This,
            /* [string][retval][out] */ BSTR *ProgramInfo);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KP )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KP )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KI )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KI )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KD )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KD )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KL )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KL )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KS )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KS )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KV )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KV )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KA )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KA )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_KG )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_KG )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteFilters )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadUserVariable )( 
            ISMIMotor * This,
            /* [in] */ BSTR VariableName,
            /* [retval][out] */ LONG *Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteUserVariable )( 
            ISMIMotor * This,
            /* [in] */ BSTR VariableName,
            /* [in] */ LONG newValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadOnboardIO )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pOnboardIO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadPositionError )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pPositionError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteMaxPositionError )( 
            ISMIMotor * This,
            /* [in] */ LONG MaxPositionError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadMaxPositionError )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pMaxPositionError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProgramChecksum )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pProgramCheckSum);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetFaults )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProgramRun )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ProgramEnd )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DriveDisable )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ConfigurePort )( 
            ISMIMotor * This,
            /* [in] */ LONG PortNumber,
            /* [in] */ LONG DesiredType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WritePositionOrigin )( 
            ISMIMotor * This,
            /* [in] */ LONG PositionOrigin);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FollowAddress )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_FollowAddress )( 
            ISMIMotor * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoAndFollower )( 
            ISMIMotor * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StopAndFollower )( 
            ISMIMotor * This,
            /* [in] */ VARIANT_BOOL Decelerate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PC )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCommandedSpeed )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *CommandedSpeed);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadExpandedIO )( 
            ISMIMotor * This,
            /* [retval][out] */ LONG *pExpandedIO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsClass6 )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsClass5or6or9 )( 
            ISMIMotor * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProgramInfoBytes )( 
            ISMIMotor * This,
            /* [in] */ BYTE *pProgramInfo);
        
        END_INTERFACE
    } ISMIMotorVtbl;

    interface ISMIMotor
    {
        CONST_VTBL struct ISMIMotorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMIMotor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMIMotor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMIMotor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMIMotor_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMIMotor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMIMotor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMIMotor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMIMotor_GetPosition(This,Pos)	\
    (This)->lpVtbl -> GetPosition(This,Pos)

#define ISMIMotor_get_P(This,pVal)	\
    (This)->lpVtbl -> get_P(This,pVal)

#define ISMIMotor_put_P(This,newVal)	\
    (This)->lpVtbl -> put_P(This,newVal)

#define ISMIMotor_get_V(This,pVal)	\
    (This)->lpVtbl -> get_V(This,pVal)

#define ISMIMotor_put_V(This,newVal)	\
    (This)->lpVtbl -> put_V(This,newVal)

#define ISMIMotor_get_A(This,pVal)	\
    (This)->lpVtbl -> get_A(This,pVal)

#define ISMIMotor_put_A(This,newVal)	\
    (This)->lpVtbl -> put_A(This,newVal)

#define ISMIMotor_GetSpeed(This,Speed)	\
    (This)->lpVtbl -> GetSpeed(This,Speed)

#define ISMIMotor_GetAcceleration(This,Acceleration)	\
    (This)->lpVtbl -> GetAcceleration(This,Acceleration)

#define ISMIMotor_Go(This)	\
    (This)->lpVtbl -> Go(This)

#define ISMIMotor_WriteCommand(This,Command)	\
    (This)->lpVtbl -> WriteCommand(This,Command)

#define ISMIMotor_GetResponseOf(This,Command,Response)	\
    (This)->lpVtbl -> GetResponseOf(This,Command,Response)

#define ISMIMotor_SetPositionMode(This)	\
    (This)->lpVtbl -> SetPositionMode(This)

#define ISMIMotor_SetVelocityMode(This)	\
    (This)->lpVtbl -> SetVelocityMode(This)

#define ISMIMotor_get_Version(This,pVal)	\
    (This)->lpVtbl -> get_Version(This,pVal)

#define ISMIMotor_get_SampleRate(This,pVal)	\
    (This)->lpVtbl -> get_SampleRate(This,pVal)

#define ISMIMotor_WaitForStop(This)	\
    (This)->lpVtbl -> WaitForStop(This)

#define ISMIMotor_Stop(This,Decelerate)	\
    (This)->lpVtbl -> Stop(This,Decelerate)

#define ISMIMotor_GoTo(This,Position,Velocity,Acceleration)	\
    (This)->lpVtbl -> GoTo(This,Position,Velocity,Acceleration)

#define ISMIMotor_Download(This,FileName)	\
    (This)->lpVtbl -> Download(This,FileName)

#define ISMIMotor_Upload(This,FileName)	\
    (This)->lpVtbl -> Upload(This,FileName)

#define ISMIMotor_DownloadEx(This,FileName,Flags,pVal)	\
    (This)->lpVtbl -> DownloadEx(This,FileName,Flags,pVal)

#define ISMIMotor_UploadEx(This,FileName)	\
    (This)->lpVtbl -> UploadEx(This,FileName)

#define ISMIMotor_ClearProgram(This,Flags)	\
    (This)->lpVtbl -> ClearProgram(This,Flags)

#define ISMIMotor_GetMotorInfo(This,pVal)	\
    (This)->lpVtbl -> GetMotorInfo(This,pVal)

#define ISMIMotor_GetSerialNumber(This,SN)	\
    (This)->lpVtbl -> GetSerialNumber(This,SN)

#define ISMIMotor_GetModelStr(This,ModelStr)	\
    (This)->lpVtbl -> GetModelStr(This,ModelStr)

#define ISMIMotor_GetEncoderResolution(This,EncoderResolution)	\
    (This)->lpVtbl -> GetEncoderResolution(This,EncoderResolution)

#define ISMIMotor_GetMotorByte(This,Address,TheByte)	\
    (This)->lpVtbl -> GetMotorByte(This,Address,TheByte)

#define ISMIMotor_GetMotorWord(This,Address,TheWord)	\
    (This)->lpVtbl -> GetMotorWord(This,Address,TheWord)

#define ISMIMotor_GetMotorDWord(This,Address,TheDWord)	\
    (This)->lpVtbl -> GetMotorDWord(This,Address,TheDWord)

#define ISMIMotor_SetMotorByte(This,Address,TheByte)	\
    (This)->lpVtbl -> SetMotorByte(This,Address,TheByte)

#define ISMIMotor_SetMotorWord(This,Address,TheWord)	\
    (This)->lpVtbl -> SetMotorWord(This,Address,TheWord)

#define ISMIMotor_SetMotorDWord(This,Address,TheDWord)	\
    (This)->lpVtbl -> SetMotorDWord(This,Address,TheDWord)

#define ISMIMotor_IsServo(This,pVal)	\
    (This)->lpVtbl -> IsServo(This,pVal)

#define ISMIMotor_DownloadProgramStr(This,ProgramStr,Flags,pVal)	\
    (This)->lpVtbl -> DownloadProgramStr(This,ProgramStr,Flags,pVal)

#define ISMIMotor_UploadProgramStr(This,CalPrg,ProgramStr)	\
    (This)->lpVtbl -> UploadProgramStr(This,CalPrg,ProgramStr)

#define ISMIMotor_UpProgramStr(This,CalPrg,ProgramStr)	\
    (This)->lpVtbl -> UpProgramStr(This,CalPrg,ProgramStr)

#define ISMIMotor_GetMotorInt(This,Address,TheInt)	\
    (This)->lpVtbl -> GetMotorInt(This,Address,TheInt)

#define ISMIMotor_GetMotorLong(This,Address,TheLong)	\
    (This)->lpVtbl -> GetMotorLong(This,Address,TheLong)

#define ISMIMotor_SetMotorInt(This,Address,TheInt)	\
    (This)->lpVtbl -> SetMotorInt(This,Address,TheInt)

#define ISMIMotor_SetMotorLong(This,Address,TheLong)	\
    (This)->lpVtbl -> SetMotorLong(This,Address,TheLong)

#define ISMIMotor_get_AT(This,pVal)	\
    (This)->lpVtbl -> get_AT(This,pVal)

#define ISMIMotor_put_AT(This,newVal)	\
    (This)->lpVtbl -> put_AT(This,newVal)

#define ISMIMotor_get_DT(This,pVal)	\
    (This)->lpVtbl -> get_DT(This,pVal)

#define ISMIMotor_put_DT(This,newVal)	\
    (This)->lpVtbl -> put_DT(This,newVal)

#define ISMIMotor_put_ADT(This,newVal)	\
    (This)->lpVtbl -> put_ADT(This,newVal)

#define ISMIMotor_get_PT(This,pVal)	\
    (This)->lpVtbl -> get_PT(This,pVal)

#define ISMIMotor_put_PT(This,newVal)	\
    (This)->lpVtbl -> put_PT(This,newVal)

#define ISMIMotor_get_VT(This,pVal)	\
    (This)->lpVtbl -> get_VT(This,pVal)

#define ISMIMotor_put_VT(This,newVal)	\
    (This)->lpVtbl -> put_VT(This,newVal)

#define ISMIMotor_IsClass5(This,pVal)	\
    (This)->lpVtbl -> IsClass5(This,pVal)

#define ISMIMotor_IsClass9(This,pVal)	\
    (This)->lpVtbl -> IsClass9(This,pVal)

#define ISMIMotor_IsClass5or9(This,pVal)	\
    (This)->lpVtbl -> IsClass5or9(This,pVal)

#define ISMIMotor_GetIPAddressStr(This,IPAddressStr)	\
    (This)->lpVtbl -> GetIPAddressStr(This,IPAddressStr)

#define ISMIMotor_GetStatusWord(This,DesiredWord,StatusWord)	\
    (This)->lpVtbl -> GetStatusWord(This,DesiredWord,StatusWord)

#define ISMIMotor_ReadLongSDO(This,index,subindex,pValue)	\
    (This)->lpVtbl -> ReadLongSDO(This,index,subindex,pValue)

#define ISMIMotor_ReadWordSDO(This,index,subindex,pValue)	\
    (This)->lpVtbl -> ReadWordSDO(This,index,subindex,pValue)

#define ISMIMotor_ReadByteSDO(This,index,subindex,pValue)	\
    (This)->lpVtbl -> ReadByteSDO(This,index,subindex,pValue)

#define ISMIMotor_WriteLongSDO(This,index,subindex,Value)	\
    (This)->lpVtbl -> WriteLongSDO(This,index,subindex,Value)

#define ISMIMotor_WriteWordSDO(This,index,subindex,Value)	\
    (This)->lpVtbl -> WriteWordSDO(This,index,subindex,Value)

#define ISMIMotor_WriteByteSDO(This,index,subindex,Value)	\
    (This)->lpVtbl -> WriteByteSDO(This,index,subindex,Value)

#define ISMIMotor_GetProgramInfo(This,ProgramInfo)	\
    (This)->lpVtbl -> GetProgramInfo(This,ProgramInfo)

#define ISMIMotor_get_KP(This,pVal)	\
    (This)->lpVtbl -> get_KP(This,pVal)

#define ISMIMotor_put_KP(This,newVal)	\
    (This)->lpVtbl -> put_KP(This,newVal)

#define ISMIMotor_get_KI(This,pVal)	\
    (This)->lpVtbl -> get_KI(This,pVal)

#define ISMIMotor_put_KI(This,newVal)	\
    (This)->lpVtbl -> put_KI(This,newVal)

#define ISMIMotor_get_KD(This,pVal)	\
    (This)->lpVtbl -> get_KD(This,pVal)

#define ISMIMotor_put_KD(This,newVal)	\
    (This)->lpVtbl -> put_KD(This,newVal)

#define ISMIMotor_get_KL(This,pVal)	\
    (This)->lpVtbl -> get_KL(This,pVal)

#define ISMIMotor_put_KL(This,newVal)	\
    (This)->lpVtbl -> put_KL(This,newVal)

#define ISMIMotor_get_KS(This,pVal)	\
    (This)->lpVtbl -> get_KS(This,pVal)

#define ISMIMotor_put_KS(This,newVal)	\
    (This)->lpVtbl -> put_KS(This,newVal)

#define ISMIMotor_get_KV(This,pVal)	\
    (This)->lpVtbl -> get_KV(This,pVal)

#define ISMIMotor_put_KV(This,newVal)	\
    (This)->lpVtbl -> put_KV(This,newVal)

#define ISMIMotor_get_KA(This,pVal)	\
    (This)->lpVtbl -> get_KA(This,pVal)

#define ISMIMotor_put_KA(This,newVal)	\
    (This)->lpVtbl -> put_KA(This,newVal)

#define ISMIMotor_get_KG(This,pVal)	\
    (This)->lpVtbl -> get_KG(This,pVal)

#define ISMIMotor_put_KG(This,newVal)	\
    (This)->lpVtbl -> put_KG(This,newVal)

#define ISMIMotor_WriteFilters(This)	\
    (This)->lpVtbl -> WriteFilters(This)

#define ISMIMotor_ReadUserVariable(This,VariableName,Value)	\
    (This)->lpVtbl -> ReadUserVariable(This,VariableName,Value)

#define ISMIMotor_WriteUserVariable(This,VariableName,newValue)	\
    (This)->lpVtbl -> WriteUserVariable(This,VariableName,newValue)

#define ISMIMotor_ReadOnboardIO(This,pOnboardIO)	\
    (This)->lpVtbl -> ReadOnboardIO(This,pOnboardIO)

#define ISMIMotor_ReadPositionError(This,pPositionError)	\
    (This)->lpVtbl -> ReadPositionError(This,pPositionError)

#define ISMIMotor_WriteMaxPositionError(This,MaxPositionError)	\
    (This)->lpVtbl -> WriteMaxPositionError(This,MaxPositionError)

#define ISMIMotor_ReadMaxPositionError(This,pMaxPositionError)	\
    (This)->lpVtbl -> ReadMaxPositionError(This,pMaxPositionError)

#define ISMIMotor_GetProgramChecksum(This,pProgramCheckSum)	\
    (This)->lpVtbl -> GetProgramChecksum(This,pProgramCheckSum)

#define ISMIMotor_ResetFaults(This)	\
    (This)->lpVtbl -> ResetFaults(This)

#define ISMIMotor_ProgramRun(This)	\
    (This)->lpVtbl -> ProgramRun(This)

#define ISMIMotor_ProgramEnd(This)	\
    (This)->lpVtbl -> ProgramEnd(This)

#define ISMIMotor_DriveDisable(This)	\
    (This)->lpVtbl -> DriveDisable(This)

#define ISMIMotor_ConfigurePort(This,PortNumber,DesiredType)	\
    (This)->lpVtbl -> ConfigurePort(This,PortNumber,DesiredType)

#define ISMIMotor_WritePositionOrigin(This,PositionOrigin)	\
    (This)->lpVtbl -> WritePositionOrigin(This,PositionOrigin)

#define ISMIMotor_get_FollowAddress(This,pVal)	\
    (This)->lpVtbl -> get_FollowAddress(This,pVal)

#define ISMIMotor_put_FollowAddress(This,newVal)	\
    (This)->lpVtbl -> put_FollowAddress(This,newVal)

#define ISMIMotor_GoAndFollower(This)	\
    (This)->lpVtbl -> GoAndFollower(This)

#define ISMIMotor_StopAndFollower(This,Decelerate)	\
    (This)->lpVtbl -> StopAndFollower(This,Decelerate)

#define ISMIMotor_get_PC(This,pVal)	\
    (This)->lpVtbl -> get_PC(This,pVal)

#define ISMIMotor_GetCommandedSpeed(This,CommandedSpeed)	\
    (This)->lpVtbl -> GetCommandedSpeed(This,CommandedSpeed)

#define ISMIMotor_ReadExpandedIO(This,pExpandedIO)	\
    (This)->lpVtbl -> ReadExpandedIO(This,pExpandedIO)

#define ISMIMotor_IsClass6(This,pVal)	\
    (This)->lpVtbl -> IsClass6(This,pVal)

#define ISMIMotor_IsClass5or6or9(This,pVal)	\
    (This)->lpVtbl -> IsClass5or6or9(This,pVal)

#define ISMIMotor_GetProgramInfoBytes(This,pProgramInfo)	\
    (This)->lpVtbl -> GetProgramInfoBytes(This,pProgramInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetPosition_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *Pos);


void __RPC_STUB ISMIMotor_GetPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_P_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIMotor_get_P_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_P_Proxy( 
    ISMIMotor * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIMotor_put_P_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_V_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIMotor_get_V_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_V_Proxy( 
    ISMIMotor * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIMotor_put_V_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_A_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIMotor_get_A_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_A_Proxy( 
    ISMIMotor * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIMotor_put_A_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetSpeed_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *Speed);


void __RPC_STUB ISMIMotor_GetSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetAcceleration_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *Acceleration);


void __RPC_STUB ISMIMotor_GetAcceleration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_Go_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_Go_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteCommand_Proxy( 
    ISMIMotor * This,
    /* [string][in] */ BSTR Command);


void __RPC_STUB ISMIMotor_WriteCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetResponseOf_Proxy( 
    ISMIMotor * This,
    /* [string][in] */ BSTR Command,
    /* [string][retval][out] */ BSTR *Response);


void __RPC_STUB ISMIMotor_GetResponseOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetPositionMode_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_SetPositionMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetVelocityMode_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_SetVelocityMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_Version_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISMIMotor_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_SampleRate_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMIMotor_get_SampleRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WaitForStop_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_WaitForStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_Stop_Proxy( 
    ISMIMotor * This,
    /* [in] */ VARIANT_BOOL Decelerate);


void __RPC_STUB ISMIMotor_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GoTo_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Position,
    /* [in] */ long Velocity,
    /* [in] */ long Acceleration);


void __RPC_STUB ISMIMotor_GoTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_Download_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIMotor_Download_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_Upload_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIMotor_Upload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_DownloadEx_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR FileName,
    /* [in] */ long Flags,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_DownloadEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_UploadEx_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIMotor_UploadEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ClearProgram_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Flags);


void __RPC_STUB ISMIMotor_ClearProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorInfo_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_GetMotorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetSerialNumber_Proxy( 
    ISMIMotor * This,
    /* [string][retval][out] */ BSTR *SN);


void __RPC_STUB ISMIMotor_GetSerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetModelStr_Proxy( 
    ISMIMotor * This,
    /* [string][retval][out] */ BSTR *ModelStr);


void __RPC_STUB ISMIMotor_GetModelStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetEncoderResolution_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ long *EncoderResolution);


void __RPC_STUB ISMIMotor_GetEncoderResolution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorByte_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [retval][out] */ BYTE *TheByte);


void __RPC_STUB ISMIMotor_GetMotorByte_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorWord_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [retval][out] */ WORD *TheWord);


void __RPC_STUB ISMIMotor_GetMotorWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorDWord_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [retval][out] */ DWORD *TheDWord);


void __RPC_STUB ISMIMotor_GetMotorDWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetMotorByte_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [in] */ BYTE TheByte);


void __RPC_STUB ISMIMotor_SetMotorByte_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetMotorWord_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [in] */ WORD TheWord);


void __RPC_STUB ISMIMotor_SetMotorWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetMotorDWord_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [in] */ DWORD TheDWord);


void __RPC_STUB ISMIMotor_SetMotorDWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsServo_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsServo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_DownloadProgramStr_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR ProgramStr,
    /* [in] */ long Flags,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_DownloadProgramStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_UploadProgramStr_Proxy( 
    ISMIMotor * This,
    /* [in] */ VARIANT_BOOL CalPrg,
    /* [string][retval][out] */ BSTR *ProgramStr);


void __RPC_STUB ISMIMotor_UploadProgramStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_UpProgramStr_Proxy( 
    ISMIMotor * This,
    /* [in] */ VARIANT_BOOL CalPrg,
    /* [string][retval][out] */ BSTR *ProgramStr);


void __RPC_STUB ISMIMotor_UpProgramStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorInt_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [retval][out] */ short *TheInt);


void __RPC_STUB ISMIMotor_GetMotorInt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetMotorLong_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [retval][out] */ long *TheLong);


void __RPC_STUB ISMIMotor_GetMotorLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetMotorInt_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [in] */ short TheInt);


void __RPC_STUB ISMIMotor_SetMotorInt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_SetMotorLong_Proxy( 
    ISMIMotor * This,
    /* [in] */ long Address,
    /* [in] */ long TheLong);


void __RPC_STUB ISMIMotor_SetMotorLong_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_AT_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_AT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_AT_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_AT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_DT_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_DT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_DT_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_DT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_ADT_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_ADT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_PT_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_PT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_PT_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_PT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_VT_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_VT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_VT_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_VT_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsClass5_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsClass5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsClass9_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsClass9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsClass5or9_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsClass5or9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetIPAddressStr_Proxy( 
    ISMIMotor * This,
    /* [string][retval][out] */ BSTR *IPAddressStr);


void __RPC_STUB ISMIMotor_GetIPAddressStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetStatusWord_Proxy( 
    ISMIMotor * This,
    /* [in] */ SHORT DesiredWord,
    /* [retval][out] */ WORD *StatusWord);


void __RPC_STUB ISMIMotor_GetStatusWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadLongSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [retval][out] */ LONG *pValue);


void __RPC_STUB ISMIMotor_ReadLongSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadWordSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [retval][out] */ WORD *pValue);


void __RPC_STUB ISMIMotor_ReadWordSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadByteSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [retval][out] */ BYTE *pValue);


void __RPC_STUB ISMIMotor_ReadByteSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteLongSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [in] */ LONG Value);


void __RPC_STUB ISMIMotor_WriteLongSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteWordSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [in] */ WORD Value);


void __RPC_STUB ISMIMotor_WriteWordSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteByteSDO_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG index,
    /* [in] */ LONG subindex,
    /* [in] */ BYTE Value);


void __RPC_STUB ISMIMotor_WriteByteSDO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetProgramInfo_Proxy( 
    ISMIMotor * This,
    /* [string][retval][out] */ BSTR *ProgramInfo);


void __RPC_STUB ISMIMotor_GetProgramInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KP_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KP_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KI_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KI_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KD_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KD_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KD_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KD_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KL_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KL_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KS_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KS_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KV_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KV_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KV_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KV_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KA_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KA_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_KG_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_KG_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_KG_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_KG_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteFilters_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_WriteFilters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadUserVariable_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR VariableName,
    /* [retval][out] */ LONG *Value);


void __RPC_STUB ISMIMotor_ReadUserVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteUserVariable_Proxy( 
    ISMIMotor * This,
    /* [in] */ BSTR VariableName,
    /* [in] */ LONG newValue);


void __RPC_STUB ISMIMotor_WriteUserVariable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadOnboardIO_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pOnboardIO);


void __RPC_STUB ISMIMotor_ReadOnboardIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadPositionError_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pPositionError);


void __RPC_STUB ISMIMotor_ReadPositionError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WriteMaxPositionError_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG MaxPositionError);


void __RPC_STUB ISMIMotor_WriteMaxPositionError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadMaxPositionError_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pMaxPositionError);


void __RPC_STUB ISMIMotor_ReadMaxPositionError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetProgramChecksum_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pProgramCheckSum);


void __RPC_STUB ISMIMotor_GetProgramChecksum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ResetFaults_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_ResetFaults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ProgramRun_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_ProgramRun_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ProgramEnd_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_ProgramEnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_DriveDisable_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_DriveDisable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ConfigurePort_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG PortNumber,
    /* [in] */ LONG DesiredType);


void __RPC_STUB ISMIMotor_ConfigurePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_WritePositionOrigin_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG PositionOrigin);


void __RPC_STUB ISMIMotor_WritePositionOrigin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_FollowAddress_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_FollowAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIMotor_put_FollowAddress_Proxy( 
    ISMIMotor * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIMotor_put_FollowAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GoAndFollower_Proxy( 
    ISMIMotor * This);


void __RPC_STUB ISMIMotor_GoAndFollower_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_StopAndFollower_Proxy( 
    ISMIMotor * This,
    /* [in] */ VARIANT_BOOL Decelerate);


void __RPC_STUB ISMIMotor_StopAndFollower_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIMotor_get_PC_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIMotor_get_PC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetCommandedSpeed_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *CommandedSpeed);


void __RPC_STUB ISMIMotor_GetCommandedSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_ReadExpandedIO_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ LONG *pExpandedIO);


void __RPC_STUB ISMIMotor_ReadExpandedIO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsClass6_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsClass6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_IsClass5or6or9_Proxy( 
    ISMIMotor * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIMotor_IsClass5or6or9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIMotor_GetProgramInfoBytes_Proxy( 
    ISMIMotor * This,
    /* [in] */ BYTE *pProgramInfo);


void __RPC_STUB ISMIMotor_GetProgramInfoBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMIMotor_INTERFACE_DEFINED__ */


#ifndef __ISMIComm_INTERFACE_DEFINED__
#define __ISMIComm_INTERFACE_DEFINED__

/* interface ISMIComm */
/* [helpcontext][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMIComm;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BC26FBB8-00E1-11D4-B5D0-00600831E1F3")
    ISMIComm : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenPort( 
            BSTR PortName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClosePort( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PortHandle( 
            /* [retval][out] */ long **pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearBuffer( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BytesAvailable( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteCommand( 
            /* [string][in] */ BSTR Command) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteString( 
            /* [string][in] */ BSTR Data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadResponse( 
            /* [string][retval][out] */ BSTR *Response) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadString( 
            /* [in] */ long NoOfBytes,
            /* [string][retval][out] */ BSTR *Data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EstablishChain( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddressMotorChain( 
            /* [retval][out] */ long *MotorCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wait( 
            /* [in] */ long Time) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DefaultMotor( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultMotor( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Echo( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Echo( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parity( 
            /* [retval][out] */ enum_parity *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Parity( 
            /* [in] */ enum_parity newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BaudRate( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BaudRate( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Timeout( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Timeout( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CharDelay( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CharDelay( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PortName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResponseOf( 
            /* [string][in] */ BSTR Command,
            /* [string][retval][out] */ BSTR *Response) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMotor( 
            /* [in] */ long Address,
            /* [retval][out] */ ISMIMotor **ppMotor) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxMotors( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxMotors( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Download( 
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Upload( 
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ForceUpload( 
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsRS485( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IsRS485( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearEEPROM( 
            /* [in] */ long Motor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitializeNotification( 
            /* [in] */ long hWindow,
            /* [in] */ long message) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteCmd( 
            /* [string][in] */ BSTR Command) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitRS485( 
            /* [in] */ long NoOfMotors,
            /* [in] */ long MotorChannel) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TxMaxRetry( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TxMaxRetry( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TxTimeoutMul( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TxTimeoutMul( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TxTimeoutConst( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TxTimeoutConst( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFlags( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFlags( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFileName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFileName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EchoTimeoutMul( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EchoTimeoutMul( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EchoTimeoutConst( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EchoTimeoutConst( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DetectRS232( 
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DetectRS485( 
            /* [in] */ long MaxAddress,
            /* [in] */ long MotorChannel,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddressServos( 
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NoOfMotors( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EngineVersion( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitEthernet( 
            /* [in] */ long MaxAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DetectNetMotors( 
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddNetMotor( 
            /* [string][in] */ BSTR IPAddress,
            /* [retval][out] */ long *MotorAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitUSB( 
            /* [in] */ long MaxAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DetectUSBMotors( 
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReorderMotors( 
            /* [in] */ long Address,
            /* [string][in] */ BSTR SerialNo,
            /* [retval][out] */ long *OldMotorAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InitCANOpen( 
            /* [in] */ long MaxAddress) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DetectCANOpenMotors( 
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadCharacters( 
            /* [in] */ LONG NoOfBytes,
            /* [retval][out] */ BSTR *pData) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DelayEchoOff( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DelayEchoOff( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMICommVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMIComm * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMIComm * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMIComm * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMIComm * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMIComm * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMIComm * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMIComm * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenPort )( 
            ISMIComm * This,
            BSTR PortName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClosePort )( 
            ISMIComm * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PortHandle )( 
            ISMIComm * This,
            /* [retval][out] */ long **pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearBuffer )( 
            ISMIComm * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BytesAvailable )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteCommand )( 
            ISMIComm * This,
            /* [string][in] */ BSTR Command);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteString )( 
            ISMIComm * This,
            /* [string][in] */ BSTR Data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadResponse )( 
            ISMIComm * This,
            /* [string][retval][out] */ BSTR *Response);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadString )( 
            ISMIComm * This,
            /* [in] */ long NoOfBytes,
            /* [string][retval][out] */ BSTR *Data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EstablishChain )( 
            ISMIComm * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddressMotorChain )( 
            ISMIComm * This,
            /* [retval][out] */ long *MotorCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Wait )( 
            ISMIComm * This,
            /* [in] */ long Time);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultMotor )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DefaultMotor )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Echo )( 
            ISMIComm * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Echo )( 
            ISMIComm * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parity )( 
            ISMIComm * This,
            /* [retval][out] */ enum_parity *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Parity )( 
            ISMIComm * This,
            /* [in] */ enum_parity newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaudRate )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BaudRate )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Timeout )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Timeout )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CharDelay )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CharDelay )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PortName )( 
            ISMIComm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResponseOf )( 
            ISMIComm * This,
            /* [string][in] */ BSTR Command,
            /* [string][retval][out] */ BSTR *Response);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMotor )( 
            ISMIComm * This,
            /* [in] */ long Address,
            /* [retval][out] */ ISMIMotor **ppMotor);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaxMotors )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaxMotors )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Download )( 
            ISMIComm * This,
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Upload )( 
            ISMIComm * This,
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ForceUpload )( 
            ISMIComm * This,
            /* [in] */ long Motor,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsRS485 )( 
            ISMIComm * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IsRS485 )( 
            ISMIComm * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearEEPROM )( 
            ISMIComm * This,
            /* [in] */ long Motor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitializeNotification )( 
            ISMIComm * This,
            /* [in] */ long hWindow,
            /* [in] */ long message);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteCmd )( 
            ISMIComm * This,
            /* [string][in] */ BSTR Command);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitRS485 )( 
            ISMIComm * This,
            /* [in] */ long NoOfMotors,
            /* [in] */ long MotorChannel);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TxMaxRetry )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TxMaxRetry )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TxTimeoutMul )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TxTimeoutMul )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TxTimeoutConst )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TxTimeoutConst )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LogFlags )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LogFlags )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LogFileName )( 
            ISMIComm * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LogFileName )( 
            ISMIComm * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EchoTimeoutMul )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EchoTimeoutMul )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EchoTimeoutConst )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EchoTimeoutConst )( 
            ISMIComm * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DetectRS232 )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DetectRS485 )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress,
            /* [in] */ long MotorChannel,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddressServos )( 
            ISMIComm * This,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NoOfMotors )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EngineVersion )( 
            ISMIComm * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitEthernet )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DetectNetMotors )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddNetMotor )( 
            ISMIComm * This,
            /* [string][in] */ BSTR IPAddress,
            /* [retval][out] */ long *MotorAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitUSB )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DetectUSBMotors )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReorderMotors )( 
            ISMIComm * This,
            /* [in] */ long Address,
            /* [string][in] */ BSTR SerialNo,
            /* [retval][out] */ long *OldMotorAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InitCANOpen )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DetectCANOpenMotors )( 
            ISMIComm * This,
            /* [in] */ long MaxAddress,
            /* [in] */ long Flags,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadCharacters )( 
            ISMIComm * This,
            /* [in] */ LONG NoOfBytes,
            /* [retval][out] */ BSTR *pData);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DelayEchoOff )( 
            ISMIComm * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DelayEchoOff )( 
            ISMIComm * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } ISMICommVtbl;

    interface ISMIComm
    {
        CONST_VTBL struct ISMICommVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMIComm_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMIComm_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMIComm_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMIComm_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMIComm_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMIComm_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMIComm_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMIComm_OpenPort(This,PortName)	\
    (This)->lpVtbl -> OpenPort(This,PortName)

#define ISMIComm_ClosePort(This)	\
    (This)->lpVtbl -> ClosePort(This)

#define ISMIComm_get_PortHandle(This,pVal)	\
    (This)->lpVtbl -> get_PortHandle(This,pVal)

#define ISMIComm_ClearBuffer(This)	\
    (This)->lpVtbl -> ClearBuffer(This)

#define ISMIComm_get_BytesAvailable(This,pVal)	\
    (This)->lpVtbl -> get_BytesAvailable(This,pVal)

#define ISMIComm_WriteCommand(This,Command)	\
    (This)->lpVtbl -> WriteCommand(This,Command)

#define ISMIComm_WriteString(This,Data)	\
    (This)->lpVtbl -> WriteString(This,Data)

#define ISMIComm_ReadResponse(This,Response)	\
    (This)->lpVtbl -> ReadResponse(This,Response)

#define ISMIComm_ReadString(This,NoOfBytes,Data)	\
    (This)->lpVtbl -> ReadString(This,NoOfBytes,Data)

#define ISMIComm_EstablishChain(This)	\
    (This)->lpVtbl -> EstablishChain(This)

#define ISMIComm_AddressMotorChain(This,MotorCount)	\
    (This)->lpVtbl -> AddressMotorChain(This,MotorCount)

#define ISMIComm_Wait(This,Time)	\
    (This)->lpVtbl -> Wait(This,Time)

#define ISMIComm_get_DefaultMotor(This,pVal)	\
    (This)->lpVtbl -> get_DefaultMotor(This,pVal)

#define ISMIComm_put_DefaultMotor(This,newVal)	\
    (This)->lpVtbl -> put_DefaultMotor(This,newVal)

#define ISMIComm_get_Echo(This,pVal)	\
    (This)->lpVtbl -> get_Echo(This,pVal)

#define ISMIComm_put_Echo(This,newVal)	\
    (This)->lpVtbl -> put_Echo(This,newVal)

#define ISMIComm_get_Parity(This,pVal)	\
    (This)->lpVtbl -> get_Parity(This,pVal)

#define ISMIComm_put_Parity(This,newVal)	\
    (This)->lpVtbl -> put_Parity(This,newVal)

#define ISMIComm_get_BaudRate(This,pVal)	\
    (This)->lpVtbl -> get_BaudRate(This,pVal)

#define ISMIComm_put_BaudRate(This,newVal)	\
    (This)->lpVtbl -> put_BaudRate(This,newVal)

#define ISMIComm_get_Timeout(This,pVal)	\
    (This)->lpVtbl -> get_Timeout(This,pVal)

#define ISMIComm_put_Timeout(This,newVal)	\
    (This)->lpVtbl -> put_Timeout(This,newVal)

#define ISMIComm_get_CharDelay(This,pVal)	\
    (This)->lpVtbl -> get_CharDelay(This,pVal)

#define ISMIComm_put_CharDelay(This,newVal)	\
    (This)->lpVtbl -> put_CharDelay(This,newVal)

#define ISMIComm_get_PortName(This,pVal)	\
    (This)->lpVtbl -> get_PortName(This,pVal)

#define ISMIComm_GetResponseOf(This,Command,Response)	\
    (This)->lpVtbl -> GetResponseOf(This,Command,Response)

#define ISMIComm_GetMotor(This,Address,ppMotor)	\
    (This)->lpVtbl -> GetMotor(This,Address,ppMotor)

#define ISMIComm_get_MaxMotors(This,pVal)	\
    (This)->lpVtbl -> get_MaxMotors(This,pVal)

#define ISMIComm_put_MaxMotors(This,newVal)	\
    (This)->lpVtbl -> put_MaxMotors(This,newVal)

#define ISMIComm_Download(This,Motor,FileName)	\
    (This)->lpVtbl -> Download(This,Motor,FileName)

#define ISMIComm_Upload(This,Motor,FileName)	\
    (This)->lpVtbl -> Upload(This,Motor,FileName)

#define ISMIComm_ForceUpload(This,Motor,FileName)	\
    (This)->lpVtbl -> ForceUpload(This,Motor,FileName)

#define ISMIComm_get_IsRS485(This,pVal)	\
    (This)->lpVtbl -> get_IsRS485(This,pVal)

#define ISMIComm_put_IsRS485(This,newVal)	\
    (This)->lpVtbl -> put_IsRS485(This,newVal)

#define ISMIComm_ClearEEPROM(This,Motor)	\
    (This)->lpVtbl -> ClearEEPROM(This,Motor)

#define ISMIComm_InitializeNotification(This,hWindow,message)	\
    (This)->lpVtbl -> InitializeNotification(This,hWindow,message)

#define ISMIComm_WriteCmd(This,Command)	\
    (This)->lpVtbl -> WriteCmd(This,Command)

#define ISMIComm_InitRS485(This,NoOfMotors,MotorChannel)	\
    (This)->lpVtbl -> InitRS485(This,NoOfMotors,MotorChannel)

#define ISMIComm_get_TxMaxRetry(This,pVal)	\
    (This)->lpVtbl -> get_TxMaxRetry(This,pVal)

#define ISMIComm_put_TxMaxRetry(This,newVal)	\
    (This)->lpVtbl -> put_TxMaxRetry(This,newVal)

#define ISMIComm_get_TxTimeoutMul(This,pVal)	\
    (This)->lpVtbl -> get_TxTimeoutMul(This,pVal)

#define ISMIComm_put_TxTimeoutMul(This,newVal)	\
    (This)->lpVtbl -> put_TxTimeoutMul(This,newVal)

#define ISMIComm_get_TxTimeoutConst(This,pVal)	\
    (This)->lpVtbl -> get_TxTimeoutConst(This,pVal)

#define ISMIComm_put_TxTimeoutConst(This,newVal)	\
    (This)->lpVtbl -> put_TxTimeoutConst(This,newVal)

#define ISMIComm_get_LogFlags(This,pVal)	\
    (This)->lpVtbl -> get_LogFlags(This,pVal)

#define ISMIComm_put_LogFlags(This,newVal)	\
    (This)->lpVtbl -> put_LogFlags(This,newVal)

#define ISMIComm_get_LogFileName(This,pVal)	\
    (This)->lpVtbl -> get_LogFileName(This,pVal)

#define ISMIComm_put_LogFileName(This,newVal)	\
    (This)->lpVtbl -> put_LogFileName(This,newVal)

#define ISMIComm_get_EchoTimeoutMul(This,pVal)	\
    (This)->lpVtbl -> get_EchoTimeoutMul(This,pVal)

#define ISMIComm_put_EchoTimeoutMul(This,newVal)	\
    (This)->lpVtbl -> put_EchoTimeoutMul(This,newVal)

#define ISMIComm_get_EchoTimeoutConst(This,pVal)	\
    (This)->lpVtbl -> get_EchoTimeoutConst(This,pVal)

#define ISMIComm_put_EchoTimeoutConst(This,newVal)	\
    (This)->lpVtbl -> put_EchoTimeoutConst(This,newVal)

#define ISMIComm_DetectRS232(This,MaxAddress,Flags,ErrorCode)	\
    (This)->lpVtbl -> DetectRS232(This,MaxAddress,Flags,ErrorCode)

#define ISMIComm_DetectRS485(This,MaxAddress,MotorChannel,Flags,ErrorCode)	\
    (This)->lpVtbl -> DetectRS485(This,MaxAddress,MotorChannel,Flags,ErrorCode)

#define ISMIComm_AddressServos(This,Flags,ErrorCode)	\
    (This)->lpVtbl -> AddressServos(This,Flags,ErrorCode)

#define ISMIComm_get_NoOfMotors(This,pVal)	\
    (This)->lpVtbl -> get_NoOfMotors(This,pVal)

#define ISMIComm_get_EngineVersion(This,pVal)	\
    (This)->lpVtbl -> get_EngineVersion(This,pVal)

#define ISMIComm_InitEthernet(This,MaxAddress)	\
    (This)->lpVtbl -> InitEthernet(This,MaxAddress)

#define ISMIComm_DetectNetMotors(This,MaxAddress,Flags,ErrorCode)	\
    (This)->lpVtbl -> DetectNetMotors(This,MaxAddress,Flags,ErrorCode)

#define ISMIComm_AddNetMotor(This,IPAddress,MotorAddress)	\
    (This)->lpVtbl -> AddNetMotor(This,IPAddress,MotorAddress)

#define ISMIComm_InitUSB(This,MaxAddress)	\
    (This)->lpVtbl -> InitUSB(This,MaxAddress)

#define ISMIComm_DetectUSBMotors(This,MaxAddress,Flags,ErrorCode)	\
    (This)->lpVtbl -> DetectUSBMotors(This,MaxAddress,Flags,ErrorCode)

#define ISMIComm_ReorderMotors(This,Address,SerialNo,OldMotorAddress)	\
    (This)->lpVtbl -> ReorderMotors(This,Address,SerialNo,OldMotorAddress)

#define ISMIComm_InitCANOpen(This,MaxAddress)	\
    (This)->lpVtbl -> InitCANOpen(This,MaxAddress)

#define ISMIComm_DetectCANOpenMotors(This,MaxAddress,Flags,ErrorCode)	\
    (This)->lpVtbl -> DetectCANOpenMotors(This,MaxAddress,Flags,ErrorCode)

#define ISMIComm_ReadCharacters(This,NoOfBytes,pData)	\
    (This)->lpVtbl -> ReadCharacters(This,NoOfBytes,pData)

#define ISMIComm_get_DelayEchoOff(This,pVal)	\
    (This)->lpVtbl -> get_DelayEchoOff(This,pVal)

#define ISMIComm_put_DelayEchoOff(This,newVal)	\
    (This)->lpVtbl -> put_DelayEchoOff(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_OpenPort_Proxy( 
    ISMIComm * This,
    BSTR PortName);


void __RPC_STUB ISMIComm_OpenPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ClosePort_Proxy( 
    ISMIComm * This);


void __RPC_STUB ISMIComm_ClosePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_PortHandle_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long **pVal);


void __RPC_STUB ISMIComm_get_PortHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ClearBuffer_Proxy( 
    ISMIComm * This);


void __RPC_STUB ISMIComm_ClearBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_BytesAvailable_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_BytesAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_WriteCommand_Proxy( 
    ISMIComm * This,
    /* [string][in] */ BSTR Command);


void __RPC_STUB ISMIComm_WriteCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_WriteString_Proxy( 
    ISMIComm * This,
    /* [string][in] */ BSTR Data);


void __RPC_STUB ISMIComm_WriteString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ReadResponse_Proxy( 
    ISMIComm * This,
    /* [string][retval][out] */ BSTR *Response);


void __RPC_STUB ISMIComm_ReadResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ReadString_Proxy( 
    ISMIComm * This,
    /* [in] */ long NoOfBytes,
    /* [string][retval][out] */ BSTR *Data);


void __RPC_STUB ISMIComm_ReadString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_EstablishChain_Proxy( 
    ISMIComm * This);


void __RPC_STUB ISMIComm_EstablishChain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_AddressMotorChain_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *MotorCount);


void __RPC_STUB ISMIComm_AddressMotorChain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_Wait_Proxy( 
    ISMIComm * This,
    /* [in] */ long Time);


void __RPC_STUB ISMIComm_Wait_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_DefaultMotor_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_DefaultMotor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_DefaultMotor_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_DefaultMotor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_Echo_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIComm_get_Echo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_Echo_Proxy( 
    ISMIComm * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMIComm_put_Echo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_Parity_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ enum_parity *pVal);


void __RPC_STUB ISMIComm_get_Parity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_Parity_Proxy( 
    ISMIComm * This,
    /* [in] */ enum_parity newVal);


void __RPC_STUB ISMIComm_put_Parity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_BaudRate_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_BaudRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_BaudRate_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_BaudRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_Timeout_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_Timeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_Timeout_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_Timeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_CharDelay_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_CharDelay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_CharDelay_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_CharDelay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_PortName_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISMIComm_get_PortName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_GetResponseOf_Proxy( 
    ISMIComm * This,
    /* [string][in] */ BSTR Command,
    /* [string][retval][out] */ BSTR *Response);


void __RPC_STUB ISMIComm_GetResponseOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_GetMotor_Proxy( 
    ISMIComm * This,
    /* [in] */ long Address,
    /* [retval][out] */ ISMIMotor **ppMotor);


void __RPC_STUB ISMIComm_GetMotor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_MaxMotors_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_MaxMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_MaxMotors_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_MaxMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_Download_Proxy( 
    ISMIComm * This,
    /* [in] */ long Motor,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIComm_Download_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_Upload_Proxy( 
    ISMIComm * This,
    /* [in] */ long Motor,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIComm_Upload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ForceUpload_Proxy( 
    ISMIComm * This,
    /* [in] */ long Motor,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIComm_ForceUpload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_IsRS485_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIComm_get_IsRS485_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_IsRS485_Proxy( 
    ISMIComm * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMIComm_put_IsRS485_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ClearEEPROM_Proxy( 
    ISMIComm * This,
    /* [in] */ long Motor);


void __RPC_STUB ISMIComm_ClearEEPROM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_InitializeNotification_Proxy( 
    ISMIComm * This,
    /* [in] */ long hWindow,
    /* [in] */ long message);


void __RPC_STUB ISMIComm_InitializeNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_WriteCmd_Proxy( 
    ISMIComm * This,
    /* [string][in] */ BSTR Command);


void __RPC_STUB ISMIComm_WriteCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_InitRS485_Proxy( 
    ISMIComm * This,
    /* [in] */ long NoOfMotors,
    /* [in] */ long MotorChannel);


void __RPC_STUB ISMIComm_InitRS485_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_TxMaxRetry_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_TxMaxRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_TxMaxRetry_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_TxMaxRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_TxTimeoutMul_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_TxTimeoutMul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_TxTimeoutMul_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_TxTimeoutMul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_TxTimeoutConst_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_TxTimeoutConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_TxTimeoutConst_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_TxTimeoutConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_LogFlags_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_LogFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_LogFlags_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_LogFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_LogFileName_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISMIComm_get_LogFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_LogFileName_Proxy( 
    ISMIComm * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISMIComm_put_LogFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_EchoTimeoutMul_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_EchoTimeoutMul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_EchoTimeoutMul_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_EchoTimeoutMul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_EchoTimeoutConst_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_EchoTimeoutConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_EchoTimeoutConst_Proxy( 
    ISMIComm * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIComm_put_EchoTimeoutConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_DetectRS232_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_DetectRS232_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_DetectRS485_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress,
    /* [in] */ long MotorChannel,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_DetectRS485_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_AddressServos_Proxy( 
    ISMIComm * This,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_AddressServos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_NoOfMotors_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_NoOfMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_EngineVersion_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIComm_get_EngineVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_InitEthernet_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress);


void __RPC_STUB ISMIComm_InitEthernet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_DetectNetMotors_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_DetectNetMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_AddNetMotor_Proxy( 
    ISMIComm * This,
    /* [string][in] */ BSTR IPAddress,
    /* [retval][out] */ long *MotorAddress);


void __RPC_STUB ISMIComm_AddNetMotor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_InitUSB_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress);


void __RPC_STUB ISMIComm_InitUSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_DetectUSBMotors_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_DetectUSBMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ReorderMotors_Proxy( 
    ISMIComm * This,
    /* [in] */ long Address,
    /* [string][in] */ BSTR SerialNo,
    /* [retval][out] */ long *OldMotorAddress);


void __RPC_STUB ISMIComm_ReorderMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_InitCANOpen_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress);


void __RPC_STUB ISMIComm_InitCANOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_DetectCANOpenMotors_Proxy( 
    ISMIComm * This,
    /* [in] */ long MaxAddress,
    /* [in] */ long Flags,
    /* [retval][out] */ long *ErrorCode);


void __RPC_STUB ISMIComm_DetectCANOpenMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIComm_ReadCharacters_Proxy( 
    ISMIComm * This,
    /* [in] */ LONG NoOfBytes,
    /* [retval][out] */ BSTR *pData);


void __RPC_STUB ISMIComm_ReadCharacters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIComm_get_DelayEchoOff_Proxy( 
    ISMIComm * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMIComm_get_DelayEchoOff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIComm_put_DelayEchoOff_Proxy( 
    ISMIComm * This,
    /* [in] */ LONG newVal);


void __RPC_STUB ISMIComm_put_DelayEchoOff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMIComm_INTERFACE_DEFINED__ */


#ifndef __ISMICMotion_INTERFACE_DEFINED__
#define __ISMICMotion_INTERFACE_DEFINED__

/* interface ISMICMotion */
/* [helpcontext][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMICMotion;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BC26FBB9-00E1-11D4-B5D0-00600831E1F3")
    ISMICMotion : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ long NoOfMotors) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WritePosition( 
            /* [in] */ long Motor,
            /* [in] */ long Pos) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteClock( 
            /* [in] */ long Motor,
            /* [in] */ long Clock) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SlotsNeeded( 
            /* [in] */ long Motor,
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentClock( 
            /* [in] */ long Motor,
            /* [retval][out] */ ULONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckStatus( 
            /* [in] */ long Motor,
            /* [retval][out] */ long *FreeSlots) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TrajectorySlots( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TrajectorySlots( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Buffered( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Buffered( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SampleRate( 
            /* [in] */ long nMotor,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PIDFactor( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PIDFactor( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndHostMode( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Run( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SyncMotors( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MotorSlots( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PauseHostMode( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AtPausePoint( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteUserBits( 
            /* [in] */ long Motor,
            /* [in] */ long UserBits) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HostMotorAddress( 
            /* [in] */ LONG Motor_Index,
            /* [in] */ LONG Motor_Address) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetHostType( 
            /* [in] */ LONG TypeCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndHostModeStep( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WritePositionAndBits( 
            /* [in] */ LONG Motor,
            /* [in] */ LONG Pos,
            /* [in] */ LONG UserBits) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteSegmentTime( 
            /* [in] */ LONG Motor,
            /* [in] */ LONG SegmentTime) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SyncMotorClocks( 
            /* [in] */ LONG Motor,
            /* [in] */ ULONG MasterClock) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadMasterClock( 
            /* [in] */ LONG Motor,
            /* [retval][out] */ ULONG *pMasterClock) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteHostConfig( 
            /* [in] */ LONG Motor,
            /* [in] */ LONG HostConfig) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DisablePDOs( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableFollowPDOs( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopOnOverflow( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopOnOverflow( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopOnUnderflow( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopOnUnderflow( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TrapSlaveClock( 
            /* [in] */ LONG Motor) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsRunning( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMICMotionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMICMotion * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMICMotion * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMICMotion * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMICMotion * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMICMotion * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMICMotion * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMICMotion * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            ISMICMotion * This,
            /* [in] */ long NoOfMotors);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WritePosition )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [in] */ long Pos);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteClock )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [in] */ long Clock);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SlotsNeeded )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentClock )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [retval][out] */ ULONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckStatus )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [retval][out] */ long *FreeSlots);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TrajectorySlots )( 
            ISMICMotion * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TrajectorySlots )( 
            ISMICMotion * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Buffered )( 
            ISMICMotion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Buffered )( 
            ISMICMotion * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SampleRate )( 
            ISMICMotion * This,
            /* [in] */ long nMotor,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PIDFactor )( 
            ISMICMotion * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PIDFactor )( 
            ISMICMotion * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndHostMode )( 
            ISMICMotion * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Run )( 
            ISMICMotion * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SyncMotors )( 
            ISMICMotion * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MotorSlots )( 
            ISMICMotion * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PauseHostMode )( 
            ISMICMotion * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AtPausePoint )( 
            ISMICMotion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteUserBits )( 
            ISMICMotion * This,
            /* [in] */ long Motor,
            /* [in] */ long UserBits);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *HostMotorAddress )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor_Index,
            /* [in] */ LONG Motor_Address);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetHostType )( 
            ISMICMotion * This,
            /* [in] */ LONG TypeCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndHostModeStep )( 
            ISMICMotion * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WritePositionAndBits )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor,
            /* [in] */ LONG Pos,
            /* [in] */ LONG UserBits);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteSegmentTime )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor,
            /* [in] */ LONG SegmentTime);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SyncMotorClocks )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor,
            /* [in] */ ULONG MasterClock);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadMasterClock )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor,
            /* [retval][out] */ ULONG *pMasterClock);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteHostConfig )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor,
            /* [in] */ LONG HostConfig);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DisablePDOs )( 
            ISMICMotion * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableFollowPDOs )( 
            ISMICMotion * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StopOnOverflow )( 
            ISMICMotion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StopOnOverflow )( 
            ISMICMotion * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StopOnUnderflow )( 
            ISMICMotion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StopOnUnderflow )( 
            ISMICMotion * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TrapSlaveClock )( 
            ISMICMotion * This,
            /* [in] */ LONG Motor);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsRunning )( 
            ISMICMotion * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        END_INTERFACE
    } ISMICMotionVtbl;

    interface ISMICMotion
    {
        CONST_VTBL struct ISMICMotionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMICMotion_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMICMotion_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMICMotion_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMICMotion_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMICMotion_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMICMotion_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMICMotion_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMICMotion_Initialize(This,NoOfMotors)	\
    (This)->lpVtbl -> Initialize(This,NoOfMotors)

#define ISMICMotion_WritePosition(This,Motor,Pos)	\
    (This)->lpVtbl -> WritePosition(This,Motor,Pos)

#define ISMICMotion_WriteClock(This,Motor,Clock)	\
    (This)->lpVtbl -> WriteClock(This,Motor,Clock)

#define ISMICMotion_get_SlotsNeeded(This,Motor,pVal)	\
    (This)->lpVtbl -> get_SlotsNeeded(This,Motor,pVal)

#define ISMICMotion_get_CurrentClock(This,Motor,pVal)	\
    (This)->lpVtbl -> get_CurrentClock(This,Motor,pVal)

#define ISMICMotion_CheckStatus(This,Motor,FreeSlots)	\
    (This)->lpVtbl -> CheckStatus(This,Motor,FreeSlots)

#define ISMICMotion_get_TrajectorySlots(This,pVal)	\
    (This)->lpVtbl -> get_TrajectorySlots(This,pVal)

#define ISMICMotion_put_TrajectorySlots(This,newVal)	\
    (This)->lpVtbl -> put_TrajectorySlots(This,newVal)

#define ISMICMotion_get_Buffered(This,pVal)	\
    (This)->lpVtbl -> get_Buffered(This,pVal)

#define ISMICMotion_put_Buffered(This,newVal)	\
    (This)->lpVtbl -> put_Buffered(This,newVal)

#define ISMICMotion_get_SampleRate(This,nMotor,pVal)	\
    (This)->lpVtbl -> get_SampleRate(This,nMotor,pVal)

#define ISMICMotion_get_PIDFactor(This,pVal)	\
    (This)->lpVtbl -> get_PIDFactor(This,pVal)

#define ISMICMotion_put_PIDFactor(This,newVal)	\
    (This)->lpVtbl -> put_PIDFactor(This,newVal)

#define ISMICMotion_EndHostMode(This)	\
    (This)->lpVtbl -> EndHostMode(This)

#define ISMICMotion_Run(This)	\
    (This)->lpVtbl -> Run(This)

#define ISMICMotion_SyncMotors(This)	\
    (This)->lpVtbl -> SyncMotors(This)

#define ISMICMotion_get_MotorSlots(This,pVal)	\
    (This)->lpVtbl -> get_MotorSlots(This,pVal)

#define ISMICMotion_PauseHostMode(This)	\
    (This)->lpVtbl -> PauseHostMode(This)

#define ISMICMotion_get_AtPausePoint(This,pVal)	\
    (This)->lpVtbl -> get_AtPausePoint(This,pVal)

#define ISMICMotion_WriteUserBits(This,Motor,UserBits)	\
    (This)->lpVtbl -> WriteUserBits(This,Motor,UserBits)

#define ISMICMotion_HostMotorAddress(This,Motor_Index,Motor_Address)	\
    (This)->lpVtbl -> HostMotorAddress(This,Motor_Index,Motor_Address)

#define ISMICMotion_SetHostType(This,TypeCode)	\
    (This)->lpVtbl -> SetHostType(This,TypeCode)

#define ISMICMotion_EndHostModeStep(This)	\
    (This)->lpVtbl -> EndHostModeStep(This)

#define ISMICMotion_WritePositionAndBits(This,Motor,Pos,UserBits)	\
    (This)->lpVtbl -> WritePositionAndBits(This,Motor,Pos,UserBits)

#define ISMICMotion_WriteSegmentTime(This,Motor,SegmentTime)	\
    (This)->lpVtbl -> WriteSegmentTime(This,Motor,SegmentTime)

#define ISMICMotion_SyncMotorClocks(This,Motor,MasterClock)	\
    (This)->lpVtbl -> SyncMotorClocks(This,Motor,MasterClock)

#define ISMICMotion_ReadMasterClock(This,Motor,pMasterClock)	\
    (This)->lpVtbl -> ReadMasterClock(This,Motor,pMasterClock)

#define ISMICMotion_WriteHostConfig(This,Motor,HostConfig)	\
    (This)->lpVtbl -> WriteHostConfig(This,Motor,HostConfig)

#define ISMICMotion_DisablePDOs(This)	\
    (This)->lpVtbl -> DisablePDOs(This)

#define ISMICMotion_EnableFollowPDOs(This)	\
    (This)->lpVtbl -> EnableFollowPDOs(This)

#define ISMICMotion_get_StopOnOverflow(This,pVal)	\
    (This)->lpVtbl -> get_StopOnOverflow(This,pVal)

#define ISMICMotion_put_StopOnOverflow(This,newVal)	\
    (This)->lpVtbl -> put_StopOnOverflow(This,newVal)

#define ISMICMotion_get_StopOnUnderflow(This,pVal)	\
    (This)->lpVtbl -> get_StopOnUnderflow(This,pVal)

#define ISMICMotion_put_StopOnUnderflow(This,newVal)	\
    (This)->lpVtbl -> put_StopOnUnderflow(This,newVal)

#define ISMICMotion_TrapSlaveClock(This,Motor)	\
    (This)->lpVtbl -> TrapSlaveClock(This,Motor)

#define ISMICMotion_get_IsRunning(This,pVal)	\
    (This)->lpVtbl -> get_IsRunning(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_Initialize_Proxy( 
    ISMICMotion * This,
    /* [in] */ long NoOfMotors);


void __RPC_STUB ISMICMotion_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WritePosition_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [in] */ long Pos);


void __RPC_STUB ISMICMotion_WritePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WriteClock_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [in] */ long Clock);


void __RPC_STUB ISMICMotion_WriteClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_SlotsNeeded_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMICMotion_get_SlotsNeeded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_CurrentClock_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [retval][out] */ ULONG *pVal);


void __RPC_STUB ISMICMotion_get_CurrentClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_CheckStatus_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [retval][out] */ long *FreeSlots);


void __RPC_STUB ISMICMotion_CheckStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_TrajectorySlots_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMICMotion_get_TrajectorySlots_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMICMotion_put_TrajectorySlots_Proxy( 
    ISMICMotion * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMICMotion_put_TrajectorySlots_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_Buffered_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMICMotion_get_Buffered_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMICMotion_put_Buffered_Proxy( 
    ISMICMotion * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMICMotion_put_Buffered_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_SampleRate_Proxy( 
    ISMICMotion * This,
    /* [in] */ long nMotor,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMICMotion_get_SampleRate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_PIDFactor_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMICMotion_get_PIDFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMICMotion_put_PIDFactor_Proxy( 
    ISMICMotion * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMICMotion_put_PIDFactor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_EndHostMode_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_EndHostMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_Run_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_Run_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_SyncMotors_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_SyncMotors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_MotorSlots_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB ISMICMotion_get_MotorSlots_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_PauseHostMode_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_PauseHostMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_AtPausePoint_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMICMotion_get_AtPausePoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WriteUserBits_Proxy( 
    ISMICMotion * This,
    /* [in] */ long Motor,
    /* [in] */ long UserBits);


void __RPC_STUB ISMICMotion_WriteUserBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_HostMotorAddress_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor_Index,
    /* [in] */ LONG Motor_Address);


void __RPC_STUB ISMICMotion_HostMotorAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_SetHostType_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG TypeCode);


void __RPC_STUB ISMICMotion_SetHostType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_EndHostModeStep_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_EndHostModeStep_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WritePositionAndBits_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor,
    /* [in] */ LONG Pos,
    /* [in] */ LONG UserBits);


void __RPC_STUB ISMICMotion_WritePositionAndBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WriteSegmentTime_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor,
    /* [in] */ LONG SegmentTime);


void __RPC_STUB ISMICMotion_WriteSegmentTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_SyncMotorClocks_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor,
    /* [in] */ ULONG MasterClock);


void __RPC_STUB ISMICMotion_SyncMotorClocks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_ReadMasterClock_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor,
    /* [retval][out] */ ULONG *pMasterClock);


void __RPC_STUB ISMICMotion_ReadMasterClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_WriteHostConfig_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor,
    /* [in] */ LONG HostConfig);


void __RPC_STUB ISMICMotion_WriteHostConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_DisablePDOs_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_DisablePDOs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_EnableFollowPDOs_Proxy( 
    ISMICMotion * This);


void __RPC_STUB ISMICMotion_EnableFollowPDOs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_StopOnOverflow_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMICMotion_get_StopOnOverflow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMICMotion_put_StopOnOverflow_Proxy( 
    ISMICMotion * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMICMotion_put_StopOnOverflow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_StopOnUnderflow_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMICMotion_get_StopOnUnderflow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMICMotion_put_StopOnUnderflow_Proxy( 
    ISMICMotion * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMICMotion_put_StopOnUnderflow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMICMotion_TrapSlaveClock_Proxy( 
    ISMICMotion * This,
    /* [in] */ LONG Motor);


void __RPC_STUB ISMICMotion_TrapSlaveClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMICMotion_get_IsRunning_Proxy( 
    ISMICMotion * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMICMotion_get_IsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMICMotion_INTERFACE_DEFINED__ */


#ifndef __ISMIPath_INTERFACE_DEFINED__
#define __ISMIPath_INTERFACE_DEFINED__

/* interface ISMIPath */
/* [helpcontext][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMIPath;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B998E60D-DB45-4b64-A149-CC7BD21311DE")
    ISMIPath : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndSpeed( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndSpeed( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Acceleration( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Acceleration( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartPoint( 
            /* [in] */ int Axis,
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartPoint( 
            /* [in] */ int Axis,
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartClock( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartClock( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SlowToEndSpeed( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SlowToEndSpeed( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetupTimeDelta( 
            /* [in] */ long AveTimeDelta,
            /* [defaultvalue][in] */ long MinTimeDelta = -1,
            /* [defaultvalue][in] */ long AllowedCount = -1,
            /* [defaultvalue][in] */ long PrecedeCount = -1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GeneratePath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ double TotalLength,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateLinePath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateArcPath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [in] */ VARIANT Center,
            /* [in] */ enum_arcdirection ArcDirection,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartSpeed( 
            /* [retval][out] */ double *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartSpeed( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeDelta( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeDelta( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Plane( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Plane( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreatePath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ double TotalLength,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateLinePath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateArcPath( 
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [in] */ VARIANT Center,
            /* [in] */ enum_arcdirection ArcDirection,
            /* [retval][out] */ VARIANT *DataPairs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMIPathVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMIPath * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMIPath * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMIPath * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMIPath * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMIPath * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMIPath * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMIPath * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EndSpeed )( 
            ISMIPath * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EndSpeed )( 
            ISMIPath * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Acceleration )( 
            ISMIPath * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Acceleration )( 
            ISMIPath * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartPoint )( 
            ISMIPath * This,
            /* [in] */ int Axis,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartPoint )( 
            ISMIPath * This,
            /* [in] */ int Axis,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartClock )( 
            ISMIPath * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartClock )( 
            ISMIPath * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SlowToEndSpeed )( 
            ISMIPath * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SlowToEndSpeed )( 
            ISMIPath * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetupTimeDelta )( 
            ISMIPath * This,
            /* [in] */ long AveTimeDelta,
            /* [defaultvalue][in] */ long MinTimeDelta,
            /* [defaultvalue][in] */ long AllowedCount,
            /* [defaultvalue][in] */ long PrecedeCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GeneratePath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ double TotalLength,
            /* [retval][out] */ VARIANT *DataPairs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateLinePath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [retval][out] */ VARIANT *DataPairs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GenerateArcPath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [in] */ VARIANT Center,
            /* [in] */ enum_arcdirection ArcDirection,
            /* [retval][out] */ VARIANT *DataPairs);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StartSpeed )( 
            ISMIPath * This,
            /* [retval][out] */ double *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StartSpeed )( 
            ISMIPath * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TimeDelta )( 
            ISMIPath * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TimeDelta )( 
            ISMIPath * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Plane )( 
            ISMIPath * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Plane )( 
            ISMIPath * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreatePath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ double TotalLength,
            /* [retval][out] */ VARIANT *DataPairs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateLinePath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [retval][out] */ VARIANT *DataPairs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateArcPath )( 
            ISMIPath * This,
            /* [in] */ double MaxSpeed,
            /* [in] */ VARIANT Endpoint,
            /* [in] */ VARIANT Center,
            /* [in] */ enum_arcdirection ArcDirection,
            /* [retval][out] */ VARIANT *DataPairs);
        
        END_INTERFACE
    } ISMIPathVtbl;

    interface ISMIPath
    {
        CONST_VTBL struct ISMIPathVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMIPath_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMIPath_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMIPath_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMIPath_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMIPath_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMIPath_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMIPath_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMIPath_get_EndSpeed(This,pVal)	\
    (This)->lpVtbl -> get_EndSpeed(This,pVal)

#define ISMIPath_put_EndSpeed(This,newVal)	\
    (This)->lpVtbl -> put_EndSpeed(This,newVal)

#define ISMIPath_get_Acceleration(This,pVal)	\
    (This)->lpVtbl -> get_Acceleration(This,pVal)

#define ISMIPath_put_Acceleration(This,newVal)	\
    (This)->lpVtbl -> put_Acceleration(This,newVal)

#define ISMIPath_get_StartPoint(This,Axis,pVal)	\
    (This)->lpVtbl -> get_StartPoint(This,Axis,pVal)

#define ISMIPath_put_StartPoint(This,Axis,newVal)	\
    (This)->lpVtbl -> put_StartPoint(This,Axis,newVal)

#define ISMIPath_get_StartClock(This,pVal)	\
    (This)->lpVtbl -> get_StartClock(This,pVal)

#define ISMIPath_put_StartClock(This,newVal)	\
    (This)->lpVtbl -> put_StartClock(This,newVal)

#define ISMIPath_get_SlowToEndSpeed(This,pVal)	\
    (This)->lpVtbl -> get_SlowToEndSpeed(This,pVal)

#define ISMIPath_put_SlowToEndSpeed(This,newVal)	\
    (This)->lpVtbl -> put_SlowToEndSpeed(This,newVal)

#define ISMIPath_SetupTimeDelta(This,AveTimeDelta,MinTimeDelta,AllowedCount,PrecedeCount)	\
    (This)->lpVtbl -> SetupTimeDelta(This,AveTimeDelta,MinTimeDelta,AllowedCount,PrecedeCount)

#define ISMIPath_GeneratePath(This,MaxSpeed,TotalLength,DataPairs)	\
    (This)->lpVtbl -> GeneratePath(This,MaxSpeed,TotalLength,DataPairs)

#define ISMIPath_GenerateLinePath(This,MaxSpeed,Endpoint,DataPairs)	\
    (This)->lpVtbl -> GenerateLinePath(This,MaxSpeed,Endpoint,DataPairs)

#define ISMIPath_GenerateArcPath(This,MaxSpeed,Endpoint,Center,ArcDirection,DataPairs)	\
    (This)->lpVtbl -> GenerateArcPath(This,MaxSpeed,Endpoint,Center,ArcDirection,DataPairs)

#define ISMIPath_get_StartSpeed(This,pVal)	\
    (This)->lpVtbl -> get_StartSpeed(This,pVal)

#define ISMIPath_put_StartSpeed(This,newVal)	\
    (This)->lpVtbl -> put_StartSpeed(This,newVal)

#define ISMIPath_get_TimeDelta(This,pVal)	\
    (This)->lpVtbl -> get_TimeDelta(This,pVal)

#define ISMIPath_put_TimeDelta(This,newVal)	\
    (This)->lpVtbl -> put_TimeDelta(This,newVal)

#define ISMIPath_get_Plane(This,pVal)	\
    (This)->lpVtbl -> get_Plane(This,pVal)

#define ISMIPath_put_Plane(This,newVal)	\
    (This)->lpVtbl -> put_Plane(This,newVal)

#define ISMIPath_CreatePath(This,MaxSpeed,TotalLength,DataPairs)	\
    (This)->lpVtbl -> CreatePath(This,MaxSpeed,TotalLength,DataPairs)

#define ISMIPath_CreateLinePath(This,MaxSpeed,Endpoint,DataPairs)	\
    (This)->lpVtbl -> CreateLinePath(This,MaxSpeed,Endpoint,DataPairs)

#define ISMIPath_CreateArcPath(This,MaxSpeed,Endpoint,Center,ArcDirection,DataPairs)	\
    (This)->lpVtbl -> CreateArcPath(This,MaxSpeed,Endpoint,Center,ArcDirection,DataPairs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_EndSpeed_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMIPath_get_EndSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_EndSpeed_Proxy( 
    ISMIPath * This,
    /* [in] */ double newVal);


void __RPC_STUB ISMIPath_put_EndSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_Acceleration_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMIPath_get_Acceleration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_Acceleration_Proxy( 
    ISMIPath * This,
    /* [in] */ double newVal);


void __RPC_STUB ISMIPath_put_Acceleration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_StartPoint_Proxy( 
    ISMIPath * This,
    /* [in] */ int Axis,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMIPath_get_StartPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_StartPoint_Proxy( 
    ISMIPath * This,
    /* [in] */ int Axis,
    /* [in] */ double newVal);


void __RPC_STUB ISMIPath_put_StartPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_StartClock_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIPath_get_StartClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_StartClock_Proxy( 
    ISMIPath * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIPath_put_StartClock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_SlowToEndSpeed_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIPath_get_SlowToEndSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_SlowToEndSpeed_Proxy( 
    ISMIPath * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMIPath_put_SlowToEndSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_SetupTimeDelta_Proxy( 
    ISMIPath * This,
    /* [in] */ long AveTimeDelta,
    /* [defaultvalue][in] */ long MinTimeDelta,
    /* [defaultvalue][in] */ long AllowedCount,
    /* [defaultvalue][in] */ long PrecedeCount);


void __RPC_STUB ISMIPath_SetupTimeDelta_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_GeneratePath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ double TotalLength,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_GeneratePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_GenerateLinePath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ VARIANT Endpoint,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_GenerateLinePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_GenerateArcPath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ VARIANT Endpoint,
    /* [in] */ VARIANT Center,
    /* [in] */ enum_arcdirection ArcDirection,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_GenerateArcPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_StartSpeed_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ double *pVal);


void __RPC_STUB ISMIPath_get_StartSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_StartSpeed_Proxy( 
    ISMIPath * This,
    /* [in] */ double newVal);


void __RPC_STUB ISMIPath_put_StartSpeed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_TimeDelta_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIPath_get_TimeDelta_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_TimeDelta_Proxy( 
    ISMIPath * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIPath_put_TimeDelta_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIPath_get_Plane_Proxy( 
    ISMIPath * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIPath_get_Plane_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIPath_put_Plane_Proxy( 
    ISMIPath * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIPath_put_Plane_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_CreatePath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ double TotalLength,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_CreatePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_CreateLinePath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ VARIANT Endpoint,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_CreateLinePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIPath_CreateArcPath_Proxy( 
    ISMIPath * This,
    /* [in] */ double MaxSpeed,
    /* [in] */ VARIANT Endpoint,
    /* [in] */ VARIANT Center,
    /* [in] */ enum_arcdirection ArcDirection,
    /* [retval][out] */ VARIANT *DataPairs);


void __RPC_STUB ISMIPath_CreateArcPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMIPath_INTERFACE_DEFINED__ */


#ifndef __ISMIScan_INTERFACE_DEFINED__
#define __ISMIScan_INTERFACE_DEFINED__

/* interface ISMIScan */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMIScan;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("124D89B8-26B4-42A9-B988-C2A38D57CAED")
    ISMIScan : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Scan( 
            /* [in] */ BSTR Program) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScanFile( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MakeSMX( 
            /* [in] */ BSTR FileName,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MakeSMX2( 
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR SMXFileName,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LanguageFile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LanguageFile( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Errors2Warnings( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Errors2Warnings( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMIScanVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMIScan * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMIScan * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMIScan * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMIScan * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMIScan * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMIScan * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMIScan * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Scan )( 
            ISMIScan * This,
            /* [in] */ BSTR Program);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ScanFile )( 
            ISMIScan * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MakeSMX )( 
            ISMIScan * This,
            /* [in] */ BSTR FileName,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MakeSMX2 )( 
            ISMIScan * This,
            /* [in] */ BSTR FileName,
            /* [in] */ BSTR SMXFileName,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LanguageFile )( 
            ISMIScan * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LanguageFile )( 
            ISMIScan * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Errors2Warnings )( 
            ISMIScan * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Errors2Warnings )( 
            ISMIScan * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } ISMIScanVtbl;

    interface ISMIScan
    {
        CONST_VTBL struct ISMIScanVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMIScan_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMIScan_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMIScan_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMIScan_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMIScan_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMIScan_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMIScan_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMIScan_Scan(This,Program)	\
    (This)->lpVtbl -> Scan(This,Program)

#define ISMIScan_ScanFile(This,FileName)	\
    (This)->lpVtbl -> ScanFile(This,FileName)

#define ISMIScan_MakeSMX(This,FileName,pVal)	\
    (This)->lpVtbl -> MakeSMX(This,FileName,pVal)

#define ISMIScan_MakeSMX2(This,FileName,SMXFileName,pVal)	\
    (This)->lpVtbl -> MakeSMX2(This,FileName,SMXFileName,pVal)

#define ISMIScan_get_LanguageFile(This,pVal)	\
    (This)->lpVtbl -> get_LanguageFile(This,pVal)

#define ISMIScan_put_LanguageFile(This,newVal)	\
    (This)->lpVtbl -> put_LanguageFile(This,newVal)

#define ISMIScan_get_Errors2Warnings(This,pVal)	\
    (This)->lpVtbl -> get_Errors2Warnings(This,pVal)

#define ISMIScan_put_Errors2Warnings(This,newVal)	\
    (This)->lpVtbl -> put_Errors2Warnings(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIScan_Scan_Proxy( 
    ISMIScan * This,
    /* [in] */ BSTR Program);


void __RPC_STUB ISMIScan_Scan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIScan_ScanFile_Proxy( 
    ISMIScan * This,
    /* [in] */ BSTR FileName);


void __RPC_STUB ISMIScan_ScanFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIScan_MakeSMX_Proxy( 
    ISMIScan * This,
    /* [in] */ BSTR FileName,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIScan_MakeSMX_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIScan_MakeSMX2_Proxy( 
    ISMIScan * This,
    /* [in] */ BSTR FileName,
    /* [in] */ BSTR SMXFileName,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIScan_MakeSMX2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIScan_get_LanguageFile_Proxy( 
    ISMIScan * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISMIScan_get_LanguageFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIScan_put_LanguageFile_Proxy( 
    ISMIScan * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB ISMIScan_put_LanguageFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIScan_get_Errors2Warnings_Proxy( 
    ISMIScan * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIScan_get_Errors2Warnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIScan_put_Errors2Warnings_Proxy( 
    ISMIScan * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB ISMIScan_put_Errors2Warnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMIScan_INTERFACE_DEFINED__ */


#ifndef __ISMIErrors_INTERFACE_DEFINED__
#define __ISMIErrors_INTERFACE_DEFINED__

/* interface ISMIErrors */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISMIErrors;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D1B5EE00-2230-44e3-878A-55263DA82E3D")
    ISMIErrors : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NoOfWarnings( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NoOfErrors( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxErrors( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxErrors( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxWarnings( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxWarnings( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GoFirst( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GoNext( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Code( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Line( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Column( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsWarning( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISMIErrorsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISMIErrors * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISMIErrors * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISMIErrors * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISMIErrors * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISMIErrors * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISMIErrors * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISMIErrors * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NoOfWarnings )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NoOfErrors )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaxErrors )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaxErrors )( 
            ISMIErrors * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_MaxWarnings )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_MaxWarnings )( 
            ISMIErrors * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoFirst )( 
            ISMIErrors * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoNext )( 
            ISMIErrors * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            ISMIErrors * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Code )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Line )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Column )( 
            ISMIErrors * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsWarning )( 
            ISMIErrors * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        END_INTERFACE
    } ISMIErrorsVtbl;

    interface ISMIErrors
    {
        CONST_VTBL struct ISMIErrorsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISMIErrors_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISMIErrors_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISMIErrors_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISMIErrors_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISMIErrors_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISMIErrors_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISMIErrors_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISMIErrors_get_NoOfWarnings(This,pVal)	\
    (This)->lpVtbl -> get_NoOfWarnings(This,pVal)

#define ISMIErrors_get_NoOfErrors(This,pVal)	\
    (This)->lpVtbl -> get_NoOfErrors(This,pVal)

#define ISMIErrors_get_MaxErrors(This,pVal)	\
    (This)->lpVtbl -> get_MaxErrors(This,pVal)

#define ISMIErrors_put_MaxErrors(This,newVal)	\
    (This)->lpVtbl -> put_MaxErrors(This,newVal)

#define ISMIErrors_get_MaxWarnings(This,pVal)	\
    (This)->lpVtbl -> get_MaxWarnings(This,pVal)

#define ISMIErrors_put_MaxWarnings(This,newVal)	\
    (This)->lpVtbl -> put_MaxWarnings(This,newVal)

#define ISMIErrors_GoFirst(This)	\
    (This)->lpVtbl -> GoFirst(This)

#define ISMIErrors_GoNext(This)	\
    (This)->lpVtbl -> GoNext(This)

#define ISMIErrors_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define ISMIErrors_get_Code(This,pVal)	\
    (This)->lpVtbl -> get_Code(This,pVal)

#define ISMIErrors_get_Line(This,pVal)	\
    (This)->lpVtbl -> get_Line(This,pVal)

#define ISMIErrors_get_Column(This,pVal)	\
    (This)->lpVtbl -> get_Column(This,pVal)

#define ISMIErrors_get_IsWarning(This,pVal)	\
    (This)->lpVtbl -> get_IsWarning(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_NoOfWarnings_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_NoOfWarnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_NoOfErrors_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_NoOfErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_MaxErrors_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_MaxErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIErrors_put_MaxErrors_Proxy( 
    ISMIErrors * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIErrors_put_MaxErrors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_MaxWarnings_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_MaxWarnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISMIErrors_put_MaxWarnings_Proxy( 
    ISMIErrors * This,
    /* [in] */ long newVal);


void __RPC_STUB ISMIErrors_put_MaxWarnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIErrors_GoFirst_Proxy( 
    ISMIErrors * This);


void __RPC_STUB ISMIErrors_GoFirst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISMIErrors_GoNext_Proxy( 
    ISMIErrors * This);


void __RPC_STUB ISMIErrors_GoNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_Description_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB ISMIErrors_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_Code_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_Code_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_Line_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_Line_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_Column_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB ISMIErrors_get_Column_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISMIErrors_get_IsWarning_Proxy( 
    ISMIErrors * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISMIErrors_get_IsWarning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISMIErrors_INTERFACE_DEFINED__ */



#ifndef __INTEGMOTORINTERFACELib_LIBRARY_DEFINED__
#define __INTEGMOTORINTERFACELib_LIBRARY_DEFINED__

/* library INTEGMOTORINTERFACELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_INTEGMOTORINTERFACELib;

EXTERN_C const CLSID CLSID_SMIHost;

#ifdef __cplusplus

class DECLSPEC_UUID("BC26FBBA-00E1-11D4-B5D0-00600831E1F3")
SMIHost;
#endif

EXTERN_C const CLSID CLSID_SMIMotor;

#ifdef __cplusplus

class DECLSPEC_UUID("D3A8EE8B-CAEF-40B6-8FD1-D43D498FE3A0")
SMIMotor;
#endif

EXTERN_C const CLSID CLSID_SMIScan;

#ifdef __cplusplus

class DECLSPEC_UUID("86DF7FB1-BE6A-40DB-85CC-DF71511EC21A")
SMIScan;
#endif
#endif /* __INTEGMOTORINTERFACELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


