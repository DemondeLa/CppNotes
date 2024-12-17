#include <gtest/gtest.h>
#include <string>

int throw_func(int a) {
    if (a == 0)
        throw "the number is 0";
    return 0;
}

TEST(ExpectThrowTest, ExpectThrow) {
    EXPECT_THROW(throw_func(0), const char *); // not string
}

TEST(ExpectThrowTest, ExpectNoThrow) {
    EXPECT_NO_THROW(throw_func(1));
}

TEST(ExpectThrowTest, ExpectThrowAnyValue) {
    EXPECT_ANY_THROW(throw_func(0));
}
