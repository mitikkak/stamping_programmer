#include <gtest/gtest.h>
#include "Verification.h"

using namespace stamping;

class TestVerification : public ::testing::Test {
public:

	TestVerification()
    : actuators{},
      a1{1},
      a2{2},
      a3{3},
      a4{4}
    {

    }
    void SetUp(){
    	actuators.add(a1);
    	actuators.add(a2);
    	actuators.add(a3);
    	actuators.add(a4);
    }

    void TearDown() {

    }
protected:
	Actuators actuators;
	Actuator a1;
	Actuator a2;
	Actuator a3;
	Actuator a4;
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
	v.check(progStr, actuators);
	EXPECT_EQ(true, v.passed());
	EXPECT_EQ(1, v.numOfPhases);
}
TEST_F(TestVerification, threePhasesOk)
{
	const std::string progStr{"[1:10][2:254665][1:0]"};
	Verification v;
	std::string retStr = v.check(progStr, actuators);
	EXPECT_STREQ("Ok", retStr.c_str());
	EXPECT_EQ(true, v.passed());
	EXPECT_EQ(3, v.numOfPhases);
}
TEST_F(TestVerification, failsAtZeroActuator)
{
	const std::string progStr{"[0:0]"};
	Verification v;
	v.check(progStr, actuators);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsAtWrongActuator)
{
	const std::string progStr{"[5:1]"};
	Verification v;
	v.check(progStr, actuators);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
TEST_F(TestVerification, failsAtSomethingBetweenPhases)
{
	const std::string progStr{"[1:1] [2:2]"};
	Verification v;
	v.check(progStr, actuators);
	EXPECT_EQ(false, v.passed());
	EXPECT_EQ(0, v.numOfPhases);
}
