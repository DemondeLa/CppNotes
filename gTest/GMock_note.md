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



### 七、处理模板类型

`MOCK_METHOD` 是 Google Mock 用于模拟类成员函数的宏。它的标准格式是

```cpp
MOCK_METHOD(return_type, method_name, (args...), (attributes));
// return_type：方法的返回类型；
// method_name：方法名；
// (args...)：参数列表；
// (attributes)：方法的属性（例如 override）
```

Google Mock 宏的参数解析是基于括号和逗号来分割的。如果你使用括号包裹返回类型（即 `std::map<int, int>`），这实际上是允许你将类型作为第一个参数传入宏，接下来才是方法名称、参数列表和属性。

`EXPECT_CALL` 是 Google Mock 用于设置期望的方法调用

```cpp
class MakeMap 
{
public:
    virtual std::map<int, int> make1() = 0;
    virtual std::map<int, int> make2() = 0;
};

class MockMakeMap : public MakeMap
{
public:
    MOCK_METHOD((std::map<int, int>), make1, (), (override));

    using ReturnType = std::map<int, int>;
    MOCK_METHOD(ReturnType, make2, (), (override));
};
```

`MockMakeMap` 类继承自 `MakeMap`，并使用 Google Mock 提供的 `MOCK_METHOD` 宏来模拟 `MakeMap` 类中的 `make1` 和 `make2` 方法

- 第一个 `MOCK_METHOD` 用来模拟 `make1()` 方法，返回类型为 `std::map<int, int>`，且不接受任何参数`()`，`(override)` 表示这个方法是重写自基类 `MakeMap` 的方法。

    > 如果写成：`MOCK_METHOD(std::map<int, int>, make1, (), (override));`会出现变异错误，因为`MOCK_METHOD`是一个宏，其中的参数就会以`,`分隔，当类型中出现逗号时，编译器会认为`std::map<int`是第一个参数，接收了5个参数，就会出现问题。
    >
    > 因此这里通过这种加`()`的方式，或者下面给类型起别名的方式

- 第二个 `MOCK_METHOD` 用来模拟 `make2()` 方法，返回类型为 `ReturnType`（它是 `std::map<int, int>` 的别名）。

    **`using ReturnType = std::map<int, int>;`**：这行代码定义了一个类型别名 `ReturnType`，它指代 `std::map<int, int>` 类型。这使得之后的代码更加简洁和易于维护。使用 `ReturnType` 来代替 `std::map<int, int>` 使得代码更具可读性和灵活性，特别是在类型可能发生变化的情况下

```cpp
TEST(TestMakeMap, Case1)
{
    MockMakeMap makemap;// 创建了 MockMakeMap 类的一个实例
    // 此时，make1() 和 make2() 方法已经被模拟
    EXPECT_CALL(makemap, make1);
    EXPECT_CALL(makemap, make2);
    
    makemap.make1();     
    makemap.make2(); 
}
```

`TEST(TestMakeMap, Case1)` 定义了一个 Google Test 测试用例，其中 `TestMakeMap` 是测试用例的名称，`Case1` 是测试的具体名称

`EXPECT_CALL(makemap, make1);` 和 `EXPECT_CALL(makemap, make2);`：这两行代码使用 Google Mock 的 `EXPECT_CALL` 宏来声明期望。

- `EXPECT_CALL(makemap, make);` 只是设定了期望，并没有指定具体的行为或返回值。如果需要指定返回值，通常会使用 `WillOnce` 或 `WillRepeatedly` 等方法

- `EXPECT_CALL(makemap, make1)` 表示在接下来的代码执行过程中，`make1` 方法应该被调用一次。
- 同样，`EXPECT_CALL(makemap, make2)` 表示 `make2` 方法也应该被调用一次。

`makemap.make1();` 和 `makemap.make2();`：

- 实际调用了 `make1()` 和 `make2()` 方法。此时，Google Mock 会检查这两个方法是否按照预期被调用。
- 由于在 `EXPECT_CALL` 中设定了期望，因此，如果这两个方法没有被调用，或者被调用次数不符，测试将失败。

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestMakeMap
[ RUN      ] TestMakeMap.Case1
[       OK ] TestMakeMap.Case1 (0 ms)
[----------] 1 test from TestMakeMap (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```



### 八、模拟私有成员函数

Mock一个私有成员函数和Mock一个公有的是完全一致的。

`MOCK_METHOD`其实是对成员函数的重写，虽然基类是`prvate`的，但是可以在派生类中设置公共的，因此没有区别

```cpp
class MakeMap 
{
private:
    virtual std::map<int, int> make1() = 0;
    virtual std::map<int, int> make2() = 0;
};

class MockMakeMap : public MakeMap
{
public:
    MOCK_METHOD((std::map<int, int>), make1, (), (override));

    using ReturnType = std::map<int, int>;
    MOCK_METHOD(ReturnType, make2, (), (override));
};

TEST(TestMakeMap, Case1)
{
    MockMakeMap makemap;
    EXPECT_CALL(makemap, make1);
    EXPECT_CALL(makemap, make2);

    makemap.make1();
    makemap.make2();
}
```

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestMakeMap
[ RUN      ] TestMakeMap.Case1
[       OK ] TestMakeMap.Case1 (0 ms)
[----------] 1 test from TestMakeMap (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

通过顺利编译并执行。

因此成员的属性，无论是`public`、`protected`或是`private`对于gmock来说，是没有什么区别的



### 九、模拟重载函数



```cpp
class Calc
{
public:
    virtual int calc(int a) = 0;
    virtual int calc(int a, int b) = 0;
    virtual int calc(int a) const = 0; // 声明为 const，即该方法不会修改对象的状态
    virtual int calc(double a) = 0;
};

int UseCalc(Calc &c, int a) {
    return c.calc(a);
}

int UseCalc(Calc &c, int a, int b) {
    return c.calc(a, b);
}

int UseCalc(const Calc &c, int a) {
    return c.calc(a);
}

int UseCalc(Calc &c, double a) {
    return c.calc(a);
}
```

这里定义了一个抽象类 `Calc`，包含了四个虚拟函数 `calc`，每个方法有不同的参数或常量修饰符

以及不同的`UseCal`函数用于调用 `Calc` 类的不同 `calc` 方法。这些重载函数通过不同的参数组合调用 `Calc` 类的 `calc` 方法。根据参数类型的不同，分别调用 `calc(int)`、`calc(int, int)`、`calc(double)` 或 `calc(int) const`。

其中`const Calc &c`用于处理传递给 `UseCalc` 的 `const` 对象，保证方法在不修改对象状态的情况下调用。

```cpp
class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, calc, (int a), (override));
    MOCK_METHOD(int, calc, (int a, int b), (override));
    MOCK_METHOD(int, calc, (int a), (const,override));
    MOCK_METHOD(int, calc, (double a), (override));
};
```

`MockCalc` 类继承自 `Calc`，并用 `MOCK_METHOD` 宏模拟 `Calc` 类的各个 `calc` 方法。

- 每个 `MOCK_METHOD` 对应于 `Calc` 中的一个虚拟函数，并且通过 `(override)` 修饰符指示这是对基类方法的重写。

- `MOCK_METHOD(int, calc, (int a), (const, override));` 对应 `calc(int a) const`，这是一个常量成员函数，它承诺不会修改对象的状态。

    > 在 C++ 中，`const` 对象是不能调用非 `const` 成员函数的。这是因为非 `const` 成员函数可能会修改对象的状态，而 `const` 对象承诺不修改其状态，因此不能调用可能修改对象状态的成员函数
    >
    > > ==**为什么 `const` 对象不能调用非 `const` 成员函数？**==
    > >
    > > - **编译器的检查**：编译器通过 `const` 成员函数来确保对象的状态不会发生变化。当你声明一个对象为 `const`，编译器会检查它只能调用那些保证不会修改对象状态的 `const` 成员函数。
    > > - **对象状态保护**：`const` 对象的目的是确保对象的状态不可更改。如果允许 `const` 对象调用非 `const` 成员函数，则意味着你有可能修改 `const` 对象的状态，这会破坏 `const` 的语义。

```cpp
using testing::_;
using testing::An; 
using testing::Const;
using testing::Return;
```

`_` 是 Google Mock 中的一个占位符，用来表示匹配任何类型的参数。它常用于设置期望值时不关心某个参数的具体值，意味着该参数可以是任意值。**应用场景**： 当你不关心某个参数的具体值，只关心该方法是否被调用或是否按照某种方式调用时，可以使用 `_` 来代替实际的值。比如，你只关心函数是否被调用，而不在意传递的参数是什么。

`An<Type>()` 是 Google Mock 中的一种参数匹配器，用于表示匹配某种特定类型的值。`An<Type>` 用来匹配某个特定类型的参数，无论该类型的值是什么。它相当于一个类型过滤器。**应用场景**： 当你希望精确匹配某种类型的参数，而不关心它的具体值时，可以使用 `An<Type>()`。这种方式比使用 `_` 更具类型约束性，因为它明确了匹配的参数类型。

`Const` 是 Google Mock 中用于匹配常量成员函数的匹配器。它通常用于模拟 **const** 成员函数的调用。默认情况下，`EXPECT_CALL` 只适用于非 `const` 成员函数，若想对 `const` 成员函数进行模拟，必须使用 `Const()` 匹配器。**应用场景**： 当你模拟一个对象的常量成员函数（即声明为 `const` 的成员函数）时，必须使用 `Const()`。否则，Google Mock 将无法正确匹配该成员函数。

`Return(value)` 是 Google Mock 中的一个动作（Action），它指定当某个模拟方法被调用时返回一个特定的值。这个值可以是常量、对象、指针等。**应用场景**： `Return` 用于设置方法调用的返回值，可以用于模拟方法的返回行为。当你希望模拟一个函数的返回值时，`Return()` 是必不可少的工具。

```cpp
TEST(TestCalc, Case1) 
{
    MockCalc calc;
    EXPECT_CALL(calc, calc(An<int>())).Times(1).WillRepeatedly(Return(1));
    /*
	这个期望设置 calc 对象的 calc(int) 方法会被调用，且返回值为 1。
		An<int>() 是一个匹配器，表示接受任何一个整数值。
		Times(1) 表示该方法应该被调用一次。
		WillRepeatedly(Return(1)) 表示每次调用时都返回 1。
		如果是WillOnce(Return(1)) 表示第一次调用时返回 1
    */
    EXPECT_CALL(calc, calc(An<int>(), An<int>())).Times(2).WillRepeatedly(Return(2));
    /*
    这里模拟了 calc(int, int) 方法的调用，(An<int>(), An<int>()) 表示接受任意两个整数参数，(_, _) 这种写法也可以。
    	Times(2) 表示这个方法应该被调用两次，WillRepeatedly(Return(2)) 表示每次调用时返回 2。
    */

    const MockCalc calc2;
    EXPECT_CALL(Const(calc2), calc(_)).Times(1).WillRepeatedly(Return(3));
    /*
    这里模拟了一个 const 对象上的 calc(int) 方法。
    注意，这里使用 Const(calc2) 来表明 calc2 是一个 const 对象；如果没有用Const修饰，在宏替换后，值传递的可能就是非const对象了
    	这个期望设置 calc(int) 方法在 const 对象上的调用应该返回 3
    */

    EXPECT_CALL(calc, calc(An<double>())).Times(1).WillRepeatedly(Return(4));
    /*
    这里模拟了 calc(double) 方法的调用，An<double>() 表示接受任何 double 类型的参数。
    Times(1) 表示这个方法应该被调用一次，WillRepeatedly(Return(4)) 表示每次调用时返回 4。
    */

    EXPECT_EQ(UseCalc(calc, 1), 1);
    EXPECT_EQ(UseCalc(calc, 2, 3), 2);
    EXPECT_EQ(UseCalc(calc, 0, -1), 2);
    EXPECT_EQ(UseCalc(calc2, 3), 3);
    EXPECT_EQ(UseCalc(calc, 3.14), 1);
}
```

在 `TEST(TestCalc, Case1)` 中，调用了不同版本的 `UseCalc`，这些版本分别调用了 `calc(int)`、`calc(int, int)`、`calc(double)` 和 `calc(int) const`。

每次调用都会触发相应的 `EXPECT_CALL`，并验证返回值是否与期望一致

```cpp
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestCalc
[ RUN      ] TestCalc.Case1
[       OK ] TestCalc.Case1 (0 ms)
[----------] 1 test from TestCalc (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

gmock中为了区分多个重载函数，其规则和重载函数的规则一致，根据参数的类型、数量、属性判断。



### 十、处理模板类

对模板类进行mock的方法很简单，即对模板类使用具体的类型进行实例化

```cpp
template <typename Number>
class Calc
{
public:
    virtual Number calc(Number a, Number b) = 0;
};

template <typename Number>
class MockCalc : public Calc<Number>
{
public:
    MOCK_METHOD(Number, calc, (Number a, Number b));
};
```

`MockCalc` 继承自 `Calc`，用于 **模拟** `Calc` 的行为

```
MOCK_METHOD(Number, calc, (Number a, Number b));
```

- 这是 Google Mock 的 **宏**，用于定义一个模拟方法。
- `MOCK_METHOD(返回类型, 方法名, (参数列表))` 让 Google Mock 生成一个虚拟方法，并支持断言。

```cpp
TEST(TestCalc, Case1) {
    MockCalc<int> calc;
    EXPECT_CALL(calc, calc);

    calc.calc(1, 2);
}

TEST(TestCalc, Case2) {
    MockCalc<double> calc;
    EXPECT_CALL(calc, calc);

    calc.calc(2.1, 3.4);
}
```

这里 `MockCalc<int>` 表示 `calc` 方法的 `Number` 类型为 `int`。MockCalc<double>` 指定 `Number` 类型为 `double

```
EXPECT_CALL(calc, calc);
```

- 这是 Google Mock 提供的 **预期调用** 机制。
- 这行代码表示我们 **期望** `calc.calc()` **至少被调用一次**。
- 若 `calc.calc()` 没有被调用，测试会失败。

> `MOCK_METHOD` 让 `MockCalc` 继承 `Calc`，但不需要真正实现 `calc` 方法，而是 **由 Google Mock 生成一个 Mock 版本**，允许：
>
> - **设置期望调用**（`EXPECT_CALL`）
> - **返回特定值**
> - **记录调用次数**
> - **检查调用参数**

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from TestCalc
[ RUN      ] TestCalc.Case1
[       OK ] TestCalc.Case1 (0 ms)
[ RUN      ] TestCalc.Case2
[       OK ] TestCalc.Case2 (0 ms)
[----------] 2 tests from TestCalc (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```



### 十一、模拟非虚函数

gmock是不支持非虚函数的mock的

```cpp
class Adder 
{
public:
    int add(int a, int b) {
        return a + b;
    }
};

class MockAdder // 没有继承
{
public:
    MOCK_METHOD(int, add, (int a, int b), ()); // 没有override
};
```

 `Adder`类 是一个普通的加法类，提供 `add(int a, int b)` 方法，返回 `a + b`。它没有 **虚函数**，也**没有基类**，因此无法用传统的 **继承+重写** 方式来 Mock。

`MockAdder`这个 Mock 类 **没有继承 `Adder`**，但仍然可以用 `MOCK_METHOD` 来 Mock `add` 方法：

- ```
    MOCK_METHOD(int, add, (int a, int b), ());
    ```

    - **第一个参数 `int`**：返回类型
    - **第二个参数 `add`**：函数名
    - **第三个参数 `(int a, int b)`**：参数列表
    - **第四个参数 `()`**：修饰符（这里是空的，表示默认行为）

- 这意味着 `MockAdder` **不依赖继承关系**，仍然能 Mock `add`。

```cpp
template <typename AdderType>
int UseAdder(AdderType &adder, int a, int b) {
    return adder.add(a, b);
}
```

`UseAdder` 是一个 **模板函数**，接受一个 `AdderType` 类型的对象，并调用 `adder.add(a, b)` 进行加法计算

由于是 **模板**，它可以接受任何具有 `add` 方法的对象，包括：**普通类**（如 `Adder`）、**Mock 类**（如 `MockAdder`）

```cpp
using testing::Return;
TEST(TestAdder, Case) {
    MockAdder adder; // 创建 Mock 对象
    EXPECT_CALL(adder, add) // 告诉 Google Mock：预期 adder.add() 方法会被调用
        .WillOnce(Return(10)); // 指定 add 方法 被调用一次时返回 10

    EXPECT_EQ(UseAdder(adder, 4, 1), 10);
}
```

`EXPECT_EQ(UseAdder(adder, 4, 1), 10);`

- 调用 `UseAdder(adder, 4, 1)`，实际上是 `adder.add(4,1)`。
- 由于 `MockAdder` 受 `EXPECT_CALL` 控制，它返回 **10**。
- 断言 `UseAdder` 的返回值是否等于 `10`，如果相等，测试通过。

> 传统的 Google Mock 通常要求 Mock 类 **继承基类** 并 **重写虚函数**。其本质上是将虚拟类中的虚函数用`MOCK_METHOD`实现了一下，实现中是各种控制，类似于使用`expect_call`来自定义里面的行为。因此对`MOCK_METHOD`来说，是否是继承来的并没有什么影响
>
> 在这里，直接 Mock 了一个普通方法，而 **不需要继承** `Adder`，是 Google Mock 的一项高级用法。

`UseAdder` 使用 **模板**，它不关心 `AdderType` 是 **真实类** 还是 **Mock 类**，只要它有 `add()` 方法就行（有同样的返回值和参数），这样就可以通过Mock类去替换实际的类，进而替换这个函数。甚至，没有`Adder`这个类，程序也依然是能够正常运行的

这让 `UseAdder` 更加 **灵活**：生产代码使用 **真实 `Adder`**，测试代码使用 **Mock `Adder`**

> **为什么这么设计**
>
> 1. 普通 `Adder` 没有虚函数，不能直接 Mock
>
>     `Adder` 里 `add()` 不是 `virtual`，如果 `MockAdder` **继承** 了 `Adder`，并且 `Adder::add` **不是虚函数**，那么在 `EXPECT_CALL` 之后调用 `adder.add()` 时，它 **不会调用 `MockAdder::add`**，而是调用 `Adder::add`，因为 **C++ 的非虚函数不会进行动态绑定**，Mock 机制无法拦截 **非虚方法**：
>
>     ```cpp
>     class Adder {
>     public:
>         int add(int a, int b) { return a + b; } // 不是 virtual
>     };
>     ```
>
>     **不能直接继承 `Adder`，否则 Mock 失效。**
>
>     > C++ 的 **非虚函数** **不会** 进行 **动态绑定（Dynamic Dispatch）**
>     >
>     > `MockAdder::add` 虽然使用了 `MOCK_METHOD`，但 `Adder::add` **不是虚函数**，调用 `adder.add(4, 1)` 时：
>     >
>     > - **编译器会进行静态绑定，直接调用 `Adder::add`，绕过 `MockAdder::add`**
>     > - Google Mock 只能拦截 **虚函数**，但这里 `Adder::add` 不是 `virtual`，所以不会生效
>     >
>     > 当 `adder.add(4, 1)` 被调用时：
>     >
>     > - **如果 `Adder::add` 是虚函数**，C++ 会走 **虚函数表（vtable）**，从而调用 `MockAdder::add`
>     > - **但 `Adder::add` 不是虚函数**，所以 C++ 直接调用 `Adder::add`，Google Mock 无法拦截
>
> 2. 用 `MockAdder` 替代 `Adder`
>
>     由于 `UseAdder` **使用模板**，它可以接受 `MockAdder`，这就绕过了 **继承** 的问题。
>
>     这样，我们就可以 **Mock 非虚函数**，灵活测试 `UseAdder`
>
> | 重点                   | 解释                                                         |
> | ---------------------- | ------------------------------------------------------------ |
> | **不需要继承 `Adder`** | `MockAdder` 直接使用 `MOCK_METHOD` Mock `add`，不继承 `Adder` |
> | **模板使代码更灵活**   | `UseAdder` 采用模板，既能接受 `Adder`，也能接受 `MockAdder`  |
> | **Mock 非虚函数**      | `Adder::add` 不是 `virtual`，无法传统 Mock，只能用模板技巧   |
> | **Google Mock 实现**   | `EXPECT_CALL` 设定 `add` 返回值，确保测试通过                |

**如果 `Adder::add` 不是虚函数，Google Mock 不能拦截调用，因为 C++ 进行的是** **静态绑定**，必须用 `virtual` 让它走 **动态绑定** 才能生效。

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestAdder
[ RUN      ] TestAdder.Case
[       OK ] TestAdder.Case (0 ms)
[----------] 1 test from TestAdder (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```



### 十二、处理自由函数

自由函数就是不属于任何类的函数，即非类的成员函数



```cpp
int add(int a, int b) {
    return a + b;
}

class AddInterface
{
public:
    virtual int add(int a, int b) {
        // 定义成虚函数，使得可以在子类中重写它以实现多态性
        return ::add(a, b);
    }
};
```

这里定义成虚函数可以和上面非虚函数对照的理解

```cpp
class MockAdd : public AddInterface
{
public:
    MOCK_METHOD(int, add, (int a, int b), (override));
};
```

MOCK_METHOD 宏：这是 Google Mock 提供的宏，用于生成一个模拟函数。这里的参数分别是：

- 返回类型：`int`
- 函数名：`add`
- 参数列表：`(int a, int b)`
- 附加修饰：`(override)` 表明这个模拟函数是对基类中虚函数的重写。

通过使用 `MockAdd`，我们可以在测试中定义期望（Expectations）和行为（Actions），不必依赖于真正的 `add` 实现，从而更灵活地控制测试环境

```cpp
int UseAdd(AddInterface &adder, int a, int b) {
    return adder.add(a, b);
}
```

`UseAdd` 接受一个 `AddInterface` 类型的引用（可能是实际的实现或模拟对象）和两个整数，然后调用该接口的 `add` 方法，返回计算结果

这种设计让函数不依赖于具体的实现，只需要接口定义，从而提高了代码的灵活性和测试的方便性（依赖注入）

```cpp
using testing::Return;
TEST(TestAdd, Case) {
    MockAdd adder;
    EXPECT_CALL(adder, add)
        .WillOnce(Return(10));
    EXPECT_EQ(UseAdd(adder, 4, 2), 10);
}
```

EXPECT_CALL：

- 设置对 `adder.add` 方法的调用期望。也就是说，当 `adder.add` 被调用时，应该触发设置的行为。
- `WillOnce(Return(10))` 指定第一次调用时，该方法将返回 `10`。这里是“行为驱动”的模拟设置，用于验证代码在特定输入下的行为，而不是实际进行加法计算。

`EXPECT_EQ(UseAdd(adder, 4, 2), 10);` 调用 `UseAdd` 函数，传入 `adder` 模拟对象和参数 `4, 2`，然后期望结果是 `10`。因为模拟对象的 `add` 方法已经被设置为在第一次调用时返回 `10`，所以测试通过

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestAdd
[ RUN      ] TestAdd.Case
[       OK ] TestAdd.Case (0 ms)
[----------] 1 test from TestAdd (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

==**总结**==：Google Mock（gmock）确实不支持直接模拟普通函数（即非类成员函数）和非虚函数，因为 gmock 的机制依赖于虚函数的多态性或者在类中的重写行为。为了测试这两种函数，就必须采用一些特殊的策略：

- 普通函数是全局函数或者静态函数，它们没有类的上下文，也就没有虚函数机制。解决这个问题的一种常用方法是**包装（wrap）**这个普通函数：
    1. 定义一个接口类：这个接口类提供一个虚函数，用来调用实际的普通函数。这样，我们就把普通函数封装在一个可以被继承和重写的接口中
    2. 编写一个Mock类：这个 Mock 类继承自接口类，并使用 `MOCK_METHOD` 模拟接口中的虚函数
    3. 依赖注入使用接口：这样，在你的代码中就不要直接调用全局函数 `add`，而是通过接口调用。如上例：在测试时，可以传入 `MockAdd` 对象，然后设定期望和返回值，而在生产环境中可以传入实际实现的对象
- 非虚函数不能被重写，因为编译时就已经决定了调用哪个函数，这使得 gmock 无法直接拦截调用。解决这个问题的思路一般有两种：
    1. 使用接口抽象：如果有可能，将非虚函数抽象成一个虚函数接口，然后由实际类实现。这样就可以使用类似于上面普通函数的包装方法来测试。不过这种方法需要修改原有设计，有时可能不适用。
    2. 使用模板函数进行调用：如果无法修改原有的非虚函数定义，可以采用如下策略：
        - 定义一个与非虚函数同名的 Mock 类，里面使用 `MOCK_METHOD` 定义一个同名函数。
        - 编写一个模板函数，这个函数在调用时可以根据传入的类型（真实类或 Mock 类）来调用相应的实现。这样通过依赖注入的方式，你可以在测试时传入 Mock 对象，而在实际运行时传入真实对象。

上面的设计思想（**依赖注入**、**接口抽象**）不仅有助于测试，还能提升代码的灵活性和可维护性。虽然需要额外编写一些包装代码，但在单元测试和模块解耦方面能带来很大的好处。



### 十三、处理不期望调用的函数

```cpp
class Calc
{
public:
    virtual int add(int a, int b) = 0;
    virtual int sub(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, add, (int a, int b), (override));
    MOCK_METHOD(int, sub, (int a, int b), (override));
};

TEST(TestAdd, Case1) {
    MockCalc calc;
    EXPECT_CALL(calc, add);

    calc.add(1, 1);
    calc.sub(1, 1);
}
```

在上面的测试用例中，对于`add`设置了期望，下面也确实是调用了该函数，但是，在调用`add`的同时，也调用了`sub`这个没有设置预期的函数。对于这种情况（`sub`的调用）：gmock默认会认为这是一个“未预期的调用”，进而**报警告**

```bash
[ RUN      ] TestAdd.Case1

GMOCK WARNING:
Uninteresting mock function call - returning default value.
    Function call: sub(1, 1)
          Returns: 0
NOTE: You can safely ignore the above warning unless this call should not happen.  Do not suppress it by blindly adding an EXPECT_CALL() if you don't mean to enforce the call.  See https://github.com/google/googletest/blob/main/docs/gmock_cook_book.md#knowing-when-to-expect-useoncall for details.
[       OK ] TestAdd.Case1 (0 ms)
```

在 Case1 中，对 `calc.add(1, 1)` 设置了 `EXPECT_CALL` 但没有对 `calc.sub(1, 1)` 进行任何预期设置。

当调用一个 Mock 函数而没有事先设置对应的 EXPECT_CALL 时，Google Mock 会将这次调用视为“uninteresting”（不感兴趣的调用），然后使用默认返回值（对于整型来说默认为 0）。

提示：如果这个调用本来就不应该发生，那么可能需要检查代码；如果只是你不关心这个调用的结果，就可以忽略这个警告。
Google Mock 建议不要盲目添加 EXPECT_CALL 来压制警告，除非你确实想要验证这个调用是否发生。

```cpp
TEST(TestAdd, Case2) {
    using testing::NiceMock;
    NiceMock<MockCalc> calc;
    EXPECT_CALL(calc, add);

    calc.add(1, 1);
    calc.sub(1, 1);
}
```

这里使用了 `NiceMock<MockCalc>` 包装 `calc` 对象。

NiceMock 的作用是**抑制**对未设置 EXPECT_CALL 的调用发出警告。也就是说，即使没有为 `sub` 方法设置预期，NiceMock 也会默默地接受调用，不打印警告信息。

`EXPECT_CALL(calc, add);` 指定了对 `add` 方法的预期，`calc.add(1, 1)` 被调用后满足预期。

对于 `calc.sub(1, 1)` 的调用，由于 NiceMock 的特性，测试不会因为这个未预期调用而发出警告或错误。

```bash
[ RUN      ] TestAdd.Case2
[       OK ] TestAdd.Case2 (0 ms)
```

测试 Case2 更加宽容，适用于你只关注某些方法调用而对其他方法调用不在意时使用

```cpp
TEST(TestAdd, Case3) {
    using testing::StrictMock;
    StrictMock<MockCalc> calc;
    EXPECT_CALL(calc, add);

    calc.add(1, 1);
    calc.sub(1, 1);
}
```

这里使用了 `StrictMock<MockCalc>` 包装 `calc` 对象。

StrictMock 的作用是对每一个调用都进行严格检查。任何没有明确设置预期的调用都会导致测试失败。

`EXPECT_CALL(calc, add);` 指定了对 `add` 方法的预期，`calc.add(1, 1)` 调用是允许的。

当调用 `calc.sub(1, 1)` 时，因为没有为 `sub` 方法设置 EXPECT_CALL，这个调用被 StrictMock 认为是未预期的，直接报错并导致测试失败。

```bash
[ RUN      ] TestAdd.Case3
unknown file: Failure
Uninteresting mock function call - returning default value.
    Function call: sub(1, 1)
          Returns: 0

[  FAILED  ] TestAdd.Case3 (0 ms)
```

测试 Case3 非常严格，适用于你想确保每个方法调用都被显式验证的场景。这有助于避免漏掉不必要的或意外的调用，但同时也要求测试用例中必须覆盖所有预期的方法调用。

==总结==

- **默认 Mock（Case1）**：会对未预期调用发出警告，但不会导致测试失败。适用于测试中允许某些调用存在，但希望得到提示的情况。
- **NiceMock（Case2）**：自动抑制未预期调用的警告，使得测试更宽松。如果你只关注部分方法调用的预期，这个模式会让测试日志更干净。
- **StrictMock（Case3）**：对所有调用进行严格检查，任何未设置预期的调用都会导致测试失败。适用于需要非常严格控制行为的场景。



### 十四、处理多参数接口

一个函数接口可能有非常多或者是非常复杂的参数，但是我们只需要关注其中的某几个参数或者是哪些简单的参数

```cpp
class ComplexInterface 
{
public:
    virtual int calc(int a, int b, int c, int d, 
                     int e, int f, int g, int h, int i) = 0;
};

class MockComplex : public ComplexInterface
{
public:
    int calc(int a, int b, int c, int d, 
             int e, int f, int g, int h, int i) {
        return _calc(a, h);
    }
    MOCK_METHOD(int, _calc, (int a, int h), ());
};
```

 `MOCK_METHOD(int, _calc, (int a, int h), ());` 是 Google Mock 提供的宏，用来生成一个可被 mock 的方法 `_calc`。这样做可以让我们在测试时设置 `_calc` 方法的期望行为，而无需关心真正的业务逻辑实现

```cpp
using testing::_; // 通配符，表示匹配任意值
using testing::Return;

TEST(TestComplex, Case) {
    MockComplex mcx;
    EXPECT_CALL(mcx, _calc(_, _))
        .WillRepeatedly(Return(10));
    // 这条规则表示：对 mcx 对象的 _calc 方法，任何参数组合的调用都默认返回 10，并且这个行为会一直持续（WillRepeatedly）
    EXPECT_CALL(mcx, _calc(5, 6))
        .WillRepeatedly(Return(100));
    // 这条规则比上面的更具体，当 _calc 方法被以参数 (5, 6) 调用时，将返回 100
    
    EXPECT_EQ(mcx.calc(5, 1, 2, 3, 4, 5, 6, 6, 10), 100);
    EXPECT_EQ(mcx.calc(0, 1, 2, 3, 4, 5, 6, 7, 10), 10);
}
```

Google Mock 会根据匹配的具体性来决定使用哪条规则，当多个规则可以匹配时，<u>**更具体的规则（参数匹配更精确的）优先级更高**</u>

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestComplex
[ RUN      ] TestComplex.Case
[       OK ] TestComplex.Case (0 ms)
[----------] 1 test from TestComplex (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

**总结**：当你面对一个参数非常多的函数，而你只关心其中一两个参数时，可以采用以下策略：

1. **提取关注点**：在被测试函数内部，把真正关心的参数提取出来，转发给一个内部辅助函数。比如上面代码中，`calc` 方法虽然有 9 个参数，但它只把 `a` 和 `h` 传递给了内部的 `_calc` 方法。这种做法使得测试的重点更加明确。
2. **使用 Mock 辅助方法**：利用 Google Mock 的 `MOCK_METHOD` 为这个内部方法创建模拟版本，这样你可以对这两个参数设置期望行为，而忽略其他参数。测试时，可以通过参数匹配器（如 `_` 通配符）来灵活控制返回值。
3. **简化测试**：在测试用例中只针对内部方法的调用设置断言，这样你无需为所有参数编写复杂的匹配规则，而只专注于你关心的那两个参数的值，从而让测试代码更简洁、更易维护。

==当函数接口参数众多但只关心其中一部分时，提取关键参数到一个内部方法，然后对这个内部方法进行 mock，是一种非常有效的测试策略。==这样不仅能保持测试聚焦，还能避免因为其他参数的不必要的细节而增加测试复杂度

> 在 Google Mock 中，当一个成员函数被多次 `EXPECT_CALL` 调用时，**匹配的顺序是根据规则的精确度来决定的**。具体来说，匹配顺序是按照以下几个原则进行的：
>
> 1. 精确匹配优先
>
>     如果定义了多个 `EXPECT_CALL` 规则，其中某一条规则的参数匹配更加精确（即参数值更具体），那么该规则将优先被应用
>
> 2. 顺序匹配（最先匹配的规则先执行）
>
>     如果多个 `EXPECT_CALL` 的规则都能够匹配当前的调用（即它们的匹配条件一样精确），那么 Google Mock 会按照规则定义的顺序来决定使用哪一条规则。
>
> > `WillOnce` 用于指定某个调用的**单次返回值**，它会在第一次匹配时返回设定的值，之后就不再起作用。
> >
> > `WillRepeatedly` 用于指定**每次匹配时都返回相同的值**，它会一直保持有效，直到所有的 `EXPECT_CALL` 都被匹配完
>
> > `_` 是一个通配符，它可以匹配任何传入的参数。因此，在定义 `EXPECT_CALL` 时，使用 `_` 可以匹配更多的调用。
> >
> > 如果你有多个规则，其中一个规则使用了 `_`，那么它会匹配任何参数的情况，但如果有一个更精确的匹配规则（如指定了具体的数值），更精确的规则会优先被执行



### 十五、使用代理

在gmock中使用代理，自定义mock的行为，而不仅仅是做参数校验和返回特定的值

`ON_CALL` 是 Google Mock 提供的一个函数，它的作用是为一个模拟的成员函数设置**默认行为**，即当模拟函数被调用时，如果没有显式的 `EXPECT_CALL` 指定行为时，使用 `ON_CALL` 设置的行为。

- **主要用途**：`ON_CALL` 不会对测试行为进行验证，只是提供一个默认行为，确保在测试过程中被调用的方法会按预期返回结果。也就是说，`ON_CALL` 主要用来设置当模拟对象的函数被调用时默认的返回值，而不涉及期望调用的次数或参数匹配。

- **语法**：

    ```cpp
    ON_CALL(mock_obj, method_name)
        .WillByDefault(some_action);
    ```

`WillByDefault` 是用来指定在没有显式设置期望（`EXPECT_CALL`）的情况下，模拟方法应该执行什么操作或返回什么值。它通常与 `ON_CALL` 一起使用。

- **主要用途**：`WillByDefault` 可以定义模拟方法的默认行为，例如返回某个值、执行某段代码、或进行其他操作。它不会干涉测试用例中的期望行为，且只在没有 `EXPECT_CALL` 设置特定行为时生效。

`WillByDefault` 定义了 `method_name` 方法的默认行为，`some_action` 可以是返回一个固定值、一个函数调用或其他任何合适的操作。

- `ON_CALL` 和 `WillByDefault` 通常是配对使用的。`ON_CALL` 用来指定我们想要为哪些方法设置默认行为，而 `WillByDefault` 则定义了具体的行为或返回值。
- 它们的常见用法是，在某个方法的默认行为没有被明确覆盖时，`WillByDefault` 会提供一个行为，比如返回一个值或执行某个操作。

```cpp
class Calc
{
public:
    virtual int add(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, add, (int a, int b), (override));
};
```

```cpp
TEST(TestAdd, Case) {
    MockCalc calc;
    EXPECT_CALL(calc, add);
    ON_CALL(calc, add)
        .WillByDefault([](int a, int b) {
            return a + b;
        });
    EXPECT_EQ(calc.add(1, 2), 3);
}
```

**`EXPECT_CALL(calc, add)`**：这行代码设定了期望 `calc` 对象的 `add` 方法被调用。这个期望并没有指定参数或返回值，它的作用是监视是否调用了 `add` 方法。

- `EXPECT_CALL` 通常用于设定函数被调用时应该发生的行为，或者验证方法是否按预期被调用。

`ON_CALL` 是 Google Mock 提供的用于设置方法默认行为的方式。它告诉框架，当 `add` 方法被调用时，默认应该返回什么值。

- `WillByDefault([](int a, int b) { return a + b; })` 通过一个 Lambda 表达式指定了 `add` 方法的默认行为：它返回 `a + b` 的和。
- `ON_CALL` 主要用于设置默认行为，而 `EXPECT_CALL` 则用于验证特定方法的调用

```bash
Running main() from /home/will/lesson/gTest/googletest/googletest/src/gtest_main.cc
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from TestAdd
[ RUN      ] TestAdd.Case
[       OK ] TestAdd.Case (0 ms)
[----------] 1 test from TestAdd (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

> `ON_CALL`和`EXPECT_CALL`的区别
>
> **`EXPECT_CALL`**：
>
> - 用于设置期望，验证模拟方法是否按预期被调用。
> - 可以对特定参数设置期望，验证方法是否调用特定次数。
> - 如果期望的调用不发生，测试将失败。
>
> **`ON_CALL`**：
>
> - 用于设置默认行为，适用于不需要特定期望的情况。
> - 不会验证调用次数，主要是定义方法的行为。
> - `ON_CALL` 不会引起测试失败，即使该方法没有被调用。

使用`ON_CALL`和`WillByDefault`的好处

- **简化测试**：`ON_CALL` 和 `WillByDefault` 可以帮助你避免在测试中为每次方法调用都设置期望，尤其当你有很多方法调用时。这使得测试代码更加简洁和易于维护。
- **设置默认行为**：如果你的模拟方法很多，且它们的默认行为都相似，使用 `ON_CALL` 配合 `WillByDefault` 可以减少重复代码。
- **允许灵活性**：你可以对某些方法设置特定的期望（通过 `EXPECT_CALL`），对其他方法只设置默认行为，这为测试提供了更大的灵活性。
