## Google Mock

### 一、介绍

`gmock`是gtest中的一个重要组件

假设，应用程序中，有一个数据库，在访问数据库的过程中，可能会遇到数据库的一些错误，比如连接断开、返回数据非预期等异常情况，而这些场景，在真实的生产场景中是比较难构造的

此时，就可以使用`gmock`来模拟出以上的场景

==**`gmock`的作用：模拟一个接口的实现**==

```cpp
class File 
{
public:
    virtual int open(const char *name) = 0;
    virtual int close() = 0;
    virtual int read(char *buf, size_t size) = 0;
    virtual int write(const char *buf, size_t size) = 0;
};
```

`File`这是一个纯虚类（接口类），定义了文件操作的接口：`open`、`close`、`read` 和 `write`。它是一个抽象基类，不能直接实例化，主要用于定义接口规范，供派生类实现。

```cpp
class Logger
{
public:
    Logger(File *f)
    : _file(f)
    {
        std::cout << "Logger(File *) \n";
    }

    bool init()
    {
        return _file->open("log.txt") == 0;
    }

    bool write(const char *buf, size_t size)
    {
        return _file->write(buf, size) == size;
    }

private:
    File *_file = nullptr;
};
```

`Logger` 使用了一个 `File` 接口的实现（通过依赖注入）

- **构造函数**接收一个 `File*` 对象（可能是具体实现，也可能是一个 Mock 对象）。
- 功能
    - `init()` 方法尝试打开 `log.txt` 文件，返回操作是否成功。
    - `write()` 方法写入数据并返回操作是否成功。

现在需要对`Logger`进行测试，（并不是对`File`进行测试，接口一般是不需要测试的，只有实现才需要测试），它是具体的逻辑实现，在构造的时候，将`file`对象通过依赖注入，然后在`logger`的逻辑中去使用基础设施层`File`的一些东西

`Logger`在初始化的时候会返回结果，以及文件写入时，会返回写入的成功与否。对这2个场景进行测试

```cpp
class MockFile : public File
{
public:
    MOCK_METHOD(int, open, (const char *name), (override));
    MOCK_METHOD(int, read, (char *buf, size_t size), (override));
    MOCK_METHOD(int, write, (const char *buf, size_t size), (override));
    MOCK_METHOD(int, close, (), (override));
};
```

首先，创建一个`MockFile`类，这种mock类一般都以Mock开头+接口名称，然后继承`File`这个接口类

`MOCK_METHOD`是 Google Mock 提供的一个宏，用于在单元测试中为虚函数生成模拟（Mock）实现。通过这个宏，可以方便地在测试中替代实际的类方法，用于验证调用行为、设置返回值以及模拟特定场景。

> **`MOCK_METHOD` 的作用**
>
> 1. **模拟类的虚函数行为**
>     - 它可以生成一个虚函数的 Mock 实现，供测试时使用，而无需真正实现该函数。
>     - 可以通过测试框架设置返回值、调用次数、调用顺序等。
> 2. **简化 Mock 类的实现**
>     - 传统的 Mock 类需要手动实现所有需要测试的方法，`MOCK_METHOD` 自动生成这些实现，节省开发时间。
> 3. **支持调用验证**
>     - 配合 Google Mock 的 `EXPECT_CALL` 等工具，可以验证 Mock 方法是否按预期被调用（如调用次数、参数等）。
> 4. **灵活定义行为**
>     - 使用 `WillOnce`、`WillRepeatedly` 等，可以动态定义方法的返回值或行为。
>
> ```cpp
> MOCK_METHOD(返回类型, 方法名称, (参数列表), (选项/修饰符));
> ```

`EXPECT_CALL` 是 Google Mock 提供的一个宏，用于设置对 Mock 对象的方法调用的**期望行为**，并进行调用验证。它允许开发者精确控制测试中 Mock 方法的行为，包括：

- 期望方法被调用的**次数**。
- 方法调用时的**参数**。
- 方法的**返回值**或其他行为。
- 验证是否按照预期调用了方法。

> **`EXPECT_CALL`的作用：**
>
> 1. **定义期望行为**
>     - 指定一个 Mock 方法在测试中可能被调用的条件、参数和返回值。
> 2. **验证调用**
>     - 确保方法被调用的次数、顺序和参数符合预期，未满足时测试会失败。
> 3. **动态模拟行为**
>     - 可以设置返回值、触发自定义动作等，从而模拟不同的业务场景。
>
> ```cpp
> EXPECT_CALL(mockObject, methodCall(matchers))
>     .Times(n)
>     .WillOnce(action)
>     .WillRepeatedly(action);
> ```
>
> 1. **`mockObject`**：Mock 对象的实例。
> 2. **`methodCall`**：Mock 方法的名称。参数（`_` 表示匹配任何值）
> 3. **`matchers`**：参数匹配器，用于匹配调用时传入的参数。
> 4. **`Times(n)`**：设置方法被调用的次数。
> 5. **`WillOnce(action)`**：定义方法第一次调用时的行为。
> 6. **`WillRepeatedly(action)`**：定义方法多次调用时的行为。
>
> > 常用功能解析：
> >
> > 1. **`Times`：调用次数**
> >
> >     ```cpp
> >     EXPECT_CALL(mockObject, methodCall(_)).Times(AtLeast(1));
> >     ```
> >
> >     说明：`Times`  指定方法调用的次数：
> >
> >     - `Times(1)`：必须调用一次。
> >     - `Times(AtLeast(1))`：至少调用一次。
> >     - `Times(Exactly(2))`：必须调用两次。
> >      - `Times(AnyNumber())`：调用次数不限。
> >
> > 2. **参数匹配器**
> >
> >     `EXPECT_CALL` 支持使用 Google Mock 提供的匹配器来检查方法的参数：
> >
> >     - 常用匹配器：
> >
> >         - `_`：是占位符，用于匹配任意值。
> >
> >         - `Eq(val)`：匹配等于 `val` 的值。
> >          - `Ne(val)`：匹配不等于 `val` 的值。
> >         - `Gt(val)` / `Lt(val)`：匹配大于/小于 `val` 的值。
> >          - `AllOf(m1, m2)`：同时满足多个条件。
> >          - `HasSubstr(s)`：字符串包含子串 `s`。
> >
> >     - 示例：
> >
> >         ```cpp
> >         EXPECT_CALL(mockObject, methodCall(Gt(10)));
> >         ```
> >
> >     - **作用**：方法 `methodCall` 的参数必须大于 `10`。
> >
> > 3. **`WillOnce` 和 `WillRepeatedly`**
> >
> >     - **`WillOnce`**：定义方法第一次调用的行为。
> >     - **`WillRepeatedly`**：定义方法后续调用的行为。
> >
> >     示例：
> >
> >     ```cpp
> >     EXPECT_CALL(mockObject, methodCall(_))
> >         .WillOnce(Return(10))
> >        .WillRepeatedly(Return(20));
> >     ```
> >
> >     - 作用：
> >
> >         - 第一次调用返回 `10`。
> >         - 后续每次调用返回 `20`。
> >
> > 

```cpp
using ::testing::_;
using ::testing::Return;

// 测试初始化失败的场景
TEST(GMockDemo, InitFailed)
{
    MockFile mockFile; // 从mock类中实例化一个mock对象
    EXPECT_CALL(mockFile, open(_)) 
        .WillOnce(Return(-1));
    // 期望调用mock对象的open函数，参数可以是任意值（用_表示），执行一次，并返回-1

    Logger logger(&mockFile); // 创建 Logger，并将 MockFile 作为依赖注入
    EXPECT_FALSE(logger.init()); // 调用 Logger::init()，期望其返回 false
}
```

目的：验证当 `open()` 方法失败时，`Logger::init()` 返回 `false`

```cpp
// 
TEST(GMockDemo, WriteFailed)
{
    MockFile mockFile;
    EXPECT_CALL(mockFile, open(_))
        .WillOnce(Return(0));
    // 设置期望，调用 open() 返回 0

    EXPECT_CALL(mockFile, write(_, 10))
        .Times(3)
        .WillOnce(Return(10)) // 第 1 次返回 10（模拟写入成功）
        .WillOnce(Return(-1)) // 第 2 次返回 -1（模拟写入失败）
        .WillOnce(Return(5)); // 第 3 次返回 5（模拟部分写入）
    // 设置期望，调用 write() 3次

    Logger logger(&mockFile);
    
    EXPECT_TRUE(logger.init());
    EXPECT_TRUE(logger.write(nullptr, 10));
    EXPECT_FALSE(logger.write(nullptr, 10));
    EXPECT_FALSE(logger.write(nullptr, 10));
}
```

目的：

- 验证 `init()` 在 `open()` 返回失败时，返回 `false`。
- 验证 `write()` 在某些条件下的返回值行为是否符合预期。

```bash
build$ ./gmock_start 
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from GMockDemo
[ RUN      ] GMockDemo.InitFailed
Logger(File *) 
[       OK ] GMockDemo.InitFailed (0 ms)
[ RUN      ] GMockDemo.WriteFailed
Logger(File *) 
[       OK ] GMockDemo.WriteFailed (0 ms)
[----------] 2 tests from GMockDemo (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```



### 二、参数限制

```cpp
class Calc
{
public:
    virtual int Do(int a, int b) = 0;
};
```

这里是一个抽象类`Calc`，其中的`virtual int Do(int a, int b) = 0;`是一个纯虚函数声明，子类必须重写此函数，且没有函数体（即纯虚函数）

```cpp
class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, Do, (int a, int b), (override));
};
```

`MockCalc` 使用 Google Mock 框架的 `MOCK_METHOD` 宏模拟了 `Do` 方法

`MOCK_METHOD(int, Do, (int a, int b), (override));` 是使用 Google Mock 框架定义模拟函数的方法。`MOCK_METHOD` 是一个宏，表示对 `Do` 函数的模拟实现：

- `int` 表示模拟的函数返回类型是 `int`。
- `Do` 是函数名。
- `(int a, int b)` 是函数的参数列表。
- `(override)` 表示这个模拟函数重写了基类 `Calc` 中的虚函数

通过这个宏，`MockCalc` 会模拟 `Calc` 类中的 `Do` 方法。

```cpp
int UseCalc(Calc &c, int a, int b) {
    return c.Do(a, b);
}
```

下面的语句，是 Google Test 中的语法，用于引入 `_` 符号，使其可以在 `EXPECT_CALL` 中作为通配符使用。`testing::_` 是 Google Mock 提供的一个特殊值，用于表示我们不关心具体的参数值。它通常用于模拟函数时，当不关心某个参数的值时使用

```cpp
using testing::_;
// 1. 可以限定参数的值
TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    EXPECT_CALL(calc, Do(5, _)); // Do的第二个参数可以理解为占位符
    // 表示要调用calc.Do函数时，第一个参数必须是5，第二个可以任意；否则会断言失败
    UseCalc(calc, 5, 10);
}
// 2. 可以限定参数的范围
TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    EXPECT_CALL(calc, Do(testing::Gt(5), testing::Lt(19)));
    UseCalc(calc, 6, 9);
}
// 3. 不对参数做限制
TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    EXPECT_CALL(calc, Do); // 等价于EXPECT_CALL(calc, Do(_, _));
    UseCalc(calc, 0, 0) ;
}
```

这里定义了三个测试用例，都属于 `CalcTest` 测试套件。通过 `EXPECT_CALL` 设置了对 `Do` 方法的调用期望，并在测试中使用不同的输入参数验证行为

其中，`EXPECT_CALL(calc, Do(testing::Gt(5), testing::Lt(19)));` 设置对`calc.Do()` 方法的期望调用：

- `testing::Gt(5)` 表示第一个参数 `a` 必须大于 5。
- `testing::Lt(19)` 表示第二个参数 `b` 必须小于 19。

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 3 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 3 tests from CalcTest
[ RUN      ] CalcTest.MockCalcTestCase1
[       OK ] CalcTest.MockCalcTestCase1 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase2
[       OK ] CalcTest.MockCalcTestCase2 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase3
[       OK ] CalcTest.MockCalcTestCase3 (0 ms)
[----------] 3 tests from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 3 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 3 tests.
```



### 三、限制函数调用次数

还是使用上面的源代码进行测试

```cpp
TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(0);
}

TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(3);

    UseCalc(calc, 0, 1);
    UseCalc(calc, 2, 3);
    UseCalc(calc, 4, 5);
}
```

这里写了2个测试用例，进行了函数调用次数的验证

`TEST(CalcTest, MockCalcTestCase1)` 定义了一个名为 `MockCalcTestCase1` 的测试用例，它属于测试套件 `CalcTest`。

`MockCalc calc;` 创建一个 `MockCalc` 类的对象 `calc`，这个对象模拟了 `Do` 方法。

`EXPECT_CALL(calc, Do).Times(0);`设置期望`Do`方法在测试过程中**不被调用**。`Times(0)`表示期望`Do`被调用 0 次。

- 这个测试验证了，即使调用了 `UseCalc(calc, 0, 1)` 这种函数，`Do` 方法不应该被调用。

`TEST(CalcTest, MockCalcTestCase2)` 定义了一个名为 `MockCalcTestCase2` 的测试用例，属于测试套件 `CalcTest`。

`MockCalc calc;` 创建一个 `MockCalc` 类的对象 `calc`，模拟了 `Do` 方法。

`EXPECT_CALL(calc, Do).Times(3);` 设置期望 `Do` 方法被调用 3 次。

`UseCalc(calc, 0, 1); UseCalc(calc, 2, 3); UseCalc(calc, 4, 5);` 连续调用 `UseCalc` 函数 3 次，每次都传入不同的参数。每次调用都会触发 `calc.Do()` 方法的调用。

- 这个测试用例验证了：`Do` 方法确实被调用了 3 次，每次调用的参数分别是 `(0, 1)`、`(2, 3)` 和 `(4, 5)`，但是在这里我们并不关心参数值，只关心调用次数

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from CalcTest
[ RUN      ] CalcTest.MockCalcTestCase1
[       OK ] CalcTest.MockCalcTestCase1 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase2
[       OK ] CalcTest.MockCalcTestCase2 (0 ms)
[----------] 2 tests from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

而如果没有按照期望设置的次数调用函数，则

```cpp
TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(0);
    UseCalc(calc, 0, 1);
}

TEST(CalcTest, MockCalcTestCase4) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(3);

    UseCalc(calc, 0, 1);
    UseCalc(calc, 2, 3);
}
```

```bash
[ RUN      ] CalcTest.MockCalcTestCase3
/home/will/lesson/gTest/22.call_times/call_times.cpp:38: Failure
Mock function called more times than expected - returning default value.
    Function call: Do(0, 1)
          Returns: 0
         Expected: to be never called
           Actual: called once - over-saturated and active

[  FAILED  ] CalcTest.MockCalcTestCase3 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase4
/home/will/lesson/gTest/22.call_times/call_times.cpp:44: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do)...
         Expected: to be called 3 times
           Actual: called twice - unsatisfied and active

[  FAILED  ] CalcTest.MockCalcTestCase4 (0 ms)
[----------] 4 tests from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
[  FAILED  ] 2 tests, listed below:
[  FAILED  ] CalcTest.MockCalcTestCase3
[  FAILED  ] CalcTest.MockCalcTestCase4

 2 FAILED TESTS
```



### 四、设置返回值

