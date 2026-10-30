//
// Created by artyk on 10/30/26.
//

#include <cstdio>
#include <string>
#include <iostream>

#include <gtest/gtest.h>
#include <gtest/internal/gtest-port.h>

#define DEBUGMODE 0

#include "bigint/utils.hpp"

TEST(DebugOutDisabled, Printf) {
	
	testing::internal::CaptureStdout();
	
	//DO NOT MOVE THESE 2 LINES
	//THEY MUST STAY TOGETHER or test will always fail.
	//because of the silly line check
	const int call_line = __LINE__+1;
	dprintf("Answer to life, universe, and everything: %d", 42);
	
	std::fflush(stdout);
	const std::string actual = testing::internal::GetCapturedStdout();
	
	//const std::string expected = std::string("[DEBUG] [") + __FILE__ + "] [" + __func__  + "()::" + std::to_string(call_line) + "] -> Answer to life, universe, and everything: 42\n";
	const std::string expected = "";
	
	EXPECT_EQ(actual, expected);
}

TEST(DebugOutDisabled, Cout) {
	testing::internal::CaptureStdout();
	
	//DO NOT MOVE THESE 2 LINES
	//THEY MUST STAY TOGETHER or test will always fail.
	//because of the silly line check
	const int call_line = __LINE__+1;
	cdbg<<"Answer to life, universe, and everything: 42\n";
	
	
	std::fflush(stdout);
	const std::string actual = testing::internal::GetCapturedStdout();
	
	//const std::string expected = std::string("[DEBUG] [") + __FILE__ + "] [" + __func__  + "()::" + std::to_string(call_line) + "] -> Answer to life, universe, and everything: 42\n";
	const std::string expected = "";
	
	EXPECT_EQ(actual, expected);
}