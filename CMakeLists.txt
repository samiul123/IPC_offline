cmake_minimum_required(VERSION 3.5)
project(IPC_offline)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(IPC_offline ${SOURCE_FILES})