#include "stdafx.h"
#include "logger.h"

std::string g_log_file_name = "C:\\fp\\flash_protector.log";

CLogger* CLogger::instance_ = NULL;

CLogger::CLogger(std::string log_file_name) 
	: log_stream_(std::wofstream(log_file_name))
{
	log_file_name_ = log_file_name;
}

CLogger::CLogger() 
	: log_stream_(std::wofstream(g_log_file_name, std::ios::app))
{
	log_file_name_ = g_log_file_name;
}

CLogger* CLogger::GetInstance() {
	if (instance_ == NULL) {
		instance_ = new CLogger;
	}

	return instance_;
}

void  CLogger::WriteLog(char* file_name, int line_no, wchar_t* msg) {
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	wchar_t buff[32] = {0};
	wsprintf(buff, L"%4d-%02d-%02d-%02d-%02d-%02d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

	log_stream_ << "[IE] "  << "[" << GetCurrentProcessId() << "] " << "[" << GetCurrentThreadId() << "] " <<  msg << " [" << file_name << ":" << line_no << "] " << "[" << buff << "]" << std::endl;
	//log_stream_ << "aaaaaaaaaaaa";
}