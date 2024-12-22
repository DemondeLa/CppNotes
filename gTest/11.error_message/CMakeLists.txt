cmake_minimum_required(VERSION 3.10)
project(GTestExample)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIR /usr/local/include)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(error_message better_error_message.cpp)

target_include_directories(error_message PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(error_message ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

enable_testing()
add_test(NAME ErrorMessage COMMAND error_message)
