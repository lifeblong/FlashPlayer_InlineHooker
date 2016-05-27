#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
class CLogger {

public:
	static CLogger* GetInstance();
	
	void  WriteLog(char* file_name, int line_no, wchar_t* msg);
private:
	CLogger(std::string log_file_name);
	CLogger();
	static CLogger* instance_;
	std::string log_file_name_;
	std::wofstream log_stream_;
};

#define _LOG_MACRO(logEvent) {			\
	std::wostringstream logBuf;						                                \
	logBuf << logEvent;										                                \
	CLogger::GetInstance()->WriteLog(__FILE__, __LINE__, (wchar_t *)logBuf.str().c_str());	\
}

#define LOG_TRACE(msg) CLogger::GetInstance()->WriteLog(__FILE__, __LINE__, msg)
#define LOG_TRACE_EX(msg) _LOG_MACRO(msg)
#define LOG_TRACE_FUNCTION CLogger::GetInstance()->WriteLog(__FILE__, __LINE__, __FUNCTION__)