find_package(CppUTest REQUIRED)

add_executable(AllTests alltests.cpp
  fst_tests.cpp fst_search_tests.cpp)

target_link_libraries(AllTests PRIVATE
  fst_lib
  CppUTest
  CppUTestExt)

target_include_directories(AllTests PUBLIC
  "${PROJECT_SOURCE_DIR}"
)
