#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Calc
{
public:
    virtual int Do(int a, int b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, Do, (int a, int b), (override));
};

int UseCalc(Calc &c, int a, int b) {
    return c.Do(a, b);
}

using testing::_;

TEST(CalcTest, MockCalcTestCase1) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(0);
}

TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(3);

    UseCalc(calc, 0, 1);
    UseCalc(calc, 2, 3);
    UseCalc(calc, 4, 5);
}

TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(0);
    UseCalc(calc, 0, 1);
}

TEST(CalcTest, MockCalcTestCase4) {
    MockCalc calc;
    EXPECT_CALL(calc, Do).Times(1);

    UseCalc(calc, 0, 1);
    UseCalc(calc, 2, 3);
}
