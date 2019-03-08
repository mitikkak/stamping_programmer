
#include "Program.h"
#include "Print.h"
#include "Arduino.h"

namespace stamping
{

Program::Program(Actuators& actors)
: actuators{actors}
{

}

size_t Program::printTo(Print& p) const
{
	const char* serialized = phases.serialize();
	const int length = strlen(serialized);
	for (int i = 0; i < length; i++)
	{
		p.write(serialized[i]);
	}
	return length;
}
void Program::addPhase(Output const o)
{
	phases.add(o);
}
void Program::run()
{
    actuators.deactivate();
	for (int i = 0; i < phases.size(); i++)
	{
		actuators.deactivate();
		const Output& output = phases[i];
		Actuator& actor = actuators.get(output.line());
		actor.activate();
		delay(output.period());
		actor.deactivate();
	}
}
#ifdef UNIT_TEST
#include <cassert>
const Actuator& Program::actuator(const int idx) const
{
	assert(idx < actuators.size());
	return actuators[idx];
}
#endif

}
