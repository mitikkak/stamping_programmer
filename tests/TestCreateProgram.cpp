#include <gtest/gtest.h>
#include "Program.h"
#include "SerialPrinter.h"

using namespace stamping;

class TestCreateProgram : public ::testing::Test {
public:

	TestCreateProgram() {}
    void SetUp() {
    }

    void TearDown() {

    }
protected:

};

TEST_F(TestCreateProgram, initiallyEmpty)
{
	Program program;
	SerialPrinter printer;
	program.printTo(printer);
	EXPECT_STREQ("[]", printer.output());
}
TEST_F(TestCreateProgram, onePhase)
{
	Program program;
	SerialPrinter printer;
	program.addPhase(Output(1, 500));
	program.printTo(printer);
	EXPECT_STREQ("[1:500]", printer.output());
}
TEST_F(TestCreateProgram, twoPhases)
{
	Program program;
	SerialPrinter printer;
	program.addPhase(Output(4, 13));
	program.addPhase(Output(2, 20000));
	program.printTo(printer);
	EXPECT_STREQ("[4:13][2:20000]", printer.output());
}
