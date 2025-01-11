#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Calc
{
public:
    virtual int Do(int a, int b) = 0;
    virtual int Work(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, Do, (int a, int b), (override));
    MOCK_METHOD(int, Work, (int a, int b), (override));
};

int UseCalc(Calc &c, int a, int b, bool d) {
    return d ? c.Do(a, b) : c.Work(a, b);
}

using testing::_;
using testing::Return;
using testing::InSequence;

TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    
    EXPECT_CALL(calc, Do);
    EXPECT_CALL(calc, Work);
    
    UseCalc(calc, 5, 10, false);
    UseCalc(calc, 5, 10, true);
}

TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    
    InSequence seq;

    EXPECT_CALL(calc, Do);
    EXPECT_CALL(calc, Work);
    
    UseCalc(calc, 5, 10, false);
    UseCalc(calc, 5, 10, true);
}
