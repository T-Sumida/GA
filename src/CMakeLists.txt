cmake_minimum_required(VERSION 3.5)
project(GA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp GA.hpp)
add_executable(GA ${SOURCE_FILES})