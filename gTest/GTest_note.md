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

总结一下（顺便也是区别下面的`TEST_F`）:

`TEST_P()` 用于定义 **参数化测试（Parameterized Test）**，它允许你运行同一个测试逻辑，但是使用不同的输入数据集。与 `TEST_F()` 不同，`TEST_P()` 是用来处理同一测试案例对不同数据的多次执行。

- **适用场景**：当你希望用不同的输入数据来测试同一个函数、类或代码块时，使用 `TEST_P()`。这种方式有助于减少重复代码，特别是在测试函数或方法需要对多个输入进行验证时。
- **工作原理**：你需要定义一个参数化的测试夹具类，并通过 `INSTANTIATE_TEST_SUITE_P` 来指定输入的参数列表。



### 三、`Test Fixture`测试套件（测试夹具）

如果发现自己编写了两个或多个对类似数据进行操作的测试，则可以使用测试夹具

`TEST_F()` 用于定义基于 **测试夹具（test fixture）** 的单元测试。它与 `::testing::Test` 类结合使用，可以在每个测试用例前后进行初始化和清理。

- **适用场景**：当你希望在多个测试用例中共享同一个初始化和清理过程时，使用 `TEST_F()`。测试夹具中的 `SetUp()` 和 `TearDown()` 方法可以帮助你管理每个测试用例的环境。
- **工作原理**：每个 `TEST_F()` 测试用例会通过继承的测试夹具类来访问共享的测试状态。

```cpp
#include "add.h"
#include <gtest/gtest.h>

class AddTest : public ::testing::Test
{
public:
    AddTest() {
        std::cout << "AddTest\n";
    }
    ~AddTest() {
        std::cout << "~AddTest\n";
    }
    void SetUp() override {
        std::cout << "SetUp\n";
    }
    void TearDown() override {
        std::cout << "TearDown\n";
    }
};

TEST_F(AddTest, cast1) {
    std::cout << "case1: \n";
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(3, 4), 7);
}

TEST_F(AddTest, cast2) {
    std::cout << "case2: \n";
    EXPECT_EQ(add(-1, 1), 0);
    EXPECT_EQ(add(-1, -1), -2);
}
```

这里首相定义了一个`AddTest`类，继承自`::testing::Test`，其中`testing`是gtest的命名空间，`Test`是 Google Test 提供的一个基础类，本身提供了许多用于测试的工具，比如设置和清理测试状态的功能。因此用它来实现 **Test Fixture**

Test Fixture 允许你在多个测试用例中共享一些公共的初始化和清理逻辑。

- `SetUp()` 是 Google Test 提供的一个虚函数，用于在每个测试用例执行前进行初始化。

    在这个函数中，你可以做一些设置工作，比如初始化数据、分配资源等。

- `TearDown()` 是 Google Test 提供的一个虚函数，用于在每个测试用例执行后进行清理。

    在这个函数中，你可以进行一些清理工作，比如释放资源、销毁对象等。

`TEST_F` 是用于定义与 Test Fixture 相关联的测试用例的宏。Test Fixture 是在类中定义的一个共享环境，而 `TEST_F` 允许在该环境中定义多个测试。

`TEST_F` 是 **Fixture 版本** 的测试宏，它在测试执行之前会自动调用 `SetUp()` 函数，在测试结束后会自动调用 `TearDown()` 函数。

- `TEST_F(AddTest, cast1)` 定义了一个名为 `cast1` 的测试用例，它使用 `AddTest` 作为测试的环境（Test Fixture）。也就是说，测试用例 `cast1` 会在 `AddTest` 类中定义的环境中运行。
- `TEST_F(AddTest, cast2)` 定义了另一个名为 `cast2` 的测试用例，它同样使用 `AddTest` 类作为 Test Fixture。
- 注意，测试用例生成的函数名都叫`TestBody`，完整为`AddTest_case1_Test::TestBody()`，可以在代码的测试用例中加入`__FUNCTION__`打印查看一下

注意类中各个函数执行的顺序为：先执行构造函数，再执行`SetUp`，执行对应的测试用例，之后执行`TreaDown`，最后执行析构，具体的代码执行可以见下面

与之前的参数化进行对比：

| 特性             | `TEST_F()`                                                | `TEST_P()`                                                |
| ---------------- | --------------------------------------------------------- | --------------------------------------------------------- |
| **目的**         | 用于测试夹具，设置共享的测试环境                          | 用于参数化测试，针对不同的输入执行相同的测试              |
| **测试方式**     | 每个测试用例独立执行，使用相同的测试夹具状态              | 每个参数组合都会生成一个独立的测试实例                    |
| **初始化与清理** | 可以使用 `SetUp()` 和 `TearDown()` 进行共享的初始化和清理 | 可以通过 `SetUp()` 使用 `GetParam()` 获取参数并进行初始化 |
| **参数**         | 无参数支持                                                | 支持为测试用例提供多组参数                                |
| **用法示例**     | `TEST_F(FixtureClass, TestName)`                          | `TEST_P(FixtureClass, TestName)`                          |

在当前的工程下面，有`add.h`和`add.cpp`分别表示功能函数的声明和实现，以及使用功能函数的`main.cpp`，同时还有一个用于测试功能函数的`test.cpp`

注意，这里`main.cpp`不同于之前的工程，里面是使用`add`函数的逻辑，对于这种情况，可以当做没有测试用的`main.cpp`来处理，因为它没有Google Test 的入口），所以需要在 `test.cpp` 中包含 `main` 函数并初始化 Google Test。

```cpp
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

本次的目录结构更加贴进于生产环境中真实的工程目录（区别在于一般`test.cpp`是放在test目录下的），因此`CMakeList.txt`也需要进行相应的修改

```cmake
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
```

在上面的cmake中，手动设置的库文件和头文件的路径，从而让CMake能够找到对应的文件

```cmake
set(GTEST_INCLUDE_DIR /path/to/gtest/include)
# 头文件路径应该指向包含 gtest/gtest.h 的目录
set(GTEST_LIB_DIR /path/to/gtest/lib)
# 库文件路径应该指向包含编译好的 libgtest.a（静态库）或 libgtest.so（动态库）等文件的目录
```

> 对比之前的``find_package()``
>
> | **特性**                 | **`find_package(GTest REQUIRED)`**                           | **手动设置路径（`set(GTEST_INCLUDE_DIR /path/to/gtest/include)`)** |
> | ------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
> | **是否需要手动设置路径** | 不需要，CMake 会自动找到已安装的 Google Test（前提是已经安装并且有配置文件）。 | 需要手动设置 Google Test 的头文件和库文件路径。              |
> | **适用场景**             | Google Test 已经通过包管理器或其他方式安装，且 CMake 配置正确。 | 手动下载并编译的 Google Test，或者系统中没有预安装 Google Test。 |
> | **简洁性**               | 更简洁，只需要一行 `find_package(GTest REQUIRED)`。          | 需要手动指定路径，并且可能需要手动配置其他依赖项。           |
> | **灵活性**               | 自动处理安装路径，但需要 Google Test 通过 CMake 安装或包管理器安装。 | 更灵活，适用于不依赖标准安装路径的场景。                     |
> | **常见用法**             | 推荐使用，尤其是 Google Test 已经安装在系统中，且使用 CMake 安装或包管理器安装。 | 如果你手动管理 Google Test 或在非标准路径下安装 Google Test，才需要使用。 |

之后，在使用**`target_include_directories()`**：通过 `target_include_directories()` 将 Google Test 的头文件路径加入到编译器的搜索路径中。即**为 `test_add` 目标添加包含目录**，同时告诉 CMake，`test_add` 目标将使用 `${GTEST_INCLUDE_DIR}`（即 Google Test 的头文件目录）。这个包含目录只对 `test_add` 目标有效，不会影响链接到 `test_add` 的其他目标。

- `target_include_directories` 命令用于为特定目标（如可执行文件或库）设置包含目录

- `PRIVATE` 是用来指定访问权限的关键字之一，它表示所设置的包含目录仅对目标本身有效，而不对依赖该目标的其他目标有效，其他链接到该目标的目标不会看到这些目录

    也就是说，如果 `test_add` 是一个可执行文件或库，并且你通过 `PRIVATE` 为它指定了包含目录，那么这些目录只对 `test_add` 本身有效，其他链接到 `test_add` 的目标（如其他可执行文件或库）不会受到影响

    > 除了 `PRIVATE` 之外，还有两个常见的关键字：
    >
    > - **`PUBLIC`**：该目录不仅对目标本身有效，而且对所有链接到该目标的其他目标也有效。
    > - **`INTERFACE`**：该目录仅对链接到该目标的其他目标有效，但不影响目标本身。

- `CMAKE_SOURCE_DIR` 是 CMake 自动定义的一个变量，表示 **项目的根目录**，也是 CMake 在项目根目录运行时的工作目录，即 CMake 配置文件（`CMakeLists.txt`）所在的目录。

    通常它是包含你项目的 `CMakeLists.txt` 文件的目录。你可以通过它来引用项目的根目录下的文件和子目录。

最后两行是CMake 用于配置和运行测试的命令

1. `enable_testing()` 是一个启用测试的 CMake 命令，用来启用 CTest 测试框架。CMake 本身并不会默认启用测试支持，除非显式调用 `enable_testing()`。

    - **作用**：调用 `enable_testing()` 后，CMake 将会配置并启用测试功能，使得你能够使用 `add_test()` 和 `ctest` 命令来定义和运行测试。
    - **调用时机**：通常这个命令会放在 `CMakeLists.txt` 的顶部或测试配置部分。

    通过调用 `enable_testing()`，CMake 会创建一些与测试相关的功能和文件，供你后续使用，如 `ctest` 命令（这是 CMake 提供的一个测试运行工具）

2. `add_test()` 是 CMake 用来添加测试的命令，它告诉 CMake 运行一个测试命令，并为其定义名称和运行方式。

    - **`NAME test_add`**：这个参数指定了测试的名称，`test_add` 是测试的名称。在 CTest 中执行时，测试会通过这个名字进行标识。
    - **`COMMAND test_add`**：这个参数指定了测试运行时要执行的命令。在这种情况下，`test_add` 是你之前定义的目标（也就是可执行文件）。CMake 会运行 `test_add` 可执行文件，假设它包含了 Google Test 的测试逻辑。

完整的编译运行以及测试流程为：

```bash
03.test$ mkdir build && cd build
03.test/build$ cmake ..
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
-- Configuring done
-- Generating done
-- Build files have been written to: /home/will/lesson/gTest/03.test/build
will@will-vm:~/lesson/gTest/03.test/build$ make # 构建项目
[ 33%] Building CXX object CMakeFiles/test_add.dir/add.cpp.o
[ 66%] Building CXX object CMakeFiles/test_add.dir/test.cpp.o
[100%] Linking CXX executable test_add
[100%] Built target test_add

# 构建项目后，可以使用make test 或 ctest来运行所有测试，也可以使用./test_add运行项目
03.test/build$ ./test_add 
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from AddTest
[ RUN      ] AddTest.cast1
AddTest
SetUp
case1: 
TearDown
~AddTest
[       OK ] AddTest.cast1 (0 ms)
[ RUN      ] AddTest.cast2
AddTest
SetUp
case2: 
TearDown
~AddTest
[       OK ] AddTest.cast2 (0 ms)
[----------] 2 tests from AddTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.

# 运行测试，这是makefile中的测试功能
# make test 和 ctest 效果一样
03.test/build$ make test
Running tests...
Test project /home/will/lesson/gTest/03.test/build
    Start 1: test_add
1/1 Test #1: test_add .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.00 sec
03.test/build$ ctest
Test project /home/will/lesson/gTest/03.test/build
    Start 1: test_add
1/1 Test #1: test_add .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.00 sec
```



### 四、测试宏

`TYPED_TEST` 是一个用于创建类型化测试的宏，它允许你针对不同的数据类型写一组共同的测试。通过 `TYPED_TEST`，你可以定义一个测试模板，这个模板可以在多个不同类型上执行同样的测试，而不需要为每个类型重复编写测试代码。它主要用于测试模板类或者模板函数时，自动生成针对不同类型的测试。

如果用例在多个类型下，流程是相同的，那么就可以使用`TYPEED_TEST`；它和类型是没有关系的，多种类型的流程表现都是一致的

它的基本使用方法：首先需要定义一个模板类，并且让这个模板类继承`testing`命名空间中的`Test`类；然后让对这个模板类进行实例化，为了方便使用可以使用`using`给需要测试实例化的类型起别名，燃烧使用`TYPED_TEST_SUITE`宏对模板类型进行实例化；然后使用`TYPED_TEST`来写用例

```cpp
#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

template <typename T>
// 声明了一个模板类 TypeTestClass，其中 T 是类型参数
class TypeTestClass : public ::testing::Test {};
// 这个类为测试提供了基础设施，使得你可以在测试中使用 Google Test 提供的功能，如断言、SetUp 和 TearDown 等。这个类本身并不包含任何测试逻辑，只是作为一个类型化测试的基类

using MyTypes = ::testing::Types<int, double, float>;
// 这是一个类型别名
// ::testing::Types<int, double, float> 是一个包含多个类型的类型列表，这里包含了 int、double 和 float 类型
// MyTypes 代表一个类型集合，用于在后续的 TYPED_TEST 中测试这几个类型
TYPED_TEST_SUITE(TypeTestClass, MyTypes);
// 这是一个宏，表示 TypeTestClass 是一个实例化测试类，并且 MyTypes 是类型列表
// 这个宏告诉 Google Test，后续的测试会针对 MyTypes 中定义的类型依次执行

TYPED_TEST(TypeTestClass, TTest) {
    TypeParam x = 1;
    TypeParam y = 2;
    TypeParam z = x + y;
    EXPECT_EQ(z, 3);
}
```

- **`TYPED_TEST(TypeTestClass, TTest)`**：这是定义一个实例化测试的宏。`TypeTestClass` 是继承自 `::testing::Test` 的测试基类，`TTest` 是测试的名称。在这里，`TTest` 测试会针对 `MyTypes` 中的每个类型（`int`、`double`、`float`）分别执行一次。
- **`TypeParam`**：`TypeParam` 是一个内置的类型别名，代表当前正在测试的类型。在第一次执行时，`TypeParam` 为 `int`，第二次为 `double`，第三次为 `float`。它会根据 `TYPED_TEST` 测试的类型参数进行替换。

为了更好的理解`TypeParam`，还写了另一个测试文件

```cpp
#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

struct A{
    char a;
    short b;
};

template <typename T>
class MyTypeTest : public ::testing::Test {};

using MyType = ::testing::Types<int, unsigned int, int32_t, A>;
TYPED_TEST_SUITE(MyTypeTest, MyType);

TYPED_TEST(MyTypeTest, SizeTest) {
    EXPECT_EQ(sizeof(TypeParam), 4);
}
```

因此，在项目目录下，有2个源文件，需要对这两个源文件都生成对应的可执行文件，以下是CMakeList.txt

```cmake
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
```

生成构建文件（`mkdir build && cd build`，`cmake ..`）

构建项目（`make`），即使用 `make` 来编译项目，会生成2个可执行文件

接下来，可以分别运行这两个可执行文件来查看效果

```bash
$ ./type_test 
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 3 tests from 3 test suites.
[----------] Global test environment set-up.
[----------] 1 test from TypeTestClass/0, where TypeParam = int
[ RUN      ] TypeTestClass/0.TTest
[       OK ] TypeTestClass/0.TTest (0 ms)
[----------] 1 test from TypeTestClass/0 (0 ms total)

[----------] 1 test from TypeTestClass/1, where TypeParam = double
[ RUN      ] TypeTestClass/1.TTest
[       OK ] TypeTestClass/1.TTest (0 ms)
[----------] 1 test from TypeTestClass/1 (0 ms total)

[----------] 1 test from TypeTestClass/2, where TypeParam = float
[ RUN      ] TypeTestClass/2.TTest
[       OK ] TypeTestClass/2.TTest (0 ms)
[----------] 1 test from TypeTestClass/2 (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 3 test suites ran. (0 ms total)
[  PASSED  ] 3 tests.

$ ./type_struct 
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 4 tests from 4 test suites.
[----------] Global test environment set-up.
[----------] 1 test from MyTypeTest/0, where TypeParam = int
[ RUN      ] MyTypeTest/0.SizeTest
[       OK ] MyTypeTest/0.SizeTest (0 ms)
[----------] 1 test from MyTypeTest/0 (0 ms total)

[----------] 1 test from MyTypeTest/1, where TypeParam = unsigned int
[ RUN      ] MyTypeTest/1.SizeTest
[       OK ] MyTypeTest/1.SizeTest (0 ms)
[----------] 1 test from MyTypeTest/1 (0 ms total)

[----------] 1 test from MyTypeTest/2, where TypeParam = int
[ RUN      ] MyTypeTest/2.SizeTest
[       OK ] MyTypeTest/2.SizeTest (0 ms)
[----------] 1 test from MyTypeTest/2 (0 ms total)

[----------] 1 test from MyTypeTest/3, where TypeParam = A
[ RUN      ] MyTypeTest/3.SizeTest
[       OK ] MyTypeTest/3.SizeTest (0 ms)
[----------] 1 test from MyTypeTest/3 (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 4 test suites ran. (0 ms total)
[  PASSED  ] 4 tests.
```

可以看到，分别测试了`::testing::Types`中不同类型的用例

当然，在CMakeList.txt中，使用 `enable_testing()` 和 `add_test()` 定义测试后，仍然可以使用make中的测试功能，可以使用`make test`或者`ctest`命令，他们会直接测试这两个可执行文件

如果只想运行某个特定的测试，可以使用 `-R` 参数来匹配测试名称（即`NAME`后定义的）

```bash
ctest -R TypeTest
ctest -R StructTest
```

运行测试

```bash
$ make test
Running tests...
Test project /home/will/lesson/gTest/04.test/build
    Start 1: TypeTest
1/2 Test #1: TypeTest .........................   Passed    0.00 sec
    Start 2: StructTest
2/2 Test #2: StructTest .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.01 sec

$ ctest -R TypeTest
Test project /home/will/lesson/gTest/04.test/build
    Start 1: TypeTest
1/1 Test #1: TypeTest .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.00 sec

$ ctest -R StructTest
Test project /home/will/lesson/gTest/04.test/build
    Start 2: StructTest
1/1 Test #2: StructTest .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.00 sec
```



