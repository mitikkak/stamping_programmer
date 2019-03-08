#include <gtest/gtest.h>
#include "Program.h"
#include "PinEvents.h"

using namespace stamping;

class TestRunProgram : public ::testing::Test {
public:

	TestRunProgram()
	: actuators{},
	  a1{1},
	  a2{2},
	  a3{3},
	  a4{4}
    {

    }
    void SetUp(){
    	printf("SetUp \n");
    	receivedEvents.start();
    	actuators.add(a1);
    	actuators.add(a2);
    	actuators.add(a3);
    	actuators.add(a4);
    }

    void TearDown() {
    	printf("TearDown \n");
    	receivedEvents.stop();

    }
protected:
	Actuators actuators;
	Actuator a1;
	Actuator a2;
	Actuator a3;
	Actuator a4;
	void addDeactivationOfAll(std::vector<PinEvent>& expectedEvents) const
	{
		expectedEvents.push_back(PinEvent(a1.pin(), Actuator::OFF, 0));
		expectedEvents.push_back(PinEvent(a2.pin(), Actuator::OFF, 0));
		expectedEvents.push_back(PinEvent(a3.pin(), Actuator::OFF, 0));
		expectedEvents.push_back(PinEvent(a4.pin(), Actuator::OFF, 0));
	}
};
#if 1
TEST_F(TestRunProgram, empty)
{
	Program prog(actuators);
	prog.run();
	std::vector<PinEvent> expectedEvents{};
	addDeactivationOfAll(expectedEvents);
	receivedEvents.verify(expectedEvents);
}
TEST_F(TestRunProgram, simplest)
{
	Program prog(actuators);
	Output o1(0, 5);

	prog.addPhase(o1);
	prog.run();
	std::vector<PinEvent> expectedEvents{};
	addDeactivationOfAll(expectedEvents); // Initial
	addDeactivationOfAll(expectedEvents); // begin 1st phase
	expectedEvents.push_back(PinEvent(a1.pin(), Actuator::ON, 0));
	expectedEvents.push_back(PinEvent(a1.pin(), Actuator::OFF, o1.period()));
	receivedEvents.verify(expectedEvents);
}
#endif
TEST_F(TestRunProgram, longStory)
{
	Program prog(actuators);
	std::vector<Output> outputs{
		{3, 10}/*,
		{2, 30},
		{1, 34},
		{0, 67},
		{0, 45},
		{3, 900},
		{2, 10000},
		{0, 0},
		{1, 555}*/
	};
	for (const Output& o : outputs)
	{
		prog.addPhase(o);
	}
	prog.run();
	std::vector<PinEvent> expectedEvents{};
	addDeactivationOfAll(expectedEvents); // Initial
	unsigned long time = 0;
	for (const Output& o : outputs)
	{
		addDeactivationOfAll(expectedEvents); // Initial
		const int pin = prog.actuator(o.line()).pin();
		expectedEvents.push_back(PinEvent(pin, Actuator::ON, time));
		expectedEvents.push_back(PinEvent(pin, Actuator::OFF, time + o.period()));
		time += o.period();
	}
	receivedEvents.verify(expectedEvents);
}
