#include "func.h"
#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

// 定义模板类
template <typename T>
class TypedTestClass : public ::testing::Test{};

// 定义参数化测试套件
TYPED_TEST_SUITE_P(TypedTestClass);

// 测试用例场景
TYPED_TEST_P(TypedTestClass, AddTest) {
    TypeParam a = 1;
    TypeParam b = 2;
    EXPECT_EQ(a + b, 3);
}

TYPED_TEST_P(TypedTestClass, SubTest) {
    TypeParam a = 6;
    TypeParam b = 2;
    EXPECT_EQ(a - b, 4);
}

// 注册测试
REGISTER_TYPED_TEST_SUITE_P(TypedTestClass, AddTest, SubTest);

// 实例化
using MyTypeClass = ::testing::Types<int, float, long, double>;
INSTANTIATE_TYPED_TEST_SUITE_P(MyPrefix, TypedTestClass, MyTypeClass);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
