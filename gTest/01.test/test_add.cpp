#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}

TEST(Add, AddTwoNumber) {
    ASSERT_EQ(add(1, 2), 3);
}

TEST(Add, AddTwoNumber2) {
    ASSERT_EQ(add(3, 4), 1);
}
