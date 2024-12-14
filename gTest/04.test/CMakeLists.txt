cmake_minimum_required(VERSION 3.10)
project(GTestExample)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 设置Google Test 的头文件路径
set(GTEST_INCLUDE_DIR /usr/local/include) 
# 设置Google Test 的库文件路径
set(GTEST_LIB_DIR /usr/local/lib)

# 添加测试源文件
add_executable(type_test type_test.cpp)
add_executable(type_struct type_struct_test.cpp)

# 设置头文件目录
target_include_directories(type_test PRIVATE ${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
target_include_directories(type_struct PRIVATE ${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

# 链接 Google Test库
target_link_libraries(type_test ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)
target_link_libraries(type_struct ${GTEST_LIB_DIR}/libgtest.a ${GTEST_LIB_DIR}/libgtest_main.a pthread)

# 启用CTest支持，方便使用 make test 运行测试
enable_testing()

# 添加测试
add_test(NAME TypeTest COMMAND type_test)
add_test(NAME StructTest COMMAND type_struct)
