#include <gtest/gtest.h>

TEST(FooTest, Foo1) {
    EXPECT_EQ(1, 1);
    EXPECT_EQ(2, 2);
}

TEST(FooTest, Foo2) {
    EXPECT_EQ(3, 3);
    EXPECT_EQ(4, 4);
}

TEST(BarTest, Bar1) {
    EXPECT_EQ(-1, -1);
    EXPECT_EQ(-2, -2);
}

TEST(BarTest, Bar2) {
    EXPECT_EQ(-1, 0);
    EXPECT_EQ(-2, 0);
}
