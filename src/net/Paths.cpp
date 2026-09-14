#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>

namespace NetPulse {
    namespace Net {
        class Paths {
        public:
            Paths() = default;
            ~Paths() = default;

            void register_path(const std::string& path, const std::string& destination) {
                std::lock_guard<std::mutex> lock(mutex_);
                routes_[path] = destination;
            }

            bool remove_path(const std::string& path) {
                std::lock_guard<std::mutex> lock(mutex_);
                return routes_.erase(path) > 0;
            }

            std::string resolve(const std::string& path) const {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = routes_.find(path);
                if (it != routes_.end()) {
                    return it->second;
                }
                return "";
            }

            bool has_path(const std::string& path) const {
                std::lock_guard<std::mutex> lock(mutex_);
                return routes_.find(path) != routes_.end();
            }

            std::vector<std::string> get_all_paths() const {
                std::lock_guard<std::mutex> lock(mutex_);
                std::vector<std::string> keys;
                keys.reserve(routes_.size());
                for (const auto& pair : routes_) {
                    keys.push_back(pair.first);
                }
                return keys;
            }

            void clear() {
                std::lock_guard<std::mutex> lock(mutex_);
                routes_.clear();
            }

            size_t count() const {
                std::lock_guard<std::mutex> lock(mutex_);
                return routes_.size();
            }

        private:
            std::unordered_map<std::string, std::string> routes_;
            mutable std::mutex mutex_;
        };
    }
}
