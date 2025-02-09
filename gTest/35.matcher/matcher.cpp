#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock-matchers.h>

class Calc
{
public:
    virtual int add(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, add, (int a, int b), (override));
};

using testing::_;
using testing::AllOf;
using testing::Gt;
using testing::Lt;
using testing::Matches;
using testing::Return;
using testing::Truly;

TEST(TestAdd, Case1)
{
    MockCalc calc;
    EXPECT_CALL(calc, add(_, _))
        .WillRepeatedly(Return(100));
    EXPECT_CALL(calc, add(AllOf(Gt(3), Lt(6)), _))
        .WillRepeatedly(Return(10));

    EXPECT_EQ(calc.add(5, 6), 10);
    EXPECT_EQ(calc.add(0, 6), 100);
}

TEST(TestAdd, Case2)
{
    MockCalc calc;
    EXPECT_CALL(calc, add(_, _))
        .WillRepeatedly(Return(100));
    EXPECT_CALL(calc, add(Truly([](int a) {
            return a > 3 && a < 6; 
            }), _))
        .WillRepeatedly(Return(10));

    EXPECT_EQ(calc.add(5, 6), 10);
    EXPECT_EQ(calc.add(0, 6), 100);
}

#include <vector>
#include <algorithm>

TEST(TestMatcher, Case) 
{
    std::vector<int> data {1, 2, 3, 4, 5, 6};
    auto t = std::find_if(data.begin(), data.end(), Matches(Gt(4)));
    EXPECT_EQ(*t, 5);
}
