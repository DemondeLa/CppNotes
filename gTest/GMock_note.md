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

- **Mock 类**：通过 `MockCalc` 模拟了 `Calc` 类的方法，使得测试可以控制这些方法的返回值。
- **EXPECT_CALL**：用于设置对模拟方法的预期行为，指定期望调用次数、返回值以及返回策略。
- **WillOnce 和 WillRepeatedly**：分别用于指定方法在一次调用时的返回值和在后续调用中的返回值。

`testing::Return`，引入了 `Return`，它是 Google Mock 提供的一个动作（Action），用于模拟函数的返回值

在设置期望时，可以通过 `WillOnce(Return(value))` 或 `WillRepeatedly(Return(value))` 来指定 Mock 函数在被调用时返回什么值。

```cpp
using testing::_;
using testing::Return;

TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    EXPECT_CALL(calc, Do)
        .WillOnce(Return(10));
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
}
```

**`TEST(CalcTest, MockCalcTestCase1)`**: 定义了一个测试用例 `MockCalcTestCase1`，它是 `CalcTest` 类的一部分，测试 `MockCalc` 类的行为。

**`MockCalc calc;`**: 创建一个 `MockCalc` 对象 `calc`。这是一个模拟的类，用于替代真实的 `Calc` 类的对象。

**`EXPECT_CALL(calc, Do).WillOnce(Return(10));`**: 期望 `calc` 对象的 `Do` 方法被调用一次 (`WillOnce`)，并且该方法调用时返回 10。这里模拟了 `Do` 方法的行为，表示每次调用 `Do` 方法都会返回 10。这里`EXPECT_CALL(calc, Do)`其实是生成了一个对象，因此可以调用它的`WillOnce`成员函数，表示执行一次`()`中的内容

**`EXPECT_EQ(UseCalc(calc, 5, 10), 10);`**: 调用 `UseCalc` 函数，并传入 `calc` 和两个参数。期望 `UseCalc` 返回的值为 10。由于之前已经设置了 `Do` 方法返回 10，所以 `UseCalc` 的结果应为 10。

```cpp
TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    EXPECT_CALL(calc, Do)
        .Times(3)
        .WillOnce(Return(10))
        .WillRepeatedly(Return(20));
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
    EXPECT_EQ(UseCalc(calc, 5, 10), 20);
    EXPECT_EQ(UseCalc(calc, 5, 10), 20);
}
```

**`EXPECT_CALL(calc, Do).Times(3)`**: 期望 `Do` 方法会被调用 3 次。

**`.WillOnce(Return(10))`**: 在第一次调用 `Do` 时，返回 10。

**`.WillRepeatedly(Return(20))`**: 在**<u>之后的每次调用</u>**中，`Do` 方法都返回 20。`WillRepeatedly` 表示在后续的每次调用中都会返回 20。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 10);`**: 第一次调用 `UseCalc`，应该返回 10，因为第一次 `Do` 被调用时返回 10。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 20);`**: 第二次调用 `UseCalc`，应该返回 20，因为第二次 `Do` 被调用时返回 20。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 20);`**: 第三次调用 `UseCalc`，同样返回 20，因为 `WillRepeatedly` 设置了之后的每次调用都返回 20。

```cpp
TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    int n = 10;
    EXPECT_CALL(calc, Do)
        .Times(3)
        .WillRepeatedly(Return(n++));
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
}
```

**`EXPECT_CALL(calc, Do).Times(3).WillRepeatedly(Return(n++));`**: 期望 `Do` 方法被调用 3 次。每次调用时，返回 `n++` 的值，即返回当前的 `n` 值，并在返回后将 `n` 增加 1。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 10);`**: 第一次调用 `UseCalc`，返回值为 10，因为 `n` 初始值为 10。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 11);`**: 第二次调用 `UseCalc`，返回值为 11，因为 `n` 已经递增为 11。

**`EXPECT_EQ(UseCalc(calc, 5, 10), 12);`**: 第三次调用 `UseCalc`，返回值为 12，因为 `n` 已经递增为 12。

> 注意，在上面的测试用例3中，`n++`只会被求值一次，即返回的永远都是10，因此下面的断言也都会成功，而如果要实现想要的逻辑，写法如下面的case4

```cpp
TEST(CalcTest, MockCalcTestCase4) {
    MockCalc calc;
    int n = 10;
    EXPECT_CALL(calc, Do)
        .Times(3)
        .WillOnce(Return(n++))
        .WillOnce(Return(n++))
        .WillOnce(Return(n++));
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
    EXPECT_EQ(UseCalc(calc, 5, 10), 11);
    EXPECT_EQ(UseCalc(calc, 5, 10), 12);
}
```

```bash
build$ ./set_return 
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from CalcTest
[ RUN      ] CalcTest.MockCalcTestCase1
[       OK ] CalcTest.MockCalcTestCase1 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase2
[       OK ] CalcTest.MockCalcTestCase2 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase3
[       OK ] CalcTest.MockCalcTestCase3 (0 ms)
[ RUN      ] CalcTest.MockCalcTestCase4
[       OK ] CalcTest.MockCalcTestCase4 (0 ms)
[----------] 4 tests from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```



### 五、参数匹配

如何对同一个函数指定多次`EXPECT_CALL`

```cpp
TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    EXPECT_CALL(calc, Do)
        .WillOnce(Return(10));
    EXPECT_CALL(calc, Do(5, _))
        .WillOnce(Return(20));
    EXPECT_EQ(UseCalc(calc, 6, 10), 10);
    EXPECT_EQ(UseCalc(calc, 6, 10), 10);
    /* EXPECT_EQ(UseCalc(calc, 5, 10), 20); */
}
```

**`EXPECT_CALL(calc, Do).WillOnce(Return(10));`**:

- 这是一个期望设置，表示期望 `calc` 对象的 `Do` 方法被调用一次，并且这次调用应该返回值 10。
- `WillOnce(Return(10))` 指定了 `Do` 方法在第一次被调用时会返回 10。
- 注意，`Do` 方法没有指定参数，因此这行代码针对没有任何参数的 `Do` 方法设置了期望。

**`EXPECT_CALL(calc, Do(5, _)).WillOnce(Return(20));`**:

- 这行代码期望 `Do` 方法被调用一次，并且调用时会传入参数 `5` 和任意值（由 `_` 表示）。
- `_` 是 Google Mock 提供的一个通配符，表示可以匹配任何类型的值。
- 在这个期望中，`Do` 方法被调用时会返回 20。
- 所以，这一行的期望设置为，当 `Do(5, _)` 被调用时，返回 20。

**`EXPECT_EQ(UseCalc(calc, 6, 10), 10);`**:

- 这是一个断言，检查调用 `UseCalc(calc, 6, 10)` 时返回的结果是否为 10。
- `UseCalc` 是测试中的一个函数，接受 `calc` 和两个参数。它应该调用 `calc.Do` 方法，返回值将会与期望值 10 进行比较。
- 根据之前设置的期望，第一次调用 `Do`（没有参数的 `Do`）会返回 10，因此 `UseCalc` 应该返回 10。

**`EXPECT_EQ(UseCalc(calc, 6, 10), 10);`**:

- 这是第二个断言，和前一个断言一样。它再次调用 `UseCalc(calc, 6, 10)`，并期望返回值仍然是 10。
- 注意，`EXPECT_CALL(calc, Do).WillOnce(Return(10));` 只设置了 `Do` 方法的第一次调用返回 10，因此第二次调用时会用之前设置的返回值 10。

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from CalcTest
[ RUN      ] CalcTest.MockCalcTestCase1
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:26: Failure
Mock function called more times than expected - returning default value.
    Function call: Do(6, 10)
          Returns: 0
         Expected: to be called once
           Actual: called twice - over-saturated and active

/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:31: Failure
Expected equality of these values:
  UseCalc(calc, 6, 10)
    Which is: 0
  10

/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:28: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do(5, _))...
         Expected: to be called once
           Actual: never called - unsatisfied and active

[  FAILED  ] CalcTest.MockCalcTestCase1 (0 ms)
[----------] 1 test from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] CalcTest.MockCalcTestCase1

 1 FAILED TEST
```

==**注意**==：在上面的测试用例中

- `EXPECT_CALL(calc, Do)`和`EXPECT_CALL(calc, Do(5, _))`这两行是对`MockCalc`对象的`Do`方法设置期望。每个`EXPECT_CALL`都会在测试中模拟对应的方法调用的行为。
    - 第一个期望设置了当 `Do()`（没有参数）被调用时返回 10。
    - 第二个期望设置了当 `Do(5, _)` 被调用时返回 20。

- 之后，调用`UseCalc`
    - 在 `UseCalc(calc, 6, 10)` 中，第一次调用 `Do`（没有参数）时会返回 10，因此第一个 `EXPECT_EQ(UseCalc(calc, 6, 10), 10);` 会通过。
    - 但是，对于第二次调用 `UseCalc` ，可以从上面的执行结果中得知，就是这里断言失败了，这是因为上面的`WillOnce(Return(10))`可以理解为一个生产者，它生成了一个结果为`10`的返回值，但是在第一次调用`UseCalc`中被消费掉了，因而在第二次调用时，由于没有匹配到`EXPECT_CALL(calc, Do(5, _))`，所以还是会去消费`WillOnce(Return(10))`，于是出现了断言失败
    - 如果将第二次的调用改为`EXPECT_EQ(UseCalc(calc, 5, 10), 20);`，就能够匹配到`EXPECT_CALL(calc, Do(5, _))`（第一个参数为5），则会去消费`WillOnce(Return(20))`

```cpp
TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    for (int i = 0; i < 3; ++i) {
        EXPECT_CALL(calc, Do)
            .WillOnce(Return(10 * i));
    }
    EXPECT_EQ(UseCalc(calc, 5, 10), 20);
}
```

将上面的用例中的循环展开，就相当于是

```cpp
EXPECT_CALL(calc, Do).WillOnce(Return(0));
EXPECT_CALL(calc, Do).WillOnce(Return(10));
EXPECT_CALL(calc, Do).WillOnce(Return(20));
```

==注意==：如果对同一个函数做多次`EXPECT_CALL`会从后向前匹配，即最后设置的期望的优先级最高

在后面进行断言，结果如下：

```bash
[ RUN      ] CalcTest.MockCalcTestCase2
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do)...
         Expected: to be called once
           Actual: never called - unsatisfied and active

/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do)...
         Expected: to be called once
           Actual: never called - unsatisfied and active

[  FAILED  ] CalcTest.MockCalcTestCase2 (0 ms)
```

断言失败，出现了2条报错信息，根据上面的结论，后面的断言`EXPECT_EQ(UseCalc(calc, 5, 10), 20);`会去匹配最后一条`EXPECT_CALL(calc, Do).WillOnce(Return(20));`，因此，循环中的前两条声明就没有被匹配，由此显示了上面的报错

但是，即使在最后再上两条断言`EXPECT_EQ(UseCalc(calc, 5, 10), 10);EXPECT_EQ(UseCalc(calc, 5, 10), 0);`，最终还是会报错，因为下面的3条断言都会去匹配`EXPECT_CALL(calc, Do).WillOnce(Return(20));`

```bash
[ RUN      ] CalcTest.MockCalcTestCase2
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Mock function called more times than expected - returning default value.
    Function call: Do(5, 10)
          Returns: 0
         Expected: to be called once
           Actual: called twice - over-saturated and active
# 循环内最后一次 EXPECT_CALL(calc, Do) 设置期望 Do 被调用一次，返回 20, WillOnce(Return(20)) 设置只生效 1 次，后续的调用没有期望行为
# 但实际代码调用了 UseCalc(calc, 5, 10) 3 次，导致 Do 被调用了 3 次
# 超出了期望的调用次数，Google Mock 检测到超额调用并返回默认值 0
# 调用次数与 EXPECT_CALL 设置的次数不匹配
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:42: Failure
Expected equality of these values:
  UseCalc(calc, 5, 10)
    Which is: 0
  10
# 第 2 次调用 UseCalc(calc, 5, 10)，预期返回值为 10
# EXPECT_CALL(calc, Do) 设置的返回值只覆盖第 1 次调用（20），没有为第 2 次调用设置期望行为
# 由于未设置期望，Google Mock 返回了默认值 0，导致断言失败
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Mock function called more times than expected - returning default value.
    Function call: Do(5, 10)
          Returns: 0
         Expected: to be called once
           Actual: called 3 times - over-saturated and active
# 第 3 次调用 UseCalc(calc, 5, 10) 时，Google Mock 再次发现 Do 方法的调用次数超过了预期
# EXPECT_CALL(calc, Do) 仅设置了调用一次的返回值 20，但实际调用了 3 次，超过了期望调用次数, 第 3 次调用返回默认值 0
/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do)...
         Expected: to be called once
           Actual: never called - unsatisfied and active

/home/will/lesson/gTest/24.multiple_expectation/multiple.cpp:38: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Do)...
         Expected: to be called once
           Actual: never called - unsatisfied and active

[  FAILED  ] CalcTest.MockCalcTestCase2 (0 ms)
```

最后2条报错信息：Google Mock 会追踪所有设置的 `EXPECT_CALL`，在循环内的前两次 `EXPECT_CALL(calc, Do)` 被后续的 `EXPECT_CALL` 覆盖，因此这些期望没有被任何实际调用满足。报错指出，设置的期望没有被调用（"never called"），且仍然处于“未满足和激活”的状态

**循环中的 `EXPECT_CALL` 设置相互覆盖，导致只有最后一次设置生效，之前的期望被遗漏。**

因此，这条用例想要展示的逻辑的正确写法，应该是

```cpp
TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    for (int i = 0; i < 3; ++i) {
        EXPECT_CALL(calc, Do(i, _))
            .WillOnce(Return(10 * i));
    }
    EXPECT_EQ(UseCalc(calc, 2, 10), 20);
    EXPECT_EQ(UseCalc(calc, 1, 10), 10);
    EXPECT_EQ(UseCalc(calc, 0, 10), 0);
}
```

由于这里在循环的`EXPECT_CALL`中对参数进行了设置，因此在下面的断言中，不会像上例一样去匹配最后一个，而是会根据参数进行匹配

在实际的使用中，参数可能并不会与返回值有关联，因此

```cpp
TEST(CalcTest, MockCalcTestCase4) {
    MockCalc calc;
    for (int i = 0; i < 3; ++i) {
        EXPECT_CALL(calc, Do)
            .WillOnce(Return(10 * i))
            .RetiresOnSaturation();
    }
    EXPECT_EQ(UseCalc(calc, 5, 10), 20);
    EXPECT_EQ(UseCalc(calc, 5, 10), 10);
    EXPECT_EQ(UseCalc(calc, 5, 10), 0);
}
```

其中，`.RetiresOnSaturation()`当这个期望被**满足指定调用次数**后，期望会自动“退役”（从活动期望中移除）。例如，当 `WillOnce(Return(10 * 0))` 被调用一次后，它不再是活动期望

如果不加 `RetiresOnSaturation`，多次定义的 `EXPECT_CALL` 会相互覆盖，导致测试失败

在后面的断言时，还是遵循之前的结论：==**如果对同一个函数做多次`EXPECT_CALL`会从后向前匹配，即最后设置的期望的优先级最高**==，即先消费的是`WillOnce(Return(10 * 2))`，然后是`WillOnce(Return(10 * 1))`

**总结**

1. `RetiresOnSaturation` 的作用
    - 让每次 `EXPECT_CALL` 的期望在被满足后退役，不再参与后续匹配。
    - 例如：第一次调用 `calc.Do()` 时，匹配的是 `WillOnce(Return(10 * 2))`（`20`）。匹配成功后，这条期望“退役”，不再影响后续调用。
2. 期望的优先级
    - Google Mock 中，期望的匹配优先级与定义顺序相关：
        - 最先定义的 `EXPECT_CALL` 优先级最高。
        - 通过 `RetiresOnSaturation`，可以逐一触发优先级更低的期望。
3. 多次定义 `EXPECT_CALL`
    - 每次 `for` 循环都会创建一个新的 `EXPECT_CALL`。
    - 通过 `WillOnce` 和循环变量 `i`，每个调用的返回值动态变化。

### 六、限制调用顺序

```cpp
class Calc
{
public:
    virtual int Do(int a, int b) = 0;
    virtual int Work(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, Do, (int a, int b), (override));
    MOCK_METHOD(int, Work, (int a, int b), (override));
};

int UseCalc(Calc &c, int a, int b, bool d) {
    return d ? c.Do(a, b) : c.Work(a, b);
}
```

默认的测试用例：

```cpp


TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    
    EXPECT_CALL(calc, Do);
    EXPECT_CALL(calc, Work);
    
    UseCalc(calc, 5, 10, false);
    UseCalc(calc, 5, 10, true);
}
```

`EXPECT_CALL(calc, Do);`和`EXPECT_CALL(calc, Work);`，设置对这两个函数的期望，没有执行调用次数，默认期望至少被调用一次

下面分别调用了这两个函数，因此断言通过

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from CalcTest
[ RUN      ] CalcTest.MockCalcTestCase1
[       OK ] CalcTest.MockCalcTestCase1 (0 ms)
[----------] 1 test from CalcTest (0 ms total)

[----------] Global test environment tear-down
[==========] 
```

`InSequence seq;`对应于当前的作用域，使用 `InSequence` 对象 `seq` 将所有的 `EXPECT_CALL` 组织成顺序期望。顺序期望意味着调用顺序必须严格符合定义顺序。

```cpp
using testing::InSequence;
TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    
    InSequence seq;

    EXPECT_CALL(calc, Do);
    EXPECT_CALL(calc, Work);
    
    UseCalc(calc, 5, 10, false);
    UseCalc(calc, 5, 10, true);
}
```

由于 `InSequence` 的存在，`calc.Do` 必须在 `calc.Work` 之前被调用，因此上面的测试用例会断言失败

```bash
[ RUN      ] CalcTest.MockCalcTestCase2
unknown file: Failure

Unexpected mock function call - returning default value.
    Function call: Work(5, 10)
          Returns: 0
# Google Mock 检测到 calc.Work(5, 10) 的调用，但这次调用未能匹配任何当前有效的 EXPECT_CALL。
# 因为期望中的 calc.Work 有前置条件：calc.Do 必须先被调用（由 InSequence 指定顺序）。
# Google Mock 提供了一个默认返回值（0），因为没有匹配的期望
Google Mock tried the following 1 expectation, but it didn't match:

/home/will/lesson/gTest/25.order/order.cpp:43: EXPECT_CALL(calc, Work)...
         Expected: all pre-requisites are satisfied
           Actual: the following immediate pre-requisites are not satisfied:
/home/will/lesson/gTest/25.order/order.cpp:42: pre-requisite #0
                   (end of pre-requisites)
# Google Mock 找到与 calc.Work 匹配的期望定义（EXPECT_CALL(calc, Work)），但由于前置条件未满足，该期望未生效。
# 前置条件是：EXPECT_CALL(calc, Do) 必须先执行，并且标记为已满足；而当前情况：calc.Work 的调用发生在 calc.Do 调用之前，因此前置条件未满足。
         Expected: to be called once
           Actual: never called - unsatisfied and active

/home/will/lesson/gTest/25.order/order.cpp:43: Failure
Actual function call count doesn't match EXPECT_CALL(calc, Work)...
# EXPECT_CALL(calc, Work) 的期望明确指定它需要被调用一次，但实际测试中它从未按预期顺序被调用。
# calc.Work 的调用发生在 calc.Do 调用之前，违反了 InSequence 指定的调用顺序。
         Expected: to be called once
           Actual: never called - unsatisfied and active
# 这条信息与上一条类似，进一步确认 calc.Work 的调用未满足其定义的调用顺序
# 期望中定义的 EXPECT_CALL(calc, Work) 未能触发，因为其前置条件未被满足
[  FAILED  ] CalcTest.MockCalcTestCase2 (0 ms)
```

**`InSequence` 的作用**：强制要求 `EXPECT_CALL(calc, Do)` 必须在 `EXPECT_CALL(calc, Work)` 之前执行。
