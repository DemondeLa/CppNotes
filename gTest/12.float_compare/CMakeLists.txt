cmake_minimum_required(VERSION 3.10)
project(GTestExxample)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIR /usr/local/include)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(CompareFloat compare_float.cpp)

target_include_directories(CompareFloat PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(CompareFloat ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

enable_testing()
add_test(NAME compare_float COMMAND CompareFloat)
