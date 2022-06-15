find_package(CppUTest REQUIRED)

add_executable(AllTests alltests.cpp fst_tests.cpp)

target_link_libraries(AllTests PRIVATE
  CppUTest
  CppUTestExt)
