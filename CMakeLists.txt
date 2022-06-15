cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ compiler" FORCE)
set(CMAKE_C_COMPILER clang CACHE STRING "C compiler" FORCE)

project(FST)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(fst)
add_subdirectory(tests)

add_executable(fst_main main.cpp)
target_link_libraries(fst_main PUBLIC fst_lib)
target_include_directories(fst_main PUBLIC
  "${PROJECT_SOURCE_DIR}/fst"
)
