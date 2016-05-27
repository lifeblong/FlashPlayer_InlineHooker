#include "stdafx.h"
#include "fp_memory_dumper.h"
#include "fp_util.h"
#include "logger.h"
#include <boost/format.hpp>
#include "DbgHelp.h"
#include "profile_helper.h"

bool FPMemoryDumper::CreateMiniDump() {
	PF_SCOPE_PROFILE;
	std::wstring temp_path = FPUtil::GetTempEnvPath();
	if(temp_path.empty()) {
		return false;
	}
	boost::wformat wft(L"%s\\fp_%s.dmp");
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	wchar_t buff[32] = {0};
	wsprintf(buff, L"%4d-%02d-%02d-%02d-%02d-%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	wft % temp_path.c_str() % buff;
	std::wstring dump_file_full_path = wft.str();
	LOG_TRACE_EX("[DUMP] path=[" << dump_file_full_path << "]");
	HANDLE hFile = CreateFileW(dump_file_full_path.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	//
	if(hFile == NULL) {
		return false;
	}
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
		MiniDumpWithFullMemory,
		NULL, NULL, NULL);
	CloseHandle(hFile);
	return true;
}