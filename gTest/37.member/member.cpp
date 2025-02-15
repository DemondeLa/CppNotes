#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gmock/gmock-function-mocker.h>
#include <gmock/gmock-matchers.h>
#include <string>
#include <ostream>

struct Complex 
{
    double r;
    double i;
    std::string toString() const {
        if (r == 0) {
            if (i == 0)
                return "0";
            else 
                return std::to_string(i) + "i";
        } else {
            if (i == 0)
                return std::to_string(r);
            else 
                return std::to_string(r) + "+" + std::to_string(i) + "i";
        }
    }
    Complex Add(const Complex& a) const {
        return Complex {r + a.r, i + a.i};
    }
};

class Calc
{
public:
    virtual Complex calc(Complex a, Complex b) = 0;
};

class MockCalc : public Calc
{
public:
    MOCK_METHOD(Complex, calc, (Complex a, Complex b), (override));
};

using testing::_;
using testing::Field;
using testing::Gt;

void PrintTo(const Complex &c, std::ostream *out) {
    *out << c.toString();
}

TEST(TestCalc, Case1)
{
    MockCalc calc;
    EXPECT_CALL(calc, calc(Field("parameter1.real part", &Complex::r, Gt(0)), _));

    calc.calc (Complex{0, 2}, Complex{1, 2});
}

using testing::Property;
using testing::StartsWith;
TEST(TestCalc, Case2) 
{
    MockCalc calc;
    EXPECT_CALL(calc, calc(
        Property("toString", &Complex::toString, StartsWith("3")), _));
    calc.calc(Complex {3, 2}, Complex {1, 2});
}
using testing::Truly;
TEST(TestCalc, Case4) 
{
    MockCalc calc;
    EXPECT_CALL(calc, calc(
        Truly([](const Complex& a) {
            Complex b {1, 2};  
            auto ret = a.Add(b);
            return ret.r == 4 && ret.i == 6;
        }), _));
    calc.calc(Complex {3, 4}, Complex {1, 2});
}

using testing::AllOf;
TEST(TestCalc, Case3) 
{
    MockCalc calc;
    EXPECT_CALL(calc, calc(AllOf(
        Field("r", &Complex::r, Gt(0)),
        Field("i", &Complex::i, Gt(10))), _));
    calc.calc(Complex {1, 11}, Complex {1, 11});
}
