
#include "Program.h"
#include "Print.h"
#include "Arduino.h"
#include "StringWrapper.h"

namespace stamping
{

Program::Program(Actuators& actors)
: actuators(actors)
{

}
Program::Program(Actuators& actors, const char* prgStr, const size_t programLength)
: actuators(actors)
{
    fill(prgStr, 0, programLength);
}
void Program::fill(const char* prgStr, const size_t beginIndex, const size_t endIndex)
{
    phases.reset();
	const size_t length = endIndex - beginIndex; //strlen(prgStr);
//	printf("%lu, %lu, %lu, %lu \n", prgStr.length(), beginIndex, endIndex, length);
    STRING_TYPE lineStr;
    STRING_TYPE periodStr;
    STRING_TYPE* currentStrPtr = &lineStr;
	for (int i = beginIndex; i < endIndex; i++)
	{
		char const c = prgStr[i];
//		printf("c: %c \n", c);
		if (c == '[')
		{
			// begin new output
			lineStr = "";
			periodStr = "";
			currentStrPtr = &lineStr;
//			printf("begin new output %p [%p][%p] \n", currentStrPtr, &lineStr, &periodStr);
		}
		else if (c == ']')
		{
			const int line = atoi(lineStr.c_str());
			const int period = atoi(periodStr.c_str());
			Output o(line, period);
//			printf("Adding phase [%u:%u] \n", o.line(), o.period());
			addPhase(o);

		}
		else if (c == ':')
		{
			currentStrPtr = &periodStr;
//			printf("separator %p [%p][%p] \n", currentStrPtr, &lineStr, &periodStr);
		}
		else
		{

			(*currentStrPtr) += c;
		}
	}
}

size_t Program::printTo(Print& p) const
{
	STRING_TYPE serialized = "";
	phases.serialize(serialized);
	const int length = strlen(serialized.c_str());
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
    actuators.off();
	for (int i = 0; i < phases.size(); i++)
	{
		actuators.off();
		const Output& output = phases[i];
		Actuator& actor = actuators.get(output.line()-1);
		actor.on();
		delay(output.period());
		actor.off();
		if (interphaseDelay)
		{
			delay(interphaseDelay);
		}
	}
}
void Program::insertPhase(unsigned index, Output const o)
{
	phases.insert(index, o);
}
void Program::insert(unsigned index, const Program& another)
{
	for (int i = 0; i < another.length(); i++)
	{
		phases.insert(index+i, another.phases[i]);
	}
}
int Program::length() const
{
	return phases.size();
}
void Program::setInterphaseDelay(const int val)
{
	interphaseDelay = val;
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
