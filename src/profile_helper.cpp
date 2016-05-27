#include "stdafx.h"
#include "profile_helper.h"
#include <sstream>
#include <iostream>
#include <boost/format.hpp>

namespace tmsa {

ProfileUnit::ProfileUnit(const std::string& name) {
    profile_unit_name_ = name;
    StartProfile();
}

ProfileUnit::~ProfileUnit() {
    StopProfile();
}
void ProfileUnit::SetName(std::string& name) {
    profile_unit_name_ = name;
}
void ProfileUnit::StartProfile() {
    cost_ms_ = 0;
    start_time_point_ = boost::chrono::steady_clock::now();
}

void ProfileUnit::StopProfile() {
    boost::chrono::steady_clock::time_point end_time_point = boost::chrono::steady_clock::now();
    cost_ms_ = boost::chrono::duration_cast<boost::chrono::milliseconds>(end_time_point-start_time_point_).count();
    //
    ProfileSummaryHelper::GetHelper()->AddProfileInfo(profile_unit_name_,cost_ms_);
}


ProfileSummaryHelper* ProfileSummaryHelper::GetHelper() {
    static ProfileSummaryHelper summary_helper;
    return &summary_helper;

}

void ProfileSummaryHelper::AddProfileInfo(const std::string& name, 
        unsigned long unit_cost_msg) {
    //
    boost::mutex::scoped_lock  locker(lock_);

    std::map<std::string,ProfileInfo>::iterator find_iter = profile_infos_.find(name);
    ProfileInfo profile_info;
    if(find_iter != profile_infos_.end()) {
        profile_info = find_iter->second;
        profile_info.hint_count += 1;
        profile_info.cost_ms += unit_cost_msg;
    } else {
       profile_info.name = name;
       profile_info.hint_count = 1;
       profile_info.cost_ms = unit_cost_msg;
    }
    profile_infos_[name] = profile_info;
}


std::string ProfileSummaryHelper::GetSummaryDesc() {

    boost::mutex::scoped_lock  locker(lock_);

    std::stringstream ss;
    ss << "=== Summary Info ===" << std::endl;
    
    std::map<std::string, ProfileInfo>::iterator summary_iter = profile_infos_.begin();
    for(; summary_iter != profile_infos_.end(); ++summary_iter) {
         boost::format ft_info("%-60s\tHints:%-6d\tCost(ms):%-10d");
         ft_info%summary_iter->first%summary_iter->second.hint_count%summary_iter->second.cost_ms;
         ss << ft_info.str() << std::endl;
    }
    ss << "=== Summary Info End ===" << std::endl;
    return ss.str();
}


}