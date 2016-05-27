#ifndef _TMSA_PROFILE_HELPER_H_
#define _TMSA_PROFILE_HELPER_H_

#include <string>
#include <map>
#include <boost/chrono.hpp>
#include <boost/thread/mutex.hpp>

namespace tmsa {

class ProfileUnit {
public:
    explicit ProfileUnit(const std::string& name);
    ~ProfileUnit();
    void StartProfile();
    void StopProfile();
    void SetName(std::string& name);
    
private:
    std::string profile_unit_name_;
    boost::chrono::steady_clock::time_point start_time_point_;
    unsigned long cost_ms_;
};

typedef struct _tag_profile_info {
    std::string name;
    unsigned int hint_count;
    unsigned long cost_ms;
} ProfileInfo;


class ProfileSummaryHelper {
public:
    static ProfileSummaryHelper* GetHelper();
    void AddProfileInfo(const std::string& name, unsigned long unit_cost_msg);
    std::string GetSummaryDesc();
private:
    std::map<std::string, ProfileInfo> profile_infos_;
    boost::mutex lock_;
};

} //end namespace



#define PF_SCOPE_PROFILE_VAR_NAME(pos) pu##pos
#define PF_SCOPE_PROFILE_VAR_NAME_LINE PF_SCOPE_PROFILE_VAR_NAME(__LINE__)
#define PF_SCOPE_PROFILE_WITH_NAME(pos,name) tmsa::ProfileUnit PF_SCOPE_PROFILE_VAR_NAME_LINE(name);
#define PF_SCOPE_PROFILE PF_SCOPE_PROFILE_WITH_NAME(__LINE__,__FUNCTION__)
#define PF_SCOPE_PROFILE_P1(p_name) PF_SCOPE_PROFILE_WITH_NAME(__LINE__,p_name)
//
#define PF_GET_SUMMARY_STR tmsa::ProfileSummaryHelper::GetHelper()->GetSummaryDesc













#endif
