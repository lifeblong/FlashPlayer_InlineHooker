

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Wed Sep 02 10:10:48 2015
 */
/* Compiler settings for FlashProtector.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

#ifndef __FlashProtector_i_h__
#define __FlashProtector_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFlashProtectorBHO_FWD_DEFINED__
#define __IFlashProtectorBHO_FWD_DEFINED__
typedef interface IFlashProtectorBHO IFlashProtectorBHO;

#endif 	/* __IFlashProtectorBHO_FWD_DEFINED__ */


#ifndef __FlashProtectorBHO_FWD_DEFINED__
#define __FlashProtectorBHO_FWD_DEFINED__

#ifdef __cplusplus
typedef class FlashProtectorBHO FlashProtectorBHO;
#else
typedef struct FlashProtectorBHO FlashProtectorBHO;
#endif /* __cplusplus */

#endif 	/* __FlashProtectorBHO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IFlashProtectorBHO_INTERFACE_DEFINED__
#define __IFlashProtectorBHO_INTERFACE_DEFINED__

/* interface IFlashProtectorBHO */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IFlashProtectorBHO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("87BC9A75-529D-4369-BEAD-ED35F629C1A1")
    IFlashProtectorBHO : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IFlashProtectorBHOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFlashProtectorBHO * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFlashProtectorBHO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFlashProtectorBHO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFlashProtectorBHO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFlashProtectorBHO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFlashProtectorBHO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFlashProtectorBHO * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IFlashProtectorBHOVtbl;

    interface IFlashProtectorBHO
    {
        CONST_VTBL struct IFlashProtectorBHOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFlashProtectorBHO_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFlashProtectorBHO_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFlashProtectorBHO_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFlashProtectorBHO_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFlashProtectorBHO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFlashProtectorBHO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFlashProtectorBHO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFlashProtectorBHO_INTERFACE_DEFINED__ */



#ifndef __FlashProtectorLib_LIBRARY_DEFINED__
#define __FlashProtectorLib_LIBRARY_DEFINED__

/* library FlashProtectorLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_FlashProtectorLib;

EXTERN_C const CLSID CLSID_FlashProtectorBHO;

#ifdef __cplusplus

class DECLSPEC_UUID("558E0E3A-A671-409D-A0ED-A57598EE8011")
FlashProtectorBHO;
#endif
#endif /* __FlashProtectorLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


