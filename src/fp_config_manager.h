#ifndef _FP_CONFIG_MANAGER_H_
#define _FP_CONFIG_MANAGER_H_

class FPConfigManager {
public:
	static FPConfigManager* GetConfigManger();
	~FPConfigManager();

private:
	FPConfigManager();
private:
	static FPConfigManager* instance_;
};








#endif