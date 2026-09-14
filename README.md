===============================================================================
                                NetPulse ⚡
===============================================================================

NetPulse is a lightweight, high-performance, multi-threaded networking core 
written in C++17. Designed with zero external dependencies, it features an 
asynchronous thread-pool engine, thread-safe path routing, interface state 
management, and graceful shutdown handling.


-------------------------------------------------------------------------------
1. FEATURES
-------------------------------------------------------------------------------
- Asynchronous Processing: Dynamic ThreadPool for handling concurrent network 
  connections.
- Thread-Safe Architecture: Synchronization using std::mutex across all core 
  modules.
- Graceful Shutdown: Native POSIX signal handling (SIGINT / SIGTERM) for safe 
  resource cleanup.
- Cross-Platform Build: Configured with modern CMake for Linux, macOS, and 
  Windows.
- Zero Dependencies: Depends solely on C++17 standard features and native socket 
  APIs.


-------------------------------------------------------------------------------
2. PROJECT STRUCTURE
-------------------------------------------------------------------------------
NetPulse/
├── .github/
│   └── workflows/
│       └── main.yml
├── CMakeLists.txt
├── include/
├── src/
│   ├── main.cpp
│   └── net/
│       ├── Paths.cpp
│       ├── ThreadPool.cpp
│       └── ui/
│           ├── Network.cpp
│           └── boost/
│               └── Boost.cpp
├── examples/
│   └── main.cpp
└── tests/
    └── test_main.cpp


-------------------------------------------------------------------------------
3. PREREQUISITES
-------------------------------------------------------------------------------
- C++ Compiler: Supporting C++17 (GCC 8+, Clang 7+, or MSVC 2019+)
- CMake: Version 3.16 or higher
- Build System: make, ninja, or Visual Studio


-------------------------------------------------------------------------------
4. BUILDING AND RUNNING
-------------------------------------------------------------------------------
1. Clone the repository:
   git clone https://github.com/Brenninho123/NetPulse.git
   cd NetPulse

2. Configure and compile:
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release

3. Run the core application:
   ./build/NetPulse

4. Run tests:
   cd build
   ctest --output-on-failure


-------------------------------------------------------------------------------
5. CODE EXAMPLE
-------------------------------------------------------------------------------
#include "net/ThreadPool.cpp"
#include "net/Paths.cpp"

int main() {
    NetPulse::Net::ThreadPool pool(4);
    NetPulse::Net::Paths paths;

    paths.register_path("/api/v1", "127.0.0.1:8080");

    pool.enqueue([&paths]() {
        if (paths.has_path("/api/v1")) {
            std::string target = paths.resolve("/api/v1");
        }
    });

    return 0;
}


-------------------------------------------------------------------------------
6. LICENSE
-------------------------------------------------------------------------------
This project is licensed under the MIT License.
