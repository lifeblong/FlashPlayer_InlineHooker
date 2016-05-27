// FlashProtectorBHO.cpp : Implementation of CFlashProtectorBHO

#include "stdafx.h"
#include <atlcom.h>
#include "FlashProtectorBHO.h"
#include "logger.h"
#include "ie_hooker.h"
#include "profile_helper.h"
#include <Iepmapi.h> //IEIsProtectedModeProcess

std::wstring g_cur_url = L"";

// CFlashProtectorBHO
CFlashProtectorBHO::CFlashProtectorBHO() {

}
CFlashProtectorBHO::~CFlashProtectorBHO() {
	LOG_TRACE_EX(PF_GET_SUMMARY_STR().c_str());
}
STDMETHODIMP CFlashProtectorBHO::SetSite(IUnknown* pUnkSite)
{
	if (pUnkSite != NULL)
	{
		// Cache the pointer to IWebBrowser2.
		HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void **)&m_spWebBrowser);
		if (SUCCEEDED(hr))
		{
			// Register to sink events from DWebBrowserEvents2.
			hr = DispEventAdvise(m_spWebBrowser);
			if (SUCCEEDED(hr))
			{
				m_fAdvised = TRUE;
			}
		}
		//
		BOOL is_ie_protect_mode = FALSE;
		hr = IEIsProtectedModeProcess(&is_ie_protect_mode);
		long hook_result = 0;
		if(SUCCEEDED(hr) && is_ie_protect_mode) {
			hook_result = IEHooker::GetIEHooker()->Hook(true);
		} else {
			hook_result = IEHooker::GetIEHooker()->Hook(false);
		}

	}
	else
	{
		// Unregister event sink.
		if (m_fAdvised)
		{
			DispEventUnadvise(m_spWebBrowser);
			m_fAdvised = FALSE;
		}

		// Release cached pointers and other resources here.
		m_spWebBrowser.Release();
	}

	// Call base class implementation.
	return IObjectWithSiteImpl<CFlashProtectorBHO>::SetSite(pUnkSite);
}

std::wstring CFlashProtectorBHO::GetStringFromVariant(VARIANT* variant) {
	std::wstring tmp;
	if(variant->vt == VT_BSTR) {
		tmp = std::wstring(variant->bstrVal, ::SysStringLen(variant->bstrVal));
	} else if (variant->vt == (VT_BYREF|VT_VARIANT)) {
		VARIANT* u = (VARIANT*)variant->byref;
		tmp = std::wstring(u->bstrVal, ::SysStringLen(u->bstrVal));
	}
	return tmp;
}

void STDMETHODCALLTYPE CFlashProtectorBHO::OnBeforeNavigate2(IDispatch *pDisp,
													 VARIANT *URL,VARIANT *Flags,VARIANT *TargetFrameName,
													 VARIANT *PostData,VARIANT *Headers,VARIANT_BOOL *Cancel) {
	std::wstring navi_url = GetStringFromVariant(URL) ;
	LOG_TRACE_EX("[Event BeforeNavigate2] URL = [" << navi_url.c_str() << "]");
	if (navi_url.find(L".html") != -1) {
		g_cur_url = navi_url;
	}
}

void STDMETHODCALLTYPE CFlashProtectorBHO::OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL)
{
	// Retrieve the top-level window from the site.
// 	HWND hwnd;
// 	HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);
// 	if (SUCCEEDED(hr))
// 	{
// 		LOG_TRACE("Test Result");
// 		// Output a message box when page is loaded.
// 		//MessageBox(hwnd, L"Hello World!", L"BHO", MB_OK);
// 	}
}
