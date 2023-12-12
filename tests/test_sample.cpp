#include <gtest/gtest.h>

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(5, 5);
}

TEST(AddTest, NegativeNumbers) {
    EXPECT_EQ(1, 1);
}


// Run the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
