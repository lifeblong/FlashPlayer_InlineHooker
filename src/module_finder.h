#ifndef _MODULE_FINDER_H_
#define _MODULE_FINDER_H_

#include <string>
#include <vector>

typedef struct _tag_FPModuleInfo {
	std::wstring module_name;
	unsigned char* base_address;
	unsigned long module_size;
	std::wstring module_path;
} FPModuleInfo;

class ModuleFinder {
public:
	static void FindModule(const std::wstring& find_key, std::vector<FPModuleInfo>& module_infos);
	static bool GetFlashModuleInfo(FPModuleInfo& flash_module_info, std::wstring& file_version);
	static std::wstring GetOcxName();

};





#endif