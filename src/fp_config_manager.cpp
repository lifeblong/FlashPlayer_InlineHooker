#include "stdafx.h"
#include "fp_config_manager.h"

FPConfigManager* FPConfigManager::instance_ = NULL;
FPConfigManager* FPConfigManager::GetConfigManger() {
	if(instance_ == NULL) {
		instance_ = new FPConfigManager();
	}
	return instance_;
}

FPConfigManager::FPConfigManager() {

}

FPConfigManager::~FPConfigManager() {

}