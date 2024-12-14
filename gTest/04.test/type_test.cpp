#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

template <typename T>
class TypeTestClass : public ::testing::Test {};

using MyTypes = ::testing::Types<int, double, float>;
TYPED_TEST_SUITE(TypeTestClass, MyTypes);

TYPED_TEST(TypeTestClass, TTest) {
    TypeParam x = 1;
    TypeParam y = 2;
    TypeParam z = x + y;
    EXPECT_EQ(z, 3);
}
