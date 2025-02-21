#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class Calc
{
public:
    virtual int calc(int a, int b) = 0;
    virtual ~Calc() = default;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(int, calc, (int a, int b), (override));
};

int UseCalcAndDelete(Calc *c, int a, int b)
{
    int ret = c->calc(a, b);
    /* delete c; */
    return ret;
}

TEST(TestCalc, Case1) 
{
    MockCalc *pmc = new MockCalc();
    EXPECT_CALL(*pmc, calc(2, 3))
        .WillOnce(::testing::Return(5));
    EXPECT_EQ(5, UseCalcAndDelete(pmc, 2, 3));

    ::testing::Mock::VerifyAndClearExpectations(pmc);
    /* delete pmc; */
}
