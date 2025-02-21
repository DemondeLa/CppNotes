#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest-matchers.h>
#include <ostream>
#include <string>

class DivMatcher
{
public:
    using is_gtest_matcher = int;

    explicit DivMatcher(int n)
        : _data(n)
    {}
    bool MatchAndExplain(int param, std::ostream *) const {
        std::cout << "param = " << param 
            << ", _data = " << _data 
            << "param % _data => " << (param % _data)
            << "\n";
        return param % _data == 0;
    }
    void DescribeTo(std::ostream *os) const {
        *os << "Can't be divided by " << _data << std::endl;
    }
    void DescribeNegationTo(std::ostream *os) const {
        *os << "Can be divided by " << _data << std::endl;
    }
private:
    int _data;
};

using testing::Matcher;
::testing::Matcher<int> Div(int n) {
    return DivMatcher(n);
}

class Calc
{
public:
    virtual int calc(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, calc, (int a, int b), (override));
};

TEST(TestCalc, Case1)
{
    MockCalc mc;
    EXPECT_CALL(mc, calc(Div(3), Div(5)));

    /* mc.calc(6, 10); */
    mc.calc(7, 10);
}

MATCHER(Div2, (negation ? "is not" : "is") + std::string (" divisible by 2"))
{
    return (arg % 2) == 0;
}

TEST(TestCalc, Case2)
{
    MockCalc mc;
    EXPECT_CALL(mc, calc(Div2(), Div2()));

    mc.calc(6, 10);
    /* mc.calc(7, 10); */
}

MATCHER_P(DivN, n, (negation ? "is" : "is not") + 
          std::string (" divisible by ") + std::to_string(n))
{
    return (arg % n) == 0;
}

TEST(TestCalc, Case3)
{
    MockCalc mc;
    EXPECT_CALL(mc, calc(DivN(3), DivN(5)));

    /* mc.calc(6, 10); */
    mc.calc(7, 10);
}
