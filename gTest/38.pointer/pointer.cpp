#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

class Calc
{
public:
    virtual int calc(int *pa, int *pb) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, calc, (int *pa, int *pb), (override));
};

using testing::Pointee;
using testing::Gt;
using testing::Lt;

TEST(TestCalc, Case1)
{
    MockCalc mc;
    EXPECT_CALL(mc, calc(Pointee(Gt(3)), Pointee(Lt(6))));

    int a = 10, b = 5;
    mc.calc(&a, &b);
    /* mc.calc(nullptr, &b); */
}

using testing::Truly;
TEST(TestCalc, Case2)
{
    MockCalc mc;
    EXPECT_CALL(mc, calc(Truly([](int *p) {
            /* return *p > 3; */
            return p != nullptr && *p > 3;
        }), Truly([](int *p){
            /* return *p < 6; */
            return p != nullptr && *p < 3;
        })));

    int a = 10, b = 5;
    /* mc.calc(&a, &b); */
    mc.calc(nullptr, &b);
}
