#ifndef _TMBEP_COMMON_HOOKER_H_
#define _TMBEP_COMMON_HOOKER_H_

#include "stdafx.h"
#include <string>
#include <windows.h>
#include <mutex>
#include <map>

typedef struct _tag_hook_info {
	unsigned int hook_type; 
	std::string host_dll_name;
	std::string dll_name;
	std::string func_name;
	PROC* iat_address;
	void* origin_func;
	void* hook_func;
} HookInfo;



class CommonHooker {

public:
	int Hook(const std::string& dll_name, 
		const std::string& func_name, 
		void* hook_func, 
		unsigned int hook_type, 
		const std::string& host_dll_name,
		void* reserved);

	void* GetOriginFuncAddr(const std::string& dll_name, const std::string& func_name, 
		const std::string& host_dll_name);

	int UnHookAll();

	bool ReHook(void* pfunc);

	int HookInline();
	bool EnableDebugPrivilege(bool is_enable);

public:
	void SetUseCustomKernalFunction(bool is_use_custom_kernal_function);

private:
	bool HookAddress(PROC* ppfn, PROC pfn_new);



private:
	std::map<std::string, HookInfo> hook_infos_;
	bool is_use_custom_kernal_function_;


};





#endif