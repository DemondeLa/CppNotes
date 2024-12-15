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
