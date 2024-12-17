cmake_minimum_required(VERSION 3.10)
project(TraceTest)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIR /usr/local/include)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(trace_test scoped_trace.cpp)

target_include_directories(trace_test PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(trace_test PRIVATE ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

enable_testing()
add_test(NAME TraceTest COMMAND trace_test)
