#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class ComplexInterface 
{
public:
    virtual int calc(int a, int b, int c, int d, 
                     int e, int f, int g, int h, int i) = 0;
};

class MockComplex : public ComplexInterface
{
public:
    int calc(int a, int b, int c, int d, 
             int e, int f, int g, int h, int i) {
        return _calc(a, h);
    }
    MOCK_METHOD(int, _calc, (int a, int h), ());
};

using testing::_;
using testing::Return;

TEST(TestComplex, Case) {
    MockComplex mcx;
    EXPECT_CALL(mcx, _calc(_, _))
        .WillRepeatedly(Return(10));
    EXPECT_CALL(mcx, _calc(5, 6))
        .WillRepeatedly(Return(100));
    
    EXPECT_EQ(mcx.calc(5, 1, 2, 3, 4, 5, 6, 6, 10), 100);
    EXPECT_EQ(mcx.calc(0, 1, 2, 3, 4, 5, 6, 7, 10), 10);
}
