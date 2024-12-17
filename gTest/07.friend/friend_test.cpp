#include "Circle.h"
#include <gtest/gtest.h>

TEST(CircleTest, Area) {
    Circle c(5);
    EXPECT_EQ(c._area, 78.5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
