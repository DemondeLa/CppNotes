#include "add.h"
#include <gtest/gtest.h>

class AddTest : public ::testing::Test
{
public:
    AddTest() {
        std::cout << "AddTest\n";
    }
    ~AddTest() {
        std::cout << "~AddTest\n";
    }
    void SetUp() override {
        std::cout << "SetUp\n";
    }
    void TearDown() override {
        std::cout << "TearDown\n";
    }
};

TEST_F(AddTest, cast1) {
    std::cout << "case1: \n";
    EXPECT_EQ(add(1, 2), 3);
    EXPECT_EQ(add(3, 4), 7);
}

TEST_F(AddTest, cast2) {
    std::cout << "case2: \n";
    EXPECT_EQ(add(-1, 1), 0);
    EXPECT_EQ(add(-1, -1), -2);

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
