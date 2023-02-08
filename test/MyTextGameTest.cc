#include <gtest/gtest.h>
#include <math.h>

//  Test math lib here.
TEST(BasicMathTest, BasicAssertions)
{
    EXPECT_EQ(sqrt(9), 3.f);
    EXPECT_NE(2 * 2, 5);
}

int32_t main(int32_t argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}