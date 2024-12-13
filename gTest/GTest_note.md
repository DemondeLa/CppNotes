## Google Test

### 一、介绍

test.cpp

```cpp
#include <gtest/gtest.h>

int  main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);// 这行代码初始化 Google Test 框架
    // testing::InitGoogleTest 是 Google Test 框架提供的一个函数，用于处理命令行参数并初始化框架，以便可以在命令行中传递一些参数（例如运行特定的测试）
    return RUN_ALL_TESTS(); // 这一行会启动所有的测试
    // RUN_ALL_TESTS() 是 Google Test 提供的一个宏，表示运行所有的测试用例并返回结果。它会扫描当前项目中的所有测试，并执行这些测试。
    // 如果所有测试都通过，它会返回 0；如果有任何一个测试失败，它会返回一个非零的值。
}
```

其中，`&argc`传地址，主要是为了让 `InitGoogleTest` 可以修改 `argc` 的值

在 Google Test 中，`InitGoogleTest` 可能会根据命令行参数对 `argc` 进行修改（例如，如果有一些特定的 Google Test 选项被解析出来，它可能会更新 `argc`，表示剩下的命令行参数的数量）。因此，我们传递 `&argc`（即传递 `argc` 的地址），使得 `InitGoogleTest` 可以在内部修改它的值。

```cmake
cmake_minimum_required(VERSION 3.10) # 设置 CMake 的最低版本要求。
project(GTestExample) # 定义项目的名称，是 GTestExample

# 添加 Google Test 库的路径
find_package(GTest REQUIRED) # 查找 Google Test 的安装路径并引入它到当前项目中
# GTest 是 Google Test 的库名称
# REQUIRED 表示如果没有找到 Google Test，CMake 将报错并停止执行
# CMake 会尝试从系统路径（例如 /usr/lib 或 /usr/local/lib）中查找已安装的 Google Test 库

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 添加测试源文件
# add_executable(test_example test.cpp) 
add_executable(test_example test.cpp test_add.cpp) 
# 定义一个可执行目标 test_example，并指定需要用来构建它的源文件
# test_example 是生成的可执行文件的名称, test_example.cpp 和 test_add.cpp 是项目中的源文件，CMake 会对它们进行编译并链接
# 这行的意思是：使用 test_example.cpp 和 test_add.cpp 构建一个名为 test_example 的可执行程序

# 链接 GTest 库 和 pthread 库
target_link_libraries(test_example GTest::GTest GTest::Main pthread)
# 为目标程序 test_example 链接所需的库
# GTest::GTest 包含 Google Test 的核心功能
# GTest::Main 提供了 main() 函数的实现，用于运行测试
# pthread 是 POSIX 线程库，在多线程环境下通常是必须的，尤其是在 Linux 系统上
# 这行代码告诉 CMake，将这些库链接到目标程序 test_example，以便运行时可以正确使用 Google Test 的功能
```

之后再添加要测试的函数，并对其进行测试，test_add.cpp

```cpp
#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(Add, AddTwoNumber) {
    ASSERT_EQ(add(1, 2), 3);
}
```

完成以上3个文件后进行编译、链接、运行。结果如下所示

```bash
$ mkdir build && cd build
/build$ cmake ..
/build$ make
/build$ ./test_example 
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Add
[ RUN      ] Add.AddTwoNumber(测试套件.测试用例)
[       OK ] Add.AddTwoNumber (0 ms)
[----------] 1 test from Add (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

另一种编译运行方式：这种方式构建高度抽象，适合各种构建系统

> 但是，使用 `cmake --build` 构建项目之前，必须先配置项目，即运行 `cmake` 命令以生成 `build` 目录中的构建系统文件（例如 `Makefile` 或 Ninja 文件）

```bash
/build$ cmake --build . -v # 其中.表示目标构建目录的路径，这里.表示build目录
# cmake --build 是一个跨平台命令，它告诉 CMake 使用生成的构建系统（如 Makefile、Ninja 等）来构建目标。
# -v 表示启用详细模式，会打印出详细的构建过程(比如调用的编译命令)
/build$ ./test_example 
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from Add
[ RUN      ] Add.AddTwoNumber
[       OK ] Add.AddTwoNumber (0 ms)
[ RUN      ] Add.AddTwoNumber2
/home/will/lesson/gTest/01.test/test_add.cpp:12: Failure
Expected equality of these values:
  add(3, 4)
    Which is: 7 # 实际的返回值
  1 # 在代码中设置的期望返回值

[  FAILED  ] Add.AddTwoNumber2 (0 ms)
[----------] 2 tests from Add (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] Add.AddTwoNumber2

 1 FAILED TEST
```



### 二、参数化测试

参数化测试表示：如果测试用例要执行多组数据的测试，如果只是用`TEST`来写，会写多个结构相似的函数，定义多个类似的测试用例

为了避免这种重复性的动作，gtest提供了参数化测试的方法：即先把测试的结构写好，然后传多个数据即可，`test_add.cpp`

```cpp
#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(Add, case1) {
    ASSERT_EQ(add(1, 2), 3);
}

TEST(Add, case2) {
    ASSERT_EQ(add(3, 4), 7);
}

// 参数化测试类
class NumberTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

// 参数化测试
TEST_P(NumberTest, AddTwoNumber) {
    auto param = GetParam();
    EXPECT_EQ(add(std::get<0>(param), std::get<1>(param)), std::get<2>(param));
}

// 参数实例化
INSTANTIATE_TEST_SUITE_P(AddTestPrefix, NumberTest, 
                         ::testing::Values(std::make_tuple(1, 2, 3), 
                                           (std::make_tuple(4, 5, 9))));
```

其中，`NumberTest` 类：是继承自 `::testing::TestWithParam<T>`这个模板类，其中 `T` 是参数类型，因此需要实例化一个参数，这里测试参数类型为一个包含三个整数的 元组`std::tuple`，前两个元素表示`add`函数的参数，第三个元素是前两个元素之和，用于测试

`TEST_P` 宏：用于定义参数化测试。`NumberTest` 是参数化测试类名，`AddTwoNumber` 是测试用例的名称

`auto param = GetParam();`，获取当前测试运行时的参数（`std::tuple<int, int, int>` 类型）`GetParam()` 是 GTest 提供的函数，返回一个与 `TestWithParam` 定义的模板类型匹配的参数。

`std::get<n>(param)`使用`std::get`从`tuple` 中提取第$n$个元素：`std::get<0>(param)` 是第一个参数（`int` 类型）；`std::get<1>(param)` 是第二个参数；`std::get<2>(param)` 是预期结果。

> `std::get` 是 C++ 标准库中的一个函数模板，专门用于访问 `std::tuple` 或 `std::array` 中的元素

`INSTANTIATE_TEST_SUITE_P` 宏：用于将参数集与参数化测试关联，其中参数`AddTestPrefix`：测试实例的前缀，用于生成实际测试名称，生成的测试名称类似 `AddTestPrefix/AddTwoNumber/0`；

`NumberTest`：表示参数化测试类；

`::testing::Values`：提供参数的集合：每个参数是一个`std::tuple<int, int, int>`，对应`add`函数的两个输入和预期输出：

- `(1, 2, 3)` 表示 `add(1, 2)` 的预期输出是 `3`。
- `(4, 5, 9)` 表示 `add(4, 5)` 的预期输出是 `9`。

然后使用CMakeList来进行编译

```bash
02.test$ mkdir build
02.test$ cd build
02.test/build$ cmake ..
-- The C compiler identification is GNU 11.4.0
-- The CXX compiler identification is GNU 11.4.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found GTest: /usr/local/lib/cmake/GTest/GTestConfig.cmake (found version "1.15.2")  
-- Configuring done
-- Generating done
-- Build files have been written to: /home/will/lesson/gTest/02.test/build
will@will-vm:~/lesson/gTest/02.test/build$ cmake --build .
[ 33%] Building CXX object CMakeFiles/test_example.dir/test.cpp.o
[ 66%] Building CXX object CMakeFiles/test_example.dir/test_add.cpp.o
[100%] Linking CXX executable test_example
[100%] Built target test_example
will@will-vm:~/lesson/gTest/02.test/build$ ./test_example
[==========] Running 4 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from Add
[ RUN      ] Add.case1
[       OK ] Add.case1 (0 ms)
[ RUN      ] Add.case2
[       OK ] Add.case2 (0 ms)
[----------] 2 tests from Add (0 ms total)

# 对于不同的测试套件会进行分组，并且在每个测试套件的末尾会进行总结

[----------] 2 tests from AddTestPrefix/NumberTest
[ RUN      ] AddTestPrefix/NumberTest.AddTwoNumber/0
# AddTestPrefix就是测试实例的前缀，然后是分隔符/，之后是NumberTest参数化测试类，再.上测试用例名称AddTwoNumber，再加上序号/0（从0开始
[       OK ] AddTestPrefix/NumberTest.AddTwoNumber/0 (0 ms)
[ RUN      ] AddTestPrefix/NumberTest.AddTwoNumber/1
[       OK ] AddTestPrefix/NumberTest.AddTwoNumber/1 (0 ms)
[----------] 2 tests from AddTestPrefix/NumberTest (0 ms total)

[----------] Global test environment tear-down # 全局的总结
[==========] 4 tests from 2 test suites ran. (0 ms total)
[  PASSED  ] 4 tests.
```

利用参数化测试避免重复代码，简化测试用例的编写。







