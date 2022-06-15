cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ compiler" FORCE)
set(CMAKE_C_COMPILER clang CACHE STRING "C compiler" FORCE)

project(FST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(fst main.cpp)

add_subdirectory(tests)
