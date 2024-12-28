#include <gtest/gtest.h>
#include <ostream>

class Circle {
public:
    Circle (double r)
    : _r(r)
    , _area(3.14 * _r * _r)
    {
        std::cout << "Circle(r)\n";
    }
    
    bool operator==(const Circle &other) const {
        return (_r == other._r) && (_area == other._area);
    }

    friend void PrintTo(const Circle &circle, std::ostream *os) {
        *os << "Circle: r = " << circle._r << ", area = " << circle._area << "\n";
    }

private:
    double _r;
    double _area;
};

Circle make_circle(double r) {
    return Circle(r);
}

TEST(CircleTest, case1) {
    EXPECT_EQ(make_circle(5.0), Circle(4.9));
}
