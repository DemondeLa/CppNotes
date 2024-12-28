#include <gtest/gtest.h>

TEST(ExpectAssert, Expect1) {
    EXPECT_EQ(1, 2);
    EXPECT_EQ(3, 4);
}

TEST(ExpectAssert, Assert1) {
    ASSERT_EQ(1, 2);
    ASSERT_EQ(3, 4);
}

void sub_routine_expect() {
    EXPECT_EQ(1, 2);
    EXPECT_EQ(3, 4);
}

void sub_routine_assert() {
    {
        ASSERT_EQ(-1, 1);
        ASSERT_EQ(-2, 0);
    }
    ASSERT_EQ(0, 2);
    ASSERT_EQ(3, 4);
}

TEST(ExpectAssert, Expect2) {
    sub_routine_expect();
    EXPECT_EQ(5, 6);
}

TEST(ExpectAssert, Assert2) {
    sub_routine_assert();
    ASSERT_EQ(5, 6);
}
