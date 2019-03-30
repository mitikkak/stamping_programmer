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
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.printTo(printer);
	EXPECT_STREQ("[]", printer.output());
}
TEST_F(TestCreateProgram, onePhase)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(1, 500));
	program.printTo(printer);
	EXPECT_STREQ("[1:500]", printer.output());
}
TEST_F(TestCreateProgram, twoPhases)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(4, 13));
	program.addPhase(Output(2, 20000));
	program.printTo(printer);
	EXPECT_STREQ("[4:13][2:20000]", printer.output());
}
TEST_F(TestCreateProgram, insertPhaseInMiddle)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	program.insertPhase(1, Output(44, 4466));
	program.printTo(printer);
	EXPECT_STREQ("[15:2222][44:4466][20:3333]", printer.output());
}
TEST_F(TestCreateProgram, insertPhaseInHead)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	program.insertPhase(0, Output(44, 4466));
	program.printTo(printer);
	EXPECT_STREQ("[44:4466][15:2222][20:3333]", printer.output());
}
TEST_F(TestCreateProgram, insertPhaseInTail)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	program.insertPhase(Phases::maxOutputs-1, Output(44, 4466));
	program.printTo(printer);
	EXPECT_STREQ("[15:2222][20:3333][44:4466]", printer.output());
}
TEST_F(TestCreateProgram, insertProgramInMiddle)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	Program program2(actuators);
	program2.insertPhase(Phases::maxOutputs-1, Output(44, 4466));
	program2.insertPhase(Phases::maxOutputs-1, Output(55, 5577));
	program.insert(1, program2);
	program.printTo(printer);
	EXPECT_STREQ("[15:2222][44:4466][55:5577][20:3333]", printer.output());
}
TEST_F(TestCreateProgram, insertProgramInHead)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	Program program2(actuators);
	program2.insertPhase(Phases::maxOutputs-1, Output(44, 4466));
	program2.insertPhase(Phases::maxOutputs-1, Output(55, 5577));
	program.insert(0, program2);
	program.printTo(printer);
	EXPECT_STREQ("[44:4466][55:5577][15:2222][20:3333]", printer.output());
}
TEST_F(TestCreateProgram, insertProgramInTail)
{
	Actuators actuators;
	Program program(actuators);
	SerialPrinter printer;
	program.addPhase(Output(15, 2222));
	program.addPhase(Output(20, 3333));
	Program program2(actuators);
	program2.insertPhase(Phases::maxOutputs-1, Output(44, 4466));
	program2.insertPhase(Phases::maxOutputs-1, Output(55, 5577));
	program.insert(43, program2);
	program.printTo(printer);
	EXPECT_STREQ("[15:2222][20:3333][44:4466][55:5577]", printer.output());
}
TEST_F(TestCreateProgram, loadProgram)
{
	Actuators actuators;
	std::string savedProgram = "[12:1256][14:329098][348:21432]";
	Program program(actuators, savedProgram.c_str());
	SerialPrinter printer;
	program.printTo(printer);
	EXPECT_STREQ(savedProgram.c_str(), printer.output());
}
