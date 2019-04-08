#include <gtest/gtest.h>
#include "Verification.h"

using namespace stamping;

class TestVerification : public ::testing::Test {
public:

	TestVerification()
	: numberOfOutputs{4}
    {

    }
    void SetUp(){
    }

    void TearDown() {

    }
protected:
    const int numberOfOutputs{};
};
TEST_F(TestVerification, initialState)
{
	const std::string progStr{""};
	Verification v;
	EXPECT_EQ(false, v.passed());
}
TEST_F(TestVerification, onePhaseOk)
{
	const std::string progStr{"[1:1]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(true, v.passed());
	EXPECT_EQ(1, v.numOfPhases);
}
TEST_F(TestVerification, threePhasesOk)
{
	const std::string progStr{"[1:10][2:254665][1:0]"};
	Verification v;
	std::string retStr = v.check(progStr, numberOfOutputs);
	EXPECT_STREQ("Ok", retStr.c_str());
	EXPECT_EQ(true, v.passed());
	EXPECT_EQ(3, v.numOfPhases);
}
TEST_F(TestVerification, failsAtZeroActuator)
{
	const std::string progStr{"[0:0]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsAtWrongActuator)
{
	const std::string progStr{"[5:1]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsAtSomethingBetweenPhases)
{
	const std::string progStr{"[1:1] [2:2]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsWithoutPhases)
{
	const std::string progStr{"sad"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsWithoutMeaningfulPhases)
{
	const std::string progStr{"[sad:sad]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsWithoutStartSeparator)
{
	const std::string progStr{"[1:1]1:2]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsWithoutMiddleSeparator)
{
	const std::string progStr{"[1:1][12]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsWithoutEndSeparator)
{
	const std::string progStr{"[1:2[1:1]"};
	Verification v;
	v.check(progStr, numberOfOutputs);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}