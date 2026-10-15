//
// Created by artyk on 10/15/26.
//

#include <gtest/gtest.h>

TEST(TestingInfrastructure, BasicAssertion) {
	EXPECT_EQ(2 + 2, 4);
}


//just to trigger a failure; comment this out afterwards
TEST(TestingInfrastructure, FailAssertion) {
	EXPECT_EQ(2 + 2, 5);
}
