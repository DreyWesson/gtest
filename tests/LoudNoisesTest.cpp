#include "gtest/gtest.h"
#include "../inc/LoudNoises.hpp"

TEST(LoudNoisesTest, SingleArgument) {
    char* args[] = {(char*)"program", (char*)"hello"};
    int argc = 2;
    std::string expected = "HELLO";
    std::string result = makeLoudNoises(argc, args);
    EXPECT_EQ(result, expected);
}

TEST(LoudNoisesTest, MultipleArguments) {
    char* args[] = {(char*)"program", (char*)"hello", (char*)"world"};
    int argc = 3;
    std::string expected = "HELLOWORLD";
    std::string result = makeLoudNoises(argc, args);
    EXPECT_EQ(result, expected);
}

TEST(LoudNoisesTest, NoArguments) {
    char* args[] = {(char*)"program"};
    int argc = 1;
    std::string expected = "* LOUD AND UNBEARABLE FEEDBACK NOISE *";
    std::string result = makeLoudNoises(argc, args);
    EXPECT_EQ(result, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
