#ifndef _FP_UTIL_H_
#define _FP_UTIL_H_

#include <string>

class FPUtil {
public:
	static std::wstring GetTempEnvPath();
	static std::wstring GetCurrentModulePath();
	static void Crash();
	static std::wstring GetModuleFileVersion(const std::wstring& file_path);
	static std::wstring s2ws(const std::string& s);
	static std::string ws2s(const std::wstring& ws);
};






#endif