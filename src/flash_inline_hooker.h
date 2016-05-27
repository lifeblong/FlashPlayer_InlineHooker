#ifndef _FLASH_INLINE_HOOKER_H_
#define _FLASH_INLINE_HOOKER_H_

#include <string>
#include <map>

const int kJitHookBytesNum = 5;
const int kVecHookBytesNum = 7;
const int kVecSaveThisHookBytesNum = 5;
const int kVersionHookBytesNum = 2;
const std::string kHookJIT = "hook_jit";
const std::string kHookUIntVector = "hook_uint_vector";
const std::string kHookIntVector = "hook_int_vector";
const std::string kHookUIntVectorSaveThis = "hook_uint_vector_save_this";
const std::string kHookIntVectorSaveThis = "hook_int_vector_save_this";
const std::string kHookFlashVersion = "hook_flash_version";

struct FlashInlineHookInfo {

	std::string hook_name;
	unsigned char* hook_addr;
	char* old_code;
	char* new_code;
	bool is_hooked;
	int hooked_bytes;
	FlashInlineHookInfo();
	~FlashInlineHookInfo();
	void Init(int hook_bytes);
};


class FlashInlineHooker {
public:
	static FlashInlineHooker* GetInlineHooker();
	~FlashInlineHooker();
	bool AddHookInfoAndHook(const std::string& hook_name, unsigned char* hook_addr);
	//
	bool DoHook(const std::string& hook_name);
	bool DoUnHook(const std::string& hook_name);
	unsigned int GetNewHookFunctionAddress(const std::string& hook_name);
public:
	static void* save_addr_jit;
	static void* save_addr_int_vec;
	static void* save_addr_uint_vec;

private:
	FlashInlineHooker();
	static FlashInlineHooker* instance_;

private:
	std::map<std::string,FlashInlineHookInfo> flash_inline_hook_infos;
};











#endif