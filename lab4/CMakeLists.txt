cmake_minimum_required(VERSION 2.8.4)
project(lab4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/SCMPQueue.hpp src/MCMPQueue.hpp src/PThreadHelp.hpp)
add_executable(lab4 ${SOURCE_FILES})