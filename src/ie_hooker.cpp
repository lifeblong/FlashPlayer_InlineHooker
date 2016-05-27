#include "stdafx.h"
#include "ie_hooker.h"

#include <atlbase.h>
#include <InitGuid.h>
#include <GuidDef.h>
#include <activscp.h>

#include "GlobalDef.h"
#include "logger.h"
#include "module_finder.h"

#include <vector>
#include <string>
#include <TlHelp32.h>
#include <boost/algorithm/string.hpp>
#include "util_hex.h"
#include "flash_inline_hooker.h"
#include "fp_util.h"
#include "fp_memory_dumper.h"
#include "profile_helper.h"

bool g_fp_has_hooked = false;
extern unsigned char* g_get_method_name_func;
/* push these in global namespace */
typedef HRESULT
	(__stdcall *Func_CoCreateInstance)(__in   REFCLSID rclsid,
	__in   LPUNKNOWN pUnkOuter,
	__in   DWORD dwClsContext,
	__in   REFIID riid,
	__out  LPVOID *ppv);

typedef
	HRESULT
	(__stdcall *Func_CoGetClassObject)(__in   REFCLSID rclsid,
	__in	 DWORD dwClsContext,
	__in	 LPVOID pvReserved,
	__in	 REFIID riid,
	__out	 LPVOID *ppv);

/* ************************ */


HRESULT __stdcall
	FED_CoCreateInstance(__in   REFCLSID rclsid,
	__in   LPUNKNOWN pUnkOuter,
	__in   DWORD dwClsContext,
	__in   REFIID riid,
	__out  LPVOID *ppv) {

	PF_SCOPE_PROFILE;
 	HRESULT hr;

	OLECHAR* bstrGuid;
	CLSID clsdEngine = rclsid;
	StringFromCLSID(clsdEngine, &bstrGuid);
	//LOG_TRACE_EX("[IEHook] CLSID=[" << bstrGuid << "]");
	::CoTaskMemFree(bstrGuid);

 	Func_CoCreateInstance orgin_func = 
 		(Func_CoCreateInstance)IEHooker::GetIEHooker()->GetOriginFuncAddr("", "CoCreateInstance", "mshtml.dll");
 	if(orgin_func == NULL) {
 		return S_FALSE;
 	}
 
 	if (IsEqualIID(riid, IID_IActiveScript)) {
 		CLSID clsdEngine = rclsid;
         if (IsEqualCLSID(rclsid, CLSID_Flash)) {
			 LOG_TRACE_EX("!!!Find Flash CLSID");   
		 }
 		hr = orgin_func(clsdEngine, pUnkOuter, dwClsContext, riid, ppv);		     
 	} else {
 		hr = orgin_func(rclsid, pUnkOuter, dwClsContext, riid, ppv);
 	}
 	IEHooker::GetIEHooker()->ReHook(FED_CoCreateInstance);
	return hr;
}

HRESULT __stdcall
	FED_CoGetClassObject(__in	 REFCLSID rclsid,
	__in	 DWORD dwClsContext,
	__in	 LPVOID pvReserved,
	__in	 REFIID riid,
	__out	 LPVOID *ppv) {

	PF_SCOPE_PROFILE;

	OLECHAR* bstrGuid;
	CLSID clsdEngine = rclsid;
	StringFromCLSID(clsdEngine, &bstrGuid);
	::CoTaskMemFree(bstrGuid);

	Func_CoGetClassObject orgin_func = 
		(Func_CoGetClassObject)IEHooker::GetIEHooker()->GetOriginFuncAddr(
		"","CoGetClassObject", "urlmon.dll");

	if(orgin_func == NULL) {
		return S_FALSE;
	}

	HRESULT hr = orgin_func(rclsid, dwClsContext, pvReserved, riid, ppv);
	IEHooker::GetIEHooker()->ReHook(FED_CoGetClassObject);

	if (IsEqualCLSID(rclsid, CLSID_Flash)) {
		//LOG_TRACE_EX("!!!Find Flash CLSID");   
	} else {
		return hr;
	}

	if (g_fp_has_hooked) {
		//LOG_TRACE_EX("FP has been hooked, just return");
		return hr;
	}

	std::vector<FPModuleInfo> v;
	ModuleFinder::FindModule(L"",v);
	std::wstring file_version;
	if(v.size()>0) {
		file_version = FPUtil::GetModuleFileVersion(v[0].module_path);
		LOG_TRACE_EX("File Version=[" << file_version.c_str() << "]");
	}
	
	
	FPModuleInfo flash_module;
	if (!ModuleFinder::GetFlashModuleInfo(flash_module, v[0].module_name)) {
		return hr;
	}

	int begin_index = 0x650000; // let it in config
	unsigned char* begin_address = (unsigned char*)flash_module.base_address + begin_index;
	int find_size = flash_module.module_size - begin_index - 1;
	std::string flash_module_code = HexUtil::ToHex((char*)begin_address, find_size);
	//LOG_TRACE_EX("[Code] Hex Size=[" << flash_module_code.size() << "]");

	//WIN 17,0,0,188
	if (!file_version.empty()) {
		std::string ver_str = FPUtil::ws2s(file_version);
		std::string flash_ver_str = "WIN " + boost::replace_all_copy(ver_str, ".", ",");
		LOG_TRACE_EX("Flash version string is [" << flash_ver_str.c_str() << "]");
		std::string flash_ver_str_hex = HexUtil::ToHex((char*)flash_ver_str.c_str(), flash_ver_str.length());
		std::size_t find_pos = 0;
		std::size_t find_pos_tmp = flash_module_code.find(flash_ver_str_hex.c_str(), find_pos);
		if(find_pos_tmp == std::string::npos) {
			LOG_TRACE_EX("[Code] Can't find flash version string");
			//return hr;
		} else {
			unsigned char* flash_version_string_address = begin_address + (find_pos_tmp + 1) / 2;
			FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookFlashVersion, flash_version_string_address + strlen("WIN "));
		}
	}

	// 8B 4C 24 08 56 8B 74 24 08 8B 46 30 25 FF FF 7F
	std::size_t find_pos = 0;
	std::size_t find_pos_tmp = flash_module_code.find("8B4C2408568B7424088B463025FFFF7F",find_pos);
	if(find_pos_tmp == std::string::npos) {
		LOG_TRACE_EX("[Code] Can't find Jit function");
		return hr;
	} 
	unsigned char* jit_function_hook_address = NULL;
	//LOG_TRACE_EX("[Code] Find Jit function Pos=[" << find_pos_tmp << "]");
	jit_function_hook_address = begin_address + (find_pos_tmp + 1) / 2;
	//LOG_TRACE_EX("[Code] JIT Function Hook Address=[" << std::hex << jit_function_hook_address << "]");
	FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookJIT, jit_function_hook_address);

	// 8B 41 10 A8 01 74 13 83 E0 FE 74 0C 8B 40 0C 52
	find_pos = 0;
	find_pos_tmp = flash_module_code.find("8B4110A801741383E0FE740C8B400C52", find_pos);
	if(find_pos_tmp == std::string::npos) {
		LOG_TRACE_EX("[Code] Can't find get method name function");
		return hr;
	} 
	g_get_method_name_func = NULL;
	//LOG_TRACE_EX("[Code] Find Jit function Pos=[" << find_pos_tmp << "]");
	g_get_method_name_func = begin_address + (find_pos_tmp + 1) / 2;
	LOG_TRACE_EX("[Code] Get method name Function Address=[" << std::hex << g_get_method_name_func << "]");
	//FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookJIT, g_get_method_name_func);

	//53 56 8B F1 8B 46 08 8B 5E 10 57 8B 78 10 8B 40 (Int vector)
	find_pos = 0;
	find_pos_tmp = flash_module_code.find("53568BF18B46088B5E10578B78108B40", find_pos);
	if(find_pos_tmp == std::string::npos) {
		LOG_TRACE_EX("[Code] Can't find int vector function");
		return hr;
	} 
	unsigned char* int_vec_hook_address = NULL;
	//LOG_TRACE_EX("[Code] Find int vector function Pos=[" << find_pos_tmp << "]");
	int_vec_hook_address = begin_address + (find_pos_tmp + 1) / 2;
	//LOG_TRACE_EX("[Code] Int Vec Create Function Hook Address=[" << std::hex << int_vec_hook_address << "]");
	FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookIntVector, int_vec_hook_address);
	FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookIntVectorSaveThis, int_vec_hook_address + 0x6b);
	

	//53 56 8B F1 8B 46 08 8B 5E 10 57 8B 78 10 8B 40 (UInt vector)
	find_pos = find_pos_tmp + strlen("53568BF18B46088B5E10578B78108B40");
	find_pos_tmp = flash_module_code.find("53568BF18B46088B5E10578B78108B40", find_pos);
	if(find_pos_tmp == std::string::npos) {
		LOG_TRACE_EX("[Code] Can't find uint vector function");
		return hr;
	}
	unsigned char* uint_vec_hook_address = NULL;
	//LOG_TRACE_EX("[Code] Find uint vector function Pos=[" << find_pos_tmp << "]");
	uint_vec_hook_address = begin_address + (find_pos_tmp + 1) / 2;
	//LOG_TRACE_EX("[Code] UInt Vec Create Function Hook Address=[" << std::hex << uint_vec_hook_address << "]");
	FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookUIntVector, uint_vec_hook_address);
	FlashInlineHooker::GetInlineHooker()->AddHookInfoAndHook(kHookUIntVectorSaveThis, uint_vec_hook_address + 0x6b);

	g_fp_has_hooked = true;
	return hr;
}

IEHooker* IEHooker::GetIEHooker() {
	static IEHooker ie_hooker;
	return &ie_hooker;
}

bool IEHooker::ReHook(void* pfunc) {
	return hooker_.ReHook(pfunc);
}

int IEHooker::Hook(bool is_use_custom_kernal_function) {
    boost::mutex::scoped_lock locker(hook_lock_);
	hooker_.SetUseCustomKernalFunction(is_use_custom_kernal_function);

	int hook_ret_code = 0;
// 
//    LOG_TRACE_EX("[IEHook] Hooking mshtml...");
// 
//  	hook_ret_code = hooker_.Hook("",
// 		"CoCreateInstance",
// 		TMBP_CoCreateInstance,
// 		1,
// 		"mshtml.dll",
// 		(void*)"IE");
// 
// 	if (hook_ret_code == 0) {
// 		LOG_TRACE_EX("Hook mshtml CoCreateInstance succ");
// 	} else {
// 		LOG_TRACE_EX("Hook mshtml CoCreateInstance fail");
	//}

    hook_ret_code = hooker_.Hook("",
        "CoGetClassObject",
        FED_CoGetClassObject,
        1,
        "urlmon.dll",
        (void*)"IE");
    if (hook_ret_code == 0) {
      LOG_TRACE_EX("Hook urlmon CoGetClassObject succ");
    } else {
      LOG_TRACE_EX("Hook urlmon CoGetClassObject fail");
    }

	return hook_ret_code;
}


int IEHooker::UnHook() {
	boost::mutex::scoped_lock locker(hook_lock_);
	hooker_.UnHookAll();
	return 0;
}

void* IEHooker::GetOriginFuncAddr(const std::string& dll_name, const std::string& func_name, 
	const std::string& host_dll_name) {
	return hooker_.GetOriginFuncAddr(dll_name,func_name,host_dll_name);
}
