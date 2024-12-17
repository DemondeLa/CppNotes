#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <gtest/gtest.h>

class Circle
{
public:
    Circle(int r)
    : _r(r)
    , _area(r * r * 3.14)
    {}

    friend class CircleTest;
    FRIEND_TEST(CircleTest, Area);

private:
    int _r;
    double _area;
};

#endif
