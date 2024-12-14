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

