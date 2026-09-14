#include <string>
#include <vector>
#include <mutex>

namespace NetPulse {
    namespace Net {
        namespace UI {
            struct NetworkNode {
                std::string id;
                std::string label;
                std::string status;
                std::string ip_address;
                int latency_ms;
            };

            class Network {
            public:
                Network() = default;
                ~Network() = default;

                void add_node(const NetworkNode& node) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    nodes_.push_back(node);
                }

                bool remove_node(const std::string& id) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    auto it = std::remove_if(nodes_.begin(), nodes_.end(),
                        [&id](const NetworkNode& node) { return node.id == id; });
                    
                    if (it != nodes_.end()) {
                        nodes_.erase(it, nodes_.end());
                        return true;
                    }
                    return false;
                }

                bool update_status(const std::string& id, const std::string& status, int latency) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    for (auto& node : nodes_) {
                        if (node.id == id) {
                            node.status = status;
                            node.latency_ms = latency;
                            return true;
                        }
                    }
                    return false;
                }

                std::vector<NetworkNode> get_nodes() const {
                    std::lock_guard<std::mutex> lock(mutex_);
                    return nodes_;
                }

                size_t node_count() const {
                    std::lock_guard<std::mutex> lock(mutex_);
                    return nodes_.size();
                }

                void clear() {
                    std::lock_guard<std::mutex> lock(mutex_);
                    nodes_.clear();
                }

            private:
                std::vector<NetworkNode> nodes_;
                mutable std::mutex mutex_;
            };
        }
    }
}
