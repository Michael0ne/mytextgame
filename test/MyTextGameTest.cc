#include <gtest/gtest.h>

//  Test math lib here.
TEST(BasicMathTest, BasicAssertions)
{
    EXPECT_EQ(sqrtf(9), 3.f);
    EXPECT_NE(2 * 2, 5);
}

int32_t main(int32_t argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}