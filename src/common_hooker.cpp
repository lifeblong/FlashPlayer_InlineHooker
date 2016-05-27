#include "stdafx.h"
#include "common_hooker.h"
#include "HookImport.h"
#include "logger.h"

//#include "scan_client/ipc_logger.h"
#pragma comment(lib, "imagehlp.lib")

int CommonHooker::Hook(const std::string& dll_name, 
	const std::string& func_name, 
	void* hook_func, 
	unsigned int hook_type, 
	const std::string& host_dll_name,
	void* reserved) {

   // IPC_LOG_TRACE("[CHook] DllName=[" << dll_name.c_str() << "], FuncName=["
  //          << func_name.c_str() << "], HostDLLName=[" << host_dll_name.c_str() << "]");
	std::string hook_info_key = dll_name + func_name + host_dll_name;
	auto hook_info_iter = hook_infos_.find(hook_info_key);
	if(hook_info_iter != hook_infos_.end()) {
 		LOG_TRACE_EX("[CHook] Find Hooked, Return");
		return 0;
	}
	HMODULE hModule = NULL;
	hModule = ::GetModuleHandleA(host_dll_name.c_str());
	if(hModule == NULL) {
		LOG_TRACE_EX("[CHook] Can not Find Module=[" 
			<< host_dll_name.c_str() 
			<< "],Load DLL");
		hModule = ::LoadLibraryA(host_dll_name.c_str());
		if(hModule == NULL) {
			LOG_TRACE_EX("[CHook] Can not Find Module=[" 
				<< host_dll_name.c_str() 
				<< "],Error=" << GetLastError() << "]");
			return -1;
		}
		
	}

	PROC* ppfn = NULL;
	if(reserved == NULL) {
		ppfn = GetIATAPIAddr(hModule, dll_name.c_str(), func_name.c_str(), FALSE);
		if(ppfn == NULL) {
			ppfn = GetIATAPIAddr(hModule, dll_name.c_str(), func_name.c_str(), TRUE);
		}
	} else {
		ppfn = GetIATAPIAddr(hModule, dll_name.c_str(), func_name.c_str(), FALSE, reserved);
		if(ppfn == NULL) {
			ppfn = GetIATAPIAddr(hModule, dll_name.c_str(), func_name.c_str(), TRUE, reserved);
		}
	}

	if(ppfn == NULL || *ppfn == NULL) {
       LOG_TRACE_EX("[CHook] GetIATAPIAddr Error");
		return -1;
	}

	PROC orgin_addr = *ppfn;
	if(!HookAddress(ppfn,(PROC)hook_func)) {
		LOG_TRACE_EX("[CHook] HookAddress Error");
		return -1;
	}

	HookInfo hook_info = {0};
	hook_info.hook_type = hook_type;
	hook_info.host_dll_name = host_dll_name;
	hook_info.dll_name = dll_name;
	hook_info.func_name = func_name;
	hook_info.iat_address = ppfn;
	hook_info.origin_func = orgin_addr;
	hook_info.hook_func = hook_func;

	hook_infos_[hook_info_key] = hook_info;

	return 0;
}


void* CommonHooker::GetOriginFuncAddr(const std::string& dll_name, const std::string& func_name, 
					   const std::string& host_dll_name) {

	auto iter = hook_infos_.find(dll_name+func_name+host_dll_name);
	if(iter != hook_infos_.end()) {
		return iter->second.origin_func;
	} else {
		return NULL;
	}
}

bool CommonHooker::HookAddress(PROC* ppfn, PROC pfn_new) {

	if(ppfn == NULL || *ppfn == pfn_new) {
		return false;
	}

	HANDLE hCurrProcess = GetCurrentProcess();
	if(WriteProcessMemory(hCurrProcess,ppfn, &pfn_new,sizeof(pfn_new), NULL) == TRUE) {
		//IPC_LOG_TRACE("[CHook] Write Memory Succ!");
        return true;
	}
	
    //
    DWORD dwErr = GetLastError();
   // IPC_LOG_TRACE("[CHook] Write Memory Failed, Result=[" << dwErr << "]");

	MEMORY_BASIC_INFORMATION memory_info = {0};
	SIZE_T nSize = VirtualQuery(ppfn, &memory_info, sizeof(MEMORY_BASIC_INFORMATION));
	if (nSize == 0) {
       // IPC_LOG_TRACE("[CHook] VirtualQuery Size=0");
		return false;
	}

	bool is_change_protection_succ = true;
	if(memory_info.Protect == PAGE_READONLY) {
		DWORD dwOldProtection = 0;
		if(VirtualProtect(ppfn,sizeof(ppfn),PAGE_EXECUTE_READ,&dwOldProtection) == FALSE) {
           // IPC_LOG_TRACE("[CHook] VirtualProtect GetOldProtection Failed");
			return false;
		}
        //The addresses match, change the import section address
		if(WriteProcessMemory(hCurrProcess, ppfn, &pfn_new,sizeof(pfn_new), NULL) == FALSE) {
           // IPC_LOG_TRACE("[CHook] WriteProcessMemory Failed Code=[" << GetLastError() << "]");
			is_change_protection_succ = false;
		}

		DWORD dwNewProtection = 0;
		if(VirtualProtect(ppfn,sizeof(ppfn),dwOldProtection,&dwNewProtection) == FALSE) {
           // IPC_LOG_TRACE("[CHook] VirtualProtect Failed");
			return false;
		}
	}
    //
   // IPC_LOG_TRACE("[CHook] Good! Result=[" << is_change_protection_succ << "]");


	return is_change_protection_succ;
}

void CommonHooker::SetUseCustomKernalFunction(bool is_use_custom_kernal_function) {
	is_use_custom_kernal_function_ = is_use_custom_kernal_function;
}

int CommonHooker::UnHookAll() {
	for(auto hook_item : hook_infos_) {
		//if(hook_item->second.hook)
		HookInfo& hook_info = hook_item.second;
		HookAddress(hook_info.iat_address,(PROC)hook_info.origin_func);
	}
	hook_infos_.clear();

	/* process inlinehook */
// #ifndef _WIN64
// 	if(inline_hooker_.isHooked()) {
// 		EnableDebugPrivilege(true);
// 		inline_hooker_.Unhook();
// 		EnableDebugPrivilege(false);
// 	}
// #endif // !_WIN64

	return 0;
}

bool CommonHooker::ReHook(void* pfunc) {
	bool has_been_hooked = false;
	auto hook_item_iter = hook_infos_.begin();
	for( ; hook_item_iter!= hook_infos_.end(); ++hook_item_iter) {
		if(hook_item_iter->second.hook_func == pfunc) {
			has_been_hooked = true;
			break;
		}
	}

	if(!has_been_hooked) {
		return false;
	}

	if(hook_item_iter->second.hook_func == (void*)*(hook_item_iter->second.iat_address)) {
		return true;
	}

	return HookAddress(hook_item_iter->second.iat_address, 
		(PROC)pfunc);
}





int CommonHooker::HookInline() {
// #ifndef _WIN64
// 	return inline_hooker_.Hook();
// #else
// 	return 0;
// #endif

	return 0;
}

bool CommonHooker::EnableDebugPrivilege(bool is_enable) {
	bool enable_succ = false;
	HANDLE hToken = NULL;
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES, &hToken) == FALSE) {
		return false;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
	tp.Privileges[0].Attributes = is_enable ? SE_PRIVILEGE_ENABLED : 0;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	if(GetLastError() == ERROR_SUCCESS) {
		enable_succ = true;
	} else {
		enable_succ = false;
	}
	CloseHandle(hToken);
	return enable_succ;
}

