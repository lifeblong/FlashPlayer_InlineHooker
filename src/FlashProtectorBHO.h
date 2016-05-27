// FlashProtectorBHO.h : Declaration of the CFlashProtectorBHO

#pragma once
#include "resource.h"       // main symbols



#include "FlashProtector_i.h"

#include <shlguid.h>
#include <exdispid.h> 
#include <string>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CFlashProtectorBHO

class ATL_NO_VTABLE CFlashProtectorBHO :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFlashProtectorBHO, &CLSID_FlashProtectorBHO>,
	public IObjectWithSiteImpl<CFlashProtectorBHO>,
	public IDispatchImpl<IFlashProtectorBHO, &IID_IFlashProtectorBHO, &LIBID_FlashProtectorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventImpl<1, CFlashProtectorBHO, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
public:
	CFlashProtectorBHO();
	~CFlashProtectorBHO();

DECLARE_REGISTRY_RESOURCEID(IDR_FLASHPROTECTORBHO)

DECLARE_NOT_AGGREGATABLE(CFlashProtectorBHO)

BEGIN_COM_MAP(CFlashProtectorBHO)
	COM_INTERFACE_ENTRY(IFlashProtectorBHO)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
public:
	BEGIN_SINK_MAP(CFlashProtectorBHO)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
	END_SINK_MAP()

	// DWebBrowserEvents2
	void STDMETHODCALLTYPE OnBeforeNavigate2(IDispatch *pDisp,
		VARIANT *URL,VARIANT *Flags,VARIANT *TargetFrameName,
		VARIANT *PostData,VARIANT *Headers,VARIANT_BOOL *Cancel);
	void STDMETHODCALLTYPE OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL); 

public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);

private:
	std::wstring GetStringFromVariant(VARIANT*);
	CComPtr<IWebBrowser2>  m_spWebBrowser;
	bool m_fAdvised;
};

OBJECT_ENTRY_AUTO(__uuidof(FlashProtectorBHO), CFlashProtectorBHO)
