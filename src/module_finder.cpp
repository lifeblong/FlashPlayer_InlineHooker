#include "stdafx.h"
#include "module_finder.h"
#include <TlHelp32.h>
#include <boost/algorithm/string.hpp>
#include "logger.h"
//#include <Psapi.h>
//#pragma comment( lib, "psapi.lib" )

void ModuleFinder::FindModule(const std::wstring& find_key, std::vector<FPModuleInfo>& module_infos) {
	module_infos.clear();
	// take a snapshot of all modules in the specified process
	HANDLE snaphot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	if (snaphot_handle != INVALID_HANDLE_VALUE)
	{
		// first module
		MODULEENTRY32 mod_entry;
		mod_entry.dwSize = sizeof(mod_entry);
		if (Module32First(snaphot_handle, &mod_entry))
		{
			// iterate through the module list of the process
			do
			{
				std::wstring module_name = mod_entry.szModule;
				if(boost::icontains(module_name,L"flash") && boost::iends_with(module_name,L"ocx")) {
					FPModuleInfo minfo;
					minfo.module_name = module_name;
					minfo.base_address = mod_entry.modBaseAddr;
					minfo.module_size = mod_entry.modBaseSize;
					minfo.module_path = mod_entry.szExePath;

					module_infos.push_back(minfo);
					LOG_TRACE_EX("[ModuleFinder] Find Moudule=[" <<
						module_name.c_str() << "], Path=["
						<< minfo.module_path << "],"
						<< std::hex << "BaseAddr=[0x" << minfo.base_address << "], Size=[0x"
						<< minfo.module_size << "]");

				}

			} while (Module32Next(snaphot_handle, &mod_entry));
		}
		CloseHandle(snaphot_handle);

	}
}

bool ModuleFinder::GetFlashModuleInfo(FPModuleInfo& flash_module_info, std::wstring& file_version) {
    // maybe can't work under EPM, need test
	//std::wstring file_name = boost::replace_all_copy(file_version, ".", "_");
	//std::wstring ocx_name = L"Flash32_" + file_name + L".ocx";
	std::wstring ocx_name = file_version;
	LOG_TRACE_EX("OCX name is [" << ocx_name.c_str() << "]");

	HMODULE hBase = GetModuleHandle(ocx_name.c_str());
	if (!hBase) {
		LOG_TRACE_EX("Can't get flash module handle");
		return false;
	}

	flash_module_info.module_name = ocx_name;
	flash_module_info.base_address = (unsigned char*)hBase;
	flash_module_info.module_size = 0xF00000;  // let this into config
	return true;
}

std::wstring ModuleFinder::GetOcxName() {
	std::wstring dir = L"C:\\Windows\\System32\\Macromed\\Flash\\*";
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	hFind = FindFirstFile(dir.c_str(), &ffd);
	std::wstring result;

	if (INVALID_HANDLE_VALUE == hFind) {
		LOG_TRACE_EX("FindFirstFileA failed! last_error = " << GetLastError());
		return result;
	} 

	// List all the files in the directory with some info about them.

	do {
		std::wstring name = ffd.cFileName;
		//LOG_TRACE_EX("a file found! name = [" << ffd.cFileName << "]");
		if (name.find(L".ocx") != -1) {
			result = name;
			//LOG_TRACE_EX("ocx found! name = " << result.c_str());
			break;
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		//MessageBox(NULL, L"GetLastError", L"GetLastError", MB_OK);
		//LOG_TRACE_EX("error found! error = " << dwError);
	}

	FindClose(hFind);
	return result;
}