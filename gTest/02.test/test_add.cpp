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
