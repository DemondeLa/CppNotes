#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>

class MakeMap 
{
public:
    virtual std::map<int, int> make1() = 0;
    virtual std::map<int, int> make2() = 0;
};

class MockMakeMap : public MakeMap
{
public:
    MOCK_METHOD((std::map<int, int>), make1, (), (override));

    using ReturnType = std::map<int, int>;
    MOCK_METHOD(ReturnType, make2, (), (override));
};

TEST(TestMakeMap, Case1)
{
    MockMakeMap makemap;
    EXPECT_CALL(makemap, make1);
    EXPECT_CALL(makemap, make2);

    makemap.make1();
    makemap.make2();
}
