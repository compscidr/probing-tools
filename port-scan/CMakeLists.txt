cmake_minimum_required(VERSION 3.16)
project(port_scan)

set(CMAKE_CXX_STANDARD 14)

add_executable(port_scan main.cpp udp_scanner.cpp udp_scanner.h tcp_scanner.cpp tcp_scanner.h)