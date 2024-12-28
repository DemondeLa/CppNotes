#include <gtest/gtest.h>
#include <iostream>

class SharedResource : public testing::Test
{
public:
    static int *ptr;
public:
    static void SetUpTestSuite() {
        std::cout << "SetUpTestSuite \n";
        ptr = new int(100);
    }
    static void TearDownTestSuite() {
        std::cout << "TearDownTestSuite \n";
        delete ptr;
        ptr = nullptr;
    }
};

int *SharedResource::ptr = nullptr;

TEST_F(SharedResource, case1) {
    ASSERT_EQ(100, *SharedResource::ptr);
}

TEST_F(SharedResource, case2) {
    ASSERT_EQ(100, *SharedResource::ptr);
}
