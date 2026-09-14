#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <atomic>
#include <future>
#include "../src/net/Paths.cpp"
#include "../src/net/ui/Network.cpp"
#include "../src/net/ui/boost/Boost.cpp"
#include "../src/net/ThreadPool.cpp"

void test_paths() {
    NetPulse::Net::Paths paths;
    paths.register_path("/api/v1", "192.168.1.10");
    
    assert(paths.has_path("/api/v1") == true);
    assert(paths.resolve("/api/v1") == "192.168.1.10");
    assert(paths.count() == 1);
    
    assert(paths.remove_path("/api/v1") == true);
    assert(paths.has_path("/api/v1") == false);
    assert(paths.count() == 0);
}

void test_network_ui() {
    NetPulse::Net::UI::Network net;
    NetPulse::Net::UI::NetworkNode node{"node1", "Gateway", "online", "10.0.0.1", 12};
    
    net.add_node(node);
    assert(net.node_count() == 1);
    
    assert(net.update_status("node1", "degraded", 45) == true);
    auto nodes = net.get_nodes();
    assert(nodes[0].status == "degraded");
    assert(nodes[0].latency_ms == 45);
    
    assert(net.remove_node("node1") == true);
    assert(net.node_count() == 0);
}

void test_boost_engine() {
    NetPulse::Net::UI::Boost::BoostEngine boost;
    NetPulse::Net::UI::Boost::BoostProfile profile{"p1", "node1", 5, 100.0, true};
    
    boost.create_profile(profile);
    assert(boost.get_active_profiles().size() == 1);
    
    assert(boost.set_active("p1", false) == true);
    assert(boost.get_active_profiles().size() == 0);
    
    assert(boost.remove_profile("p1") == true);
}

void test_thread_pool() {
    NetPulse::Net::ThreadPool pool(4);
    std::atomic<int> counter{0};
    const int total_tasks = 20;

    std::vector<std::future<int>> results;
    for (int i = 0; i < total_tasks; ++i) {
        results.push_back(pool.enqueue([&counter, i]() {
            counter.fetch_add(1, std::memory_order_relaxed);
            return i * 2;
        }));
    }

    for (int i = 0; i < total_tasks; ++i) {
        assert(results[i].get() == i * 2);
    }

    assert(counter.load() == total_tasks);
}

int main() {
    test_paths();
    test_network_ui();
    test_boost_engine();
    test_thread_pool();

    std::cout << "All NetPulse tests passed successfully!" << std::endl;
    return 0;
}
