cmake_minimum_required(VERSION 3.10)
project(GTestExample)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIR /usr/local/include)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(general_test generalized_assert.cpp)

target_include_directories(general_test PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(general_test ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

enable_testing()
add_test(NAME GeneralTest COMMAND general_test)
