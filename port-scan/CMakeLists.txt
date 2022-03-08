cmake_minimum_required(VERSION 3.16)
project(port_scan)

set(CMAKE_CXX_STANDARD 14)

add_library(port_scan_lib
        udp_scanner.cpp
        tcp_scanner.cpp)

add_executable(port_scan main.cpp)
target_link_libraries(port_scan port_scan_lib)

## testing

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(port_scan_tests port_scan_tests.cpp)
target_link_libraries(port_scan_tests port_scan_lib gtest gtest_main pthread)

gtest_discover_tests(port_scan_tests)