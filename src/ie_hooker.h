#ifndef _TMBEP_IE_HOOKER_H_
#define _TMBEP_IE_HOOKER_H_

#include <boost/thread/mutex.hpp>
#include "common_hooker.h"

class IEHooker {
public:
	static IEHooker* GetIEHooker();
	int Hook(bool is_use_custom_kernal_function);
	int UnHook();

	bool ReHook(void* pfunc);
	void* GetOriginFuncAddr(const std::string& dll_name, const std::string& func_name, 
		const std::string& host_dll_name);


private:
    boost::mutex hook_lock_;
	CommonHooker hooker_;


};


#endif