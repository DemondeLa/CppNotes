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
    EXPECT_CALL(calc, Do(5, _));

    UseCalc(calc, 5, 10);
}

TEST(CalcTest, MockCalcTestCase2) {
    MockCalc calc;
    EXPECT_CALL(calc, Do(testing::Gt(5), testing::Lt(19)));

    UseCalc(calc, 6, 9);
}

TEST(CalcTest, MockCalcTestCase3) {
    MockCalc calc;
    EXPECT_CALL(calc, Do);

    UseCalc(calc, -1, 0) ;
}
