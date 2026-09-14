cmake_minimum_required(VERSION 3.16)
project(NetPulse VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include src)

file(GLOB_RECURSE LIB_SOURCES "src/net/*.cpp")
add_library(NetPulse_lib ${LIB_SOURCES})

add_executable(NetPulse src/main.cpp)
target_link_libraries(NetPulse PRIVATE NetPulse_lib pthread)

add_executable(NetPulseExample examples/main.cpp)
target_link_libraries(NetPulseExample PRIVATE NetPulse_lib pthread)
