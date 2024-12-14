cmake_minimum_required(VERSION 3.10)
project(GTestExample)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 设置Google Test 的头文件路径
set(GTEST_INCLUDE_DIR /usr/local/include) 
# 设置Google Test 的库文件路径
set(GTEST_LIB_DIR /usr/local/lib)

# 添加测试源文件
add_executable(test_add add.cpp test.cpp)

# 设置头文件目录
target_include_directories(test_add PRIVATE ${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

# 链接 Google Test库
target_link_libraries(test_add ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

# 启用CTest支持，方便使用 make test 运行测试
enable_testing()
add_test(NAME test_add COMMAND test_add)
