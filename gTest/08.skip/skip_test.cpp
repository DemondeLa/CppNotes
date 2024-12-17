#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(SkipTest, case1) {
    EXPECT_EQ(2,  add(1, 1));
}

TEST(SkipTest, case2) {
    GTEST_SKIP() << "Skipping this test";
    EXPECT_EQ(4,  add(2, 3));
}

TEST(SkipTest, case3) {
    GTEST_FAIL() << "Failed this test && end test";
    EXPECT_EQ(5,  add(2, 3));
}

TEST(SkipTest, case4) {
    EXPECT_EQ(4,  add(2, 2));
}
