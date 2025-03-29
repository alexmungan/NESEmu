#include <gtest/gtest.h>

// A simple function to test
int add(int a, int b) {
    return a + b;
}

// Test case for the add function
TEST(AdditionTest, PositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);  // Test that 2 + 3 == 5
}