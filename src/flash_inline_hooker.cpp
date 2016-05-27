#include "stdafx.h"
#include "flash_inline_hooker.h"
#include <Windows.h>
#include "logger.h"
#include "ExploitVerifier.h"
#include "profile_helper.h"

typedef unsigned int uint;
namespace global {  
	uint gEAX = 0;  
	uint gEBX = 0;  
	uint gECX = 0;  
	uint gEDX = 0;  
	uint gESP = 0;  
	uint gEBP = 0;  
	uint gESI = 0;  
	uint gEDI = 0;  

	uint gRet = 0;  // 临时的返回地址  
	uint gTmp = 0;  // 一些临时的值保存  
	uint gPar = 0;  // 被hook函数的正常返回地址  
	uint gCnt = 1;  // 当前hook的次数  
	uint gMax = 0;  // 最大hook次数，为0表示一直hook  
	bool bEnt = 0;  // 是否为第一次进入hook函数  
}


void* FlashInlineHooker::save_addr_jit = NULL;
void* FlashInlineHooker::save_addr_int_vec = NULL;
void* FlashInlineHooker::save_addr_uint_vec = NULL;

unsigned int g_saved_vec_this_edi = NULL;

FlashInlineHookInfo::FlashInlineHookInfo() {
	hook_addr = NULL;
	old_code = NULL;
	new_code = NULL;
	is_hooked = false;
	hooked_bytes = 0;
}
FlashInlineHookInfo::~FlashInlineHookInfo() {
	if (new_code) {
		delete new_code;
	}
	if (old_code) {
		delete old_code;
	}
}

void FlashInlineHookInfo::Init(int hook_bytes) {
	if (new_code) {
		delete new_code;
	}
	new_code = new char[hook_bytes];
	if (old_code) {
		delete old_code;
	}
	old_code = new char[hook_bytes];
	hooked_bytes = hook_bytes;
}

void FLH_HookJIT(char* method_name) {
	//PF_SCOPE_PROFILE;
	//LOG_TRACE_EX("FLH_HookJIT");
	std::string name;
	char* name_ptr = (char*)*(int*)(method_name + 8);
	unsigned int len = *(int*)(method_name + 16);
	if (name_ptr != NULL) {
		name = std::string(name_ptr, name_ptr + len);
	}
	//LOG_TRACE_EX("Jit method name is [" << name.c_str() << "]");
	ExploitVerifier::GetInstance()->EventNotify(ExploitVerifier::AS3EXEEVENT_JIT, (void*)name.c_str());
}

void FLH_HookIntVec() {
	//PF_SCOPE_PROFILE;
	//LOG_TRACE_EX("FLH_HookIntVec");
	//ExploitVerifier::GetInstance()->EventNotify(ExploitVerifier::AS3EXEEVENT_CREATE_INT_VEC);
}

void FLH_HookUIntVec() {
	//PF_SCOPE_PROFILE;
	//LOG_TRACE_EX("FLH_HookUIntVec");
	//ExploitVerifier::GetInstance()->EventNotify(ExploitVerifier::AS3EXEEVENT_CREATE_UINT_VEC);
}

void FLH_HookIntVec_SaveThis() {
	//PF_SCOPE_PROFILE;
	//LOG_TRACE_EX("FLH_HookIntVec_SaveThis");
	ExploitVerifier::GetInstance()->EventNotify(ExploitVerifier::AS3EXEEVENT_CREATE_INT_VEC, (void*)g_saved_vec_this_edi);
}

void FLH_HookUIntVec_SaveThis() {
	//PF_SCOPE_PROFILE;
	//LOG_TRACE_EX("FLH_HookUIntVec_SaveThis");
	ExploitVerifier::GetInstance()->EventNotify(ExploitVerifier::AS3EXEEVENT_CREATE_UINT_VEC, (void*)g_saved_vec_this_edi);
}


unsigned char* g_get_method_name_func = NULL;
void __declspec( naked ) FIH_NewFunc_JIT( void )  
{  
	__asm  
	{  
		mov global::gESP, esp // 本函数的返回地址
		pushad
		mov ebx, global::gESP
		mov eax, [ebx]   // 本函数的返回地址
		mov global::gRet, eax  // 保存返回地址  
		mov ecx, edi  // call getmethodname
		call g_get_method_name_func
		push eax
		call FLH_HookJIT    //   
		pop  global::gTmp // 弹出参数
		popad
		pop  global::gTmp // 弹出本函数返回值
		// 执行被hook覆盖的5字节代码
		mov ecx, [esp + 8]
		push esi
	    push global::gRet //压入本函数的返回值
		ret
	}
					
}  


void __declspec( naked ) FIH_NewFunc_IntVec( void )  
{  
	__asm  
	{  
		mov global::gESP, esp // 本函数的返回地址
		pushad
		mov edi, global::gESP
		mov eax, [edi]   // 本函数的返回地址
		mov global::gRet, eax  // 保存返回地址  
		call FLH_HookUIntVec    //   
		popad
		pop  global::gTmp // 弹出本函数返回值
		// 执行被hook覆盖的7字节代码
		push    ebx
		push    esi
		mov     esi, ecx
		mov     eax, [esi+8]
		push global::gRet //压入本函数的返回值
		ret
	}  
}  

void __declspec( naked ) FIH_NewFunc_UintVec( void )  
{  
	__asm  
	{  
		mov global::gESP, esp // 本函数的返回地址
		pushad
		mov edi, global::gESP
		mov eax, [edi]   // 本函数的返回地址
		mov global::gRet, eax  // 保存返回地址  
		call FLH_HookIntVec    //   
		popad
		pop  global::gTmp // 弹出本函数返回值
		// 执行被hook覆盖的7字节代码
		push    ebx
		push    esi
		mov     esi, ecx
		mov     eax, [esi+8]
		push global::gRet //压入本函数的返回值
		ret
	}  
}  

void __declspec( naked ) FIH_NewFunc_IntVec_SaveObjectThis( void )  
{  
	__asm  
	{  
		mov global::gESP, esp // 本函数的返回地址
		mov g_saved_vec_this_edi, edi // save this pointer
		pushad
		mov ebx, global::gESP
		mov eax, [ebx]   // 本函数的返回地址
		mov global::gRet, eax  // 保存返回地址  
		call FLH_HookIntVec_SaveThis    //   
		popad
		pop  global::gTmp // 弹出本函数返回值
		// 执行被hook覆盖的5字节代码
		lea     ecx, [edi+10h]
		mov     edx, esi
		push global::gRet //压入本函数的返回值
		ret
	}  
}  

void __declspec( naked ) FIH_NewFunc_UintVec_SaveObjectThis( void )  
{  
	__asm  
	{  
		mov global::gESP, esp // 本函数的返回地址
		mov g_saved_vec_this_edi, edi // save this pointer
		pushad
		mov ebx, global::gESP
		mov eax, [ebx]   // 本函数的返回地址
		mov global::gRet, eax  // 保存返回地址  
		call FLH_HookUIntVec_SaveThis    //   
		popad
		pop  global::gTmp // 弹出本函数返回值
		// 执行被hook覆盖的5字节代码
		lea     ecx, [edi+10h]
		mov     edx, esi
		push global::gRet //压入本函数的返回值
		ret
	}  
}  

FlashInlineHooker::FlashInlineHooker() {
	save_addr_jit = NULL;
	save_addr_int_vec = NULL;
	save_addr_uint_vec = NULL;
}


FlashInlineHooker* FlashInlineHooker::instance_ = NULL;
FlashInlineHooker* FlashInlineHooker::GetInlineHooker() {
	if(instance_ == NULL) {
		instance_ = new FlashInlineHooker();
	}
	return instance_;
}

bool FlashInlineHooker::AddHookInfoAndHook(const std::string& hook_name, unsigned char* hook_addr) {
	unsigned int new_function_addr = 0;
	FlashInlineHookInfo hook_info;
	if(hook_name == kHookJIT) {
		new_function_addr = (unsigned int)(FIH_NewFunc_JIT);
		save_addr_jit = hook_addr;
		hook_info.Init(kJitHookBytesNum);
		LOG_TRACE_EX("[Code] JIT New Function Hook Address=[" << std::hex << new_function_addr << "]");
	} else if(hook_name == kHookIntVector) {
		hook_info.Init(kVecHookBytesNum);
		new_function_addr =  (unsigned int)(FIH_NewFunc_IntVec);
		save_addr_int_vec = hook_addr;
		LOG_TRACE_EX("[Code] Int Vec New Function Hook Address=[" << std::hex << new_function_addr << "]");
	} else if(hook_name == kHookUIntVector) {
		hook_info.Init(kVecHookBytesNum);
		new_function_addr = (unsigned int)(FIH_NewFunc_UintVec);
		save_addr_uint_vec = hook_addr;
		LOG_TRACE_EX("[Code] UInt Vec New Function Hook Address=[" << std::hex << new_function_addr << "]");
	} else if (hook_name == kHookFlashVersion){
		hook_info.Init(kVersionHookBytesNum);
		
	} else if(hook_name == kHookIntVectorSaveThis) {
		hook_info.Init(kVecSaveThisHookBytesNum);
		new_function_addr = (unsigned int)(FIH_NewFunc_IntVec_SaveObjectThis);
		save_addr_uint_vec = hook_addr;
		LOG_TRACE_EX("[Code] Int Vec Save this New Function Hook Address=[" << std::hex << new_function_addr << "]");
	}else if(hook_name == kHookUIntVectorSaveThis) {
		hook_info.Init(kVecSaveThisHookBytesNum);
		new_function_addr = (unsigned int)(FIH_NewFunc_UintVec_SaveObjectThis);
		save_addr_uint_vec = hook_addr;
		LOG_TRACE_EX("[Code] UInt Vec Save this New Function Hook Address=[" << std::hex << new_function_addr << "]");
	}else {
		return false;
	}

	
	hook_info.is_hooked = false;
	hook_info.hook_name = hook_name;
	hook_info.hook_addr = hook_addr;
	//hook_info.new_code[ 0 ] = ( char )0xe8; // call

	if (hook_name == kHookFlashVersion) {
		hook_info.new_code[0] = '1';
		hook_info.new_code[1] = '6';
	} else {
		hook_info.new_code[ 0 ] = ( char )0xe8; // call

		(unsigned int& )hook_info.new_code[ 1 ] = ( unsigned int )new_function_addr - (unsigned int)hook_addr - 5;
		if (hook_info.hooked_bytes > 5) {
			for (int i = 5; i < hook_info.hooked_bytes; ++i) {
				hook_info.new_code[i] = 0x90;
			}
		}
	}
	
	flash_inline_hook_infos[hook_name] = hook_info;

	DoHook(hook_name);

	return true;
}

unsigned int FlashInlineHooker::GetNewHookFunctionAddress(const std::string& hook_name) {
	if(hook_name == kHookJIT) {
		return (unsigned int)(FIH_NewFunc_JIT);
	} else if(hook_name == kHookIntVector) {
		return (unsigned int)(FIH_NewFunc_IntVec);
	} else if(hook_name == kHookUIntVector) {
		return (unsigned int)(FIH_NewFunc_UintVec);
	}
	return 0;
}

bool FlashInlineHooker::DoHook(const std::string& hook_name) {
	LOG_TRACE_EX("DoHook Name=[" << hook_name.c_str() << "]");
	auto& find_hook_info = flash_inline_hook_infos.find(hook_name);
	if(find_hook_info == flash_inline_hook_infos.end()) {
		return false;
	}

	FlashInlineHookInfo& hook_info = find_hook_info->second;
// 	if(hook_info.is_hooked == true) {
// 		return true;
// 	}
	DWORD dwFlag;
	void* hook_addr = hook_info.hook_addr;
	int hook_bytes_num = hook_info.hooked_bytes;
	if ( VirtualProtect(hook_addr, hook_bytes_num, PAGE_EXECUTE_READWRITE, &dwFlag ) )  
	{  
		memcpy((void*)hook_info.old_code, hook_addr, hook_bytes_num );  
		memcpy(hook_addr, hook_info.new_code, hook_bytes_num );  
		VirtualProtect(hook_addr, hook_bytes_num, dwFlag, &dwFlag );  
		return true;  
	} 
	return false;
}

bool FlashInlineHooker::DoUnHook(const std::string& hook_name) {
	LOG_TRACE_EX("DoUnHook Name=[" << hook_name.c_str() << "]");
	auto& find_hook_info = flash_inline_hook_infos.find(hook_name);
	if(find_hook_info == flash_inline_hook_infos.end()) {
		return false;
	}

	FlashInlineHookInfo& hook_info = find_hook_info->second;
	
// 	if(hook_info.is_hooked == false) {
// 		return true;
// 	}
	DWORD dwFlag;  
	void* hook_addr = hook_info.hook_addr;
	int hook_bytes_num = hook_info.hooked_bytes;
	if ( VirtualProtect(hook_addr, hook_bytes_num, PAGE_EXECUTE_READWRITE, &dwFlag)) {  
		memcpy(hook_addr, hook_info.old_code, hook_bytes_num );  
		VirtualProtect(hook_addr, hook_bytes_num, dwFlag, &dwFlag );  
		return true;
	}
	return false;
}