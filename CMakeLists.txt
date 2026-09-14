cmake_minimum_required(VERSION 3.16)
project(NetPulse VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE LIB_SOURCES "src/net/*.cpp")

add_library(NetPulse_lib STATIC ${LIB_SOURCES})

target_include_directories(NetPulse_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(NetPulse_lib PRIVATE /W4 /EHa)
else()
    target_compile_options(NetPulse_lib PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
endif()

find_package(Threads REQUIRED)
target_link_libraries(NetPulse_lib PUBLIC Threads::Threads)

if(WIN32)
    target_link_libraries(NetPulse_lib PRIVATE ws2_32)
endif()

add_executable(NetPulse src/main.cpp)
target_link_libraries(NetPulse PRIVATE NetPulse_lib)

add_executable(NetPulseExample examples/main.cpp)
target_link_libraries(NetPulseExample PRIVATE NetPulse_lib)
