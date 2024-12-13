cmake_minimum_required(VERSION 3.10)
project(GTestExample)

# 添加 Google Test 库的路径
find_package(GTest REQUIRED)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 添加测试源文件
# add_executable(test_example test.cpp)
add_executable(test_example test.cpp test_add.cpp)

# 链接 GTest 库 和 pthread 库
target_link_libraries(test_example GTest::GTest GTest::Main pthread)
