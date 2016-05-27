#include "stdafx.h"
#include "fp_util.h"
#include "logger.h"
#include <boost/format.hpp>

std::wstring FPUtil::GetTempEnvPath() {
	wchar_t buff[MAX_PATH];
	const DWORD ret = GetEnvironmentVariableW(L"temp",buff,MAX_PATH);
	if (ret==0 || ret>MAX_PATH)
		return 0;
	else
		return std::wstring(buff,ret);
}

std::wstring FPUtil::GetCurrentModulePath() {
	wchar_t buff[MAX_PATH];
	const DWORD ret = GetModuleFileName(NULL,buff,MAX_PATH);
	if (ret==0 || ret>MAX_PATH)
		return 0;
	else
		return std::wstring(buff,ret);
}

void FPUtil::Crash() {
	((void(*)(void))0)();
}

std::wstring FPUtil::GetModuleFileVersion(const std::wstring& file_path) {
	DWORD  verHandle = NULL;
	UINT   size      = 0;
	LPBYTE lpBuffer  = NULL;
	DWORD  verSize   = GetFileVersionInfoSizeW(file_path.c_str(), &verHandle);
	std::wstring file_version;

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfoW(file_path.c_str(), verHandle, verSize, verData))
		{
			if (VerQueryValue(verData,L"\\",(VOID FAR* FAR*)&lpBuffer,&size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{
						boost::wformat wft(L"%d.%d.%d.%d");
						wft % ((verInfo->dwFileVersionMS >> 16 ) & 0xffff)
							% ((verInfo->dwFileVersionMS >>  0 ) & 0xffff)
							% ((verInfo->dwFileVersionLS >>  16 ) & 0xffff)
							% ((verInfo->dwFileVersionLS >>  0 ) & 0xffff);
						file_version = wft.str();
					}
				}
			}
		}
		delete[] verData;
	}

	return file_version;
}

std::wstring FPUtil::s2ws(const std::string& s)
{
	USES_CONVERSION;
	std::wstring wstrTmp(A2W(s.c_str()));
	return wstrTmp;
}

std::string FPUtil::ws2s(const std::wstring& ws)
{
	USES_CONVERSION;
	std::string strTmp(W2A(ws.c_str()));
	return strTmp;
}