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



### 四、类型测试

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



### 五、类型参数化测试

顾名思义，就是将之前讲过的的参数化测试和类型测试结合起来

在 Google Test 中，**类型参数化测试**（Type-Parameterized Tests）是一种通过使用模板类型参数来进行测试的技术。它允许你编写同一测试逻辑，但针对不同的数据类型或类进行测试，从而避免重复代码，提高测试的复用性和覆盖面。

可以定义一个测试用例模板，该模板针对一个类型列表执行测试，这样可以在一个测试中对多个类型进行验证，而无需为每个类型写独立的测试用例。

场景：假设有两个模板函数，需要对这两个函数进行测试，分别使用不同的数据类型。如果只用类型测试，则需要进行两次实例化，而结合参数化测试，则只需要实例化一次即可

即，对于多个测试需要的类型场景完全一样，就可以把类型进行参数化，只定义一次类型参数实例化的过程，就可以将多个类型分别应用于这多个测试用例

```cpp
#include "func.h"
#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

// 1. 定义模板类
template <typename T>
class TypedTestClass : public ::testing::Test{};

// 2. 定义参数化测试套件
TYPED_TEST_SUITE_P(TypedTestClass);
// 定义了一个类型参数化的测试套件,让测试套件支持参数化, TypedTestClass 是测试夹具类的名称

// 3. 测试用例场景
TYPED_TEST_P(TypedTestClass, AddTest) {
    TypeParam a = 1;
    TypeParam b = 2;
    EXPECT_EQ(a + b, 3);
}
// 这两个个测试将会根据不同类型的 TypeParam 进行多次实例化
TYPED_TEST_P(TypedTestClass, SubTest) {
    TypeParam a = 6;
    TypeParam b = 2;
    EXPECT_EQ(a - b, 4);
}

// 4. 注册测试
REGISTER_TYPED_TEST_SUITE_P(TypedTestClass, AddTest, SubTest);
// 注册了测试用例 AddTest 和 SubTest，并将它们与 TypedTestClass 测试夹具关联起来

// 5. 实例化
using MyTypeClass = ::testing::Types<int, float, long, double>;
// 实例化了 TypedTestClass 测试夹具
INSTANTIATE_TYPED_TEST_SUITE_P(MyPrefix, TypedTestClass, MyTypeClass);
// 使用 MyTypeClass 中定义的类型列表来创建不同类型的测试用例实例
```

其中，模板类 `TypedTestClass`，它继承自 `::testing::Test`，这是所有 Google Test 测试夹具（Fixture）类的基类。同时，通过模板类型 `T`，可以根据不同的数据类型创建多个测试实例

`TYPED_TEST_SUITE_P` 宏用于标记一个测试套件，表示该套件是针对不同类型的参数进行测试的。

`TYPED_TEST_P` 宏定义了一个类型参数化的测试用例，其参数分别是`TypedTestClass`的测试夹具，以及`AddTest`的测试用例。在测试用例中，测试夹具`TypedTestClass`的模板参数 `TypeParam` 表示当前测试实例的类型

在使用`REGISTER_TYPED_TEST_SUITE_P`将测试用例与测试夹具关联后，Google Test 会为每个类型参数生成这些测试用例的实例

`::testing::Types` 是一个模板类，它允许我们指定一个类型列表。Google Test 将为这些类型生成相应的测试用例实例。

`INSTANTIATE_TYPED_TEST_SUITE_P` 使得测试框架为指定类型生成相应的测试用例。其中，`MyPrefix` 是测试套件实例的前缀，它将用于生成测试套件的名称。在上面的代码中，具体的测试名称会是 `MyPrefix<int>`、`MyPrefix<float>` 等

PS：由于没有写google test的入口函数，记得写`main()`函数

运行后可以看到，两个用例都测试了实例化的多个类型，进行了类型和用例正交的测试

```bash
$ ./type_test 
[==========] Running 8 tests from 4 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from MyPrefix/TypedTestClass/0, where TypeParam = int
[ RUN      ] MyPrefix/TypedTestClass/0.AddTest
[       OK ] MyPrefix/TypedTestClass/0.AddTest (0 ms)
[ RUN      ] MyPrefix/TypedTestClass/0.SubTest
[       OK ] MyPrefix/TypedTestClass/0.SubTest (0 ms)
[----------] 2 tests from MyPrefix/TypedTestClass/0 (0 ms total)

[----------] 2 tests from MyPrefix/TypedTestClass/1, where TypeParam = float
[ RUN      ] MyPrefix/TypedTestClass/1.AddTest
[       OK ] MyPrefix/TypedTestClass/1.AddTest (0 ms)
[ RUN      ] MyPrefix/TypedTestClass/1.SubTest
[       OK ] MyPrefix/TypedTestClass/1.SubTest (0 ms)
[----------] 2 tests from MyPrefix/TypedTestClass/1 (0 ms total)

[----------] 2 tests from MyPrefix/TypedTestClass/2, where TypeParam = long
[ RUN      ] MyPrefix/TypedTestClass/2.AddTest
[       OK ] MyPrefix/TypedTestClass/2.AddTest (0 ms)
[ RUN      ] MyPrefix/TypedTestClass/2.SubTest
[       OK ] MyPrefix/TypedTestClass/2.SubTest (0 ms)
[----------] 2 tests from MyPrefix/TypedTestClass/2 (0 ms total)

[----------] 2 tests from MyPrefix/TypedTestClass/3, where TypeParam = double
[ RUN      ] MyPrefix/TypedTestClass/3.AddTest
[       OK ] MyPrefix/TypedTestClass/3.AddTest (0 ms)
[ RUN      ] MyPrefix/TypedTestClass/3.SubTest
[       OK ] MyPrefix/TypedTestClass/3.SubTest (0 ms)
[----------] 2 tests from MyPrefix/TypedTestClass/3 (0 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 4 test suites ran. (0 ms total)
[  PASSED  ] 8 tests.

$ ctest
Test project /home/will/lesson/gTest/05.test/build
    Start 1: TypeTest
1/1 Test #1: TypeTest .........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

当不同的测试用例需要使用相同的类型进行实例化的时候，就可以使用类型参数化测试



### 六、测试跟踪

在执行测试时，希望在失败的时候，输出更多的信息。此时，就可以在测试用例中插入`SCOPED_TRACE`宏，更像是log，会把当前的文件行号都打印出来，还可以添加自定义信息，从而更好的分析错误

程序流程：

1. **定义 `add` 函数**：该函数实现了加法功能。
2. **定义参数化测试类**：`TestAdd` 类从 `::testing::TestWithParam<std::tuple<int, int, int>>` 派生，表示每个测试用例将接受一个元组作为输入。
3. **定义参数化测试用例 `Add`**：使用 `TEST_P` 宏定义了测试用例 `Add`，它从 `TestAdd` 类继承，获取测试参数，并使用 `ASSERT_EQ` 进行加法结果验证。
4. **实例化测试用例**：使用 `INSTANTIATE_TEST_SUITE_P` 定义了不同的测试数据，创建多个不同的测试实例。
5. **运行测试**：在 `main` 函数中，调用 `RUN_ALL_TESTS()` 执行所有测试，输出测试结果。

```cpp
#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>
#include <iostream>
#include <string>
#include <tuple>

int add(int a, int b) {
    return a + b;
}

class TestAdd : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

TEST_P(TestAdd, Add) {
    auto param = GetParam();
    SCOPED_TRACE("add(" + std::to_string(std::get<0>(param)) +
                 ", " + std::to_string(std::get<1>(param)) + 
                 ") = " + std::to_string(std::get<2>(param)));
    ASSERT_EQ(add(std::get<0>(param), std::get<1>(param)), std::get<2>(param));
}

INSTANTIATE_TEST_SUITE_P(Add, TestAdd, ::testing::Values(
                        std::make_tuple(1, 2, 3),
                        std::make_tuple(2, 3, 5),
                        std::make_tuple(4, 9, 12),
                        std::make_tuple(4, 9, 13)));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

`TestWithParam`是 Google Test 提供的一个模板类，它允许你为每个测试提供一组参数。在这里，`TestAdd`继承自`TestWithParam<std::tuple<int, int, int>>`，表示每个测试将接受一个包含 3 个整数的`tuple`类型的参数。

- 第一个整数：`a`，即加法运算的第一个数。
- 第二个整数：`b`，即加法运算的第二个数。
- 第三个整数：`expected result`，即 `a + b` 的预期结果。

`TEST_P` 是 Google Test 中用于定义参数化测试的宏。它定义了一个名为 `Add` 的测试用例，依赖于 `TestAdd` 类。

`GetParam()` 是 `TestWithParam` 类提供的方法，它返回当前测试的参数。在这个例子中，返回的参数是一个 `std::tuple<int, int, int>` 类型的元组。

`SCOPED_TRACE` 用于为每个测试添加额外的调试信息，它会在测试失败时显示详细的错误信息，帮助开发者理解具体的参数值。例如，输出类似 `"add(1, 2) = 3"`。

- `std::get<0>(param)` 获取元组的第一个元素（即加法的第一个参数 `a`）。

`INSTANTIATE_TEST_SUITE_P` 用于实例化测试套件

- 第一个参数 `Add` 是实例化后的测试套件的名称。
- 第二个参数 `TestAdd` 是要实例化的测试类。
- 第三个参数是`::testing::Values`，它定义了测试用例的输入值。这里使用了`std::make_tuple`来创建一个`std::tuple<int, int, int>`类型的参数列表。
    - 每个 `std::make_tuple(a, b, expected_result)` 都是一个测试实例，其中 `a` 和 `b` 是加数，`expected_result` 是期望的加法结果。
    - 例如，`std::make_tuple(1, 2, 3)` 表示对 `1 + 2` 进行测试，期望结果是 `3`。
    - 最后一组 `std::make_tuple(4, 9, 13)` 显然是一个错误的测试用例，因为 `4 + 9` 应该等于 `13`，但期望结果是 `13`。这意味着这个测试会失败，可能是为了测试断言失败的情况。

执行结果如下，比起默认的失败信息，还给出了文件名、错误的行号以及对应的测试数据

```bash
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from Add/TestAdd
[ RUN      ] Add/TestAdd.Add/0
[       OK ] Add/TestAdd.Add/0 (0 ms)
[ RUN      ] Add/TestAdd.Add/1
[       OK ] Add/TestAdd.Add/1 (0 ms)
[ RUN      ] Add/TestAdd.Add/2
/home/will/lesson/gTest/06.test/scoped_trace.cpp:18: Failure
Expected equality of these values:
  add(std::get<0>(param), std::get<1>(param))
    Which is: 13
  std::get<2>(param)
    Which is: 12
Google Test trace:
/home/will/lesson/gTest/06.test/scoped_trace.cpp:15: add(4, 9) = 12

[  FAILED  ] Add/TestAdd.Add/2, where GetParam() = (4, 9, 12) (0 ms)
[ RUN      ] Add/TestAdd.Add/3
[       OK ] Add/TestAdd.Add/3 (0 ms)
[----------] 4 tests from Add/TestAdd (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 3 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] Add/TestAdd.Add/2, where GetParam() = (4, 9, 12)

 1 FAILED TEST
```



### 七、友元测试

如果要测试的类中，需要测试的值是私有变量，那么在测试用例中就无法访问。而对于C++来说，要访问一个类的私有变量，就需要设置友元类或者友元函数



```cpp
// Circle.h
class Circle
{
public:
    Circle(int r)
    : _r(r)
    , _area(r * r * 3.14)
    {}

    friend class CircleTest; // 声明 CircleTest 类为 Circle 类的友元类
    FRIEND_TEST(CircleTest, Area);

private:
    int _r;
    double _area;
};

// friend_test.cpp
TEST(CircleTest, Area) {
    Circle c(5);
    EXPECT_EQ(c._area, 78.5);
}

```

`FRIEND_TEST`是 Google Test 框架中的宏，它的作用是声明特定的测试类的测试案例能够访问目标类的私有成员。

即它声明 `CircleTest` 测试类的 `Area` 测试函数可以访问 `Circle` 类的私有成员。使用这个宏后，`CircleTest` 中的测试函数就可以访问 `Circle` 类的 `_r` 和 `_area` 等私有成员。

具体来说，`FRIEND_TEST(CircleTest, Area)` 会使 `CircleTest` 中的 `Area` 测试用例类（自动生成的）成为 `Circle` 类的友元类。这个自动生成的测试用例类通常名为 `CircleTest_Area_Test`，其中 `Area` 是测试用例的名称。

- `FRIEND_TEST` 宏实际上是让 `CircleTest_Area_Test` 这个自动生成的测试类访问 `Circle` 的私有成员。这是因为 `TEST(CircleTest, Area)` 会生成一个名为 `CircleTest_Area_Test` 的类。

> 这行如果写成`friend class CircleTest_Area_Test;`也会有同样的效果。
>
> `friend class CircleTest_Area_Test;` 直接声明 `CircleTest_Area_Test` 为 `Circle` 类的友元类。这样，无论 `CircleTest_Area_Test` 是否由 `TEST` 宏生成，它都可以访问 `Circle` 类的私有成员。
>
> > - **`FRIEND_TEST(CircleTest, Area)`** 是与 Google Test 结合使用的宏，它会自动处理测试用例类的名称，并且与 `TEST` 宏产生的测试用例类名称紧密绑定。在 Google Test 框架下，这种方式是推荐的做法，它简化了友元类的声明，并且明确与测试相关。
> > - **`friend class CircleTest_Area_Test;`** 是显式声明一个类为友元类的方法，但它要求你知道并显式声明 `CircleTest_Area_Test` 类。如果你修改了测试名称或其他结构，必须手动调整这个声明。而使用 `FRIEND_TEST` 宏可以更灵活，且不需要关注测试类名称的变化。

`TEST(CircleTest, Area)` 使用 Google Test 框架的 `TEST` 宏定义了一个测试案例。`CircleTest` 是测试夹具名称，`Area` 是测试案例名称。

```bash
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from CircleTest
[ RUN      ] CircleTest.Area
[       OK ] CircleTest.Area (0 ms)
[----------] 1 test from CircleTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```



### 八、跳过测试

有时写了测试用例，但是还不完善，经常失败，想要临时的不执行，只执行其他的用例。基于这种情况，就可以在测试用例前面，使用`GTEST_SKIP()`来进行跳过，而且还可以打印出一些描述性的信息。

- `GTEST_SKIP()` 用于跳过当前的测试案例。它会使当前测试不再执行，直接标记为“跳过”，并不会影响后续的测试。
- `GTEST_SKIP()` 常用于在某些特定条件下，跳过测试的执行。例如，测试依赖某些外部环境或条件，当条件不满足时，可以使用 `GTEST_SKIP()` 来跳过该测试。

这是一种用于控制测试执行流程的宏。类似的还有`GTEST_FAIL()`，它会直接导致测试用例执行失败，而不会继续往后执行

- `GTEST_FAIL()` 用于强制使当前的测试失败。它会立即标记当前测试为失败，并跳过后续的断言和执行。
- `GTEST_FAIL()` 常用于在某些特殊情况下，提前标记测试失败。例如，当某些重要的前提条件无法满足时，可以直接调用 `GTEST_FAIL()` 来表示当前测试不应该继续执行。

同理，还有`GTEST_SUCCEED()`强制当前用例正确，还需继续执行剩余的用例，用处不大

| 特性                     | `GTEST_SKIP()`                     | `GTEST_FAIL()`                   |
| ------------------------ | ---------------------------------- | -------------------------------- |
| **作用**                 | 跳过当前测试，测试不再执行         | 强制测试失败，后续的断言不再执行 |
| **测试结果**             | 标记为跳过（SKIPPED）              | 标记为失败（FAILED）             |
| **是否继续执行后续断言** | 是，跳过当前测试后的代码仍会被执行 | 否，测试失败后不会执行后续代码   |
| **常见用途**             | 某些条件不满足时跳过测试           | 某些前提条件不满足时提前失败测试 |

```cpp
#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(SkipTest, case1) {
    EXPECT_EQ(2,  add(1, 1));
}

TEST(SkipTest, case2) {
    GTEST_SKIP() << "Skipping this test";
    EXPECT_EQ(4,  add(2, 3));
}

TEST(SkipTest, case3) {
    GTEST_FAIL() << "Failed this test && end test";
    EXPECT_EQ(5,  add(2, 3));
}

TEST(SkipTest, case4) {
    EXPECT_EQ(4,  add(2, 2));
}
```

注意，在上面的代码中，`case4` 是另外一个独立的测试用例，它会正常执行，无论 `case3` 之前是否调用了 `GTEST_FAIL()`。每个 `TEST` 用例都有自己的独立执行上下文和生命周期，所以 `case4` 会照常执行，除非它本身遇到某些错误。

在 `case3` 中，`GTEST_FAIL()` 被调用后，当前测试就会失败并停止后续的断言执行，但它不会停止整个测试文件中其他测试的执行。也就是说，虽然 `case3` 会失败，但这并不影响其他测试用例（如 `case4`）的运行。

> 另一个需要注意的点就是，==在上面的代码中，并没有google test的`main`函数==，但是依然能够正常运行，并生成可执行文件
>
> 这是因为 Google Test 框架自动处理了主函数的调用。在使用 Google Test 时，通常不需要手动编写 `main` 函数。Google Test 提供了一个默认的 `main` 函数实现，它会在底层自动初始化测试框架并运行所有测试用例。这个默认的 `main` 函数就是你不写 `main` 时，Google Test 会自动提供的

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from SkipTest
[ RUN      ] SkipTest.case1
[       OK ] SkipTest.case1 (0 ms)
[ RUN      ] SkipTest.case2
/home/will/lesson/gTest/08.skip/skip_test.cpp:12: Skipped
Skipping this test

[  SKIPPED ] SkipTest.case2 (0 ms)
[ RUN      ] SkipTest.case3
/home/will/lesson/gTest/08.skip/skip_test.cpp:17: Failure
Failed
Failed this test && end test

[  FAILED  ] SkipTest.case3 (0 ms)
[ RUN      ] SkipTest.case4
[       OK ] SkipTest.case4 (0 ms)
[----------] 4 tests from SkipTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
[  SKIPPED ] 1 test, listed below:
[  SKIPPED ] SkipTest.case2
[  FAILED  ] 1 test, listed below:
[  FAILED  ] SkipTest.case3

 1 FAILED TEST
```

不使用上面的`GTEST_SKIP()`也可以跳过测试用例

在运行可执行文件时，添加`--gtest_filter`，也可以灵活的控制测试用例的执行

```bash
$ ./skip_test --gtest_filter='SkipTest.case1'
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
Note: Google Test filter = SkipTest.case1
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from SkipTest
[ RUN      ] SkipTest.case1
[       OK ] SkipTest.case1 (0 ms)
[----------] 1 test from SkipTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.

$ ./skip_test --gtest_filter='SkipTest.case*-SkipTest.case3'
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
Note: Google Test filter = SkipTest.case*-SkipTest.case3
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from SkipTest
[ RUN      ] SkipTest.case1
[       OK ] SkipTest.case1 (0 ms)
[ RUN      ] SkipTest.case2
/home/will/lesson/gTest/08.skip/skip_test.cpp:12: Skipped
Skipping this test

[  SKIPPED ] SkipTest.case2 (0 ms)
[ RUN      ] SkipTest.case4
[       OK ] SkipTest.case4 (0 ms)
[----------] 3 tests from SkipTest (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
[  SKIPPED ] 1 test, listed below:
[  SKIPPED ] SkipTest.case2
```

其中`*`表示运行所有前缀为`case`的用例，`-`表示不执行后面的测试用例，当然前面省略，直接从`-`开始也是可以的，因为默认就是全集

从执行的结果来看，这种是直接不执行，即不纳入执行的测试用例的队列中，而前面的跳过则是纳入执行的队列中，当执行到该用例的时候进行跳过

另外，可以通过`./可执行程序 --help`来查看控制其行为的参数



### 九、广义断言

相较于之前用过的`EXPECT_EQ`、`EXXPECT_NE`、`EXPECT_LS`等，google test还支持更多的方式，诸如字符串的匹配、正则匹配，以及数字范围的判断

`EXPECT_THAT`：用于执行期望检查，判断 `Hello()` 函数返回的值是否符合预期。

- `::testing::StartsWith("Hello World")`：这是 Google Test 提供的一个匹配器，检查返回的字符串是否以 `"Hello World"` 开头。即使随机数部分不同，只要返回的字符串前缀是 `"Hello World"`，这个测试就会通过。
- `::testing::AllOf(::testing::Ge(0), ::testing::Lt(10))`：这是一个组合匹配器，表示返回的值应该同时满足以下两个条件：
    - `::testing::Ge(0)`：大于 0。
    - `::testing::Lt(10)`：小于 10。

```cpp
std::string Hello() {
    return "Hello World" + std::to_string(rand());
}

int Rand10() {
    return rand() % 10;
}

TEST(GeneralizedAssert, Hello) {
    EXPECT_THAT(Hello(), ::testing::StartsWith("Hello World"));
}

TEST(GeneralizedAssert, Rand10) {
    EXPECT_THAT(Rand10(), ::testing::AllOf(::testing::Gt(0), ::testing::Lt(10)));
}
```

在上面的代码中，`Hello()`函数的返回值中含有随机数，难以像之前的`EQ`一样准确的设置期望值，所以使用`EXPECT_THAT`后面跟一个匹配器（GTest有非常多的匹配器）

因为这里涉及到随机数，可以使用上一章的命令，多次运行测试用例

```bash
$ ./general_test --gtest_repeat=100
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc

Repeating all tests (iteration 1) . . .

[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from GeneralizedAssert
[ RUN      ] GeneralizedAssert.Hello
[       OK ] GeneralizedAssert.Hello (0 ms)
[ RUN      ] GeneralizedAssert.Rand10
[       OK ] GeneralizedAssert.Rand10 (0 ms)
[----------] 2 tests from GeneralizedAssert (0 ms total)

[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.


Repeating all tests (iteration 99) . . .

[==========] Running 2 tests from 1 test suite.
[----------] 2 tests from GeneralizedAssert
[ RUN      ] GeneralizedAssert.Hello
[       OK ] GeneralizedAssert.Hello (0 ms)
[ RUN      ] GeneralizedAssert.Rand10
[       OK ] GeneralizedAssert.Rand10 (0 ms)
[----------] 2 tests from GeneralizedAssert (0 ms total)

[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.

Repeating all tests (iteration 100) . . .

[==========] Running 2 tests from 1 test suite.
[----------] 2 tests from GeneralizedAssert
[ RUN      ] GeneralizedAssert.Hello
[       OK ] GeneralizedAssert.Hello (0 ms)
[ RUN      ] GeneralizedAssert.Rand10
[       OK ] GeneralizedAssert.Rand10 (0 ms)
[----------] 2 tests from GeneralizedAssert (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

更多的匹配器，可以查看官方文档中https://google.github.io/googletest/reference/matchers.html，里面介绍的匹配器
