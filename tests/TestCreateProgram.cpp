#include <gtest/gtest.h>
#include "Program.h"
#include "SerialPrinter.h"


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
	stamping::Program program;
	SerialPrinter printer;
	program.printTo(printer);
	EXPECT_STREQ("[]", printer.output());
}
