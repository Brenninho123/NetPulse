#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <atomic>
#include <future>
#include <algorithm>
#include "../src/net/Paths.cpp"
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
    test_thread_pool();

    std::cout << "All NetPulse tests passed successfully!" << std::endl;
    return 0;
}
