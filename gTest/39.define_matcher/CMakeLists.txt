cmake_minimum_required(VERSION 3.10)
project(GTestExample)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIRS /usr/local/include /usr/include/c++/11)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(define_matcher define_matcher.cpp)

target_include_directories(define_matcher PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(define_matcher ${GTEST_LIB_DIR}/libgtest.a 
    ${GTEST_LIB_DIR}/libgtest_main.a 
    ${GTEST_LIB_DIR}/libgmock.a
    ${GTEST_LIB_DIR}/libgmock_main.a
    pthread)

enable_testing()
add_test(NAME DefineMatcher COMMAND define_matcher)
