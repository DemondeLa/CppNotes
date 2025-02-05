cmake_minimum_required(VERSION 3.10)
project(GTestExample)

set(CMAKE_CXX_STANDARD 14)

set(GTEST_INCLUDE_DIRS /usr/local/include /usr/include/c++/11)
set(GTEST_LIB_DIR /usr/local/lib)

add_executable(nice_strict nice_strict.cpp)

target_include_directories(nice_strict PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(nice_strict ${GTEST_LIB_DIR}/libgtest.a 
    ${GTEST_LIB_DIR}/libgtest_main.a 
    ${GTEST_LIB_DIR}/libgmock.a
    ${GTEST_LIB_DIR}/libgmock_main.a
    pthread)

enable_testing()
add_test(NAME NiceStrict COMMAND nice_strict)
