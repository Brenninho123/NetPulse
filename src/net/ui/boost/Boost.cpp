#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <algorithm>

namespace NetPulse {
    namespace Net {
        namespace UI {
            namespace Boost {
                struct BoostProfile {
                    std::string profile_id;
                    std::string target_node_id;
                    int priority_level;
                    double allocated_bandwidth_mbps;
                    bool active;
                };

                class BoostEngine {
                public:
                    BoostEngine() = default;
                    ~BoostEngine() = default;

                    void create_profile(const BoostProfile& profile) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        profiles_.push_back(profile);
                    }

                    bool set_active(const std::string& profile_id, bool state) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        for (auto& profile : profiles_) {
                            if (profile.profile_id == profile_id) {
                                profile.active = state;
                                return true;
                            }
                        }
                        return false;
                    }

                    bool update_priority(const std::string& profile_id, int priority) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        for (auto& profile : profiles_) {
                            if (profile.profile_id == profile_id) {
                                profile.priority_level = priority;
                                return true;
                            }
                        }
                        return false;
                    }

                    std::vector<BoostProfile> get_active_profiles() const {
                        std::lock_guard<std::mutex> lock(mutex_);
                        std::vector<BoostProfile> active_list;
                        for (const auto& profile : profiles_) {
                            if (profile.active) {
                                active_list.push_back(profile);
                            }
                        }
                        return active_list;
                    }

                    bool remove_profile(const std::string& profile_id) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        auto it = std::remove_if(profiles_.begin(), profiles_.end(),
                            [&profile_id](const BoostProfile& p) { return p.profile_id == profile_id; });

                        if (it != profiles_.end()) {
                            profiles_.erase(it, profiles_.end());
                            return true;
                        }
                        return false;
                    }

                    void clear() {
                        std::lock_guard<std::mutex> lock(mutex_);
                        profiles_.clear();
                    }

                private:
                    std::vector<BoostProfile> profiles_;
                    mutable std::mutex mutex_;
                };
            }
        }
    }
}
