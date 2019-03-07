
#include "Phases.h"
#include <string>
namespace stamping
{
Phases::Phases()
: outputs{}, numOfOutputs{0}
{

}

void Phases::add(Output o)
{
	if (numOfOutputs < maxOutputs)
	{
		outputs[numOfOutputs++] = o;
	}
}
const char* Phases::serialize() const
{
	std::string retValue = "";
	if (not numOfOutputs)
	{
		return "[]";
	}
	for (int i = 0; i < numOfOutputs; i++)
	{
		const Output& o = outputs[i];
		retValue += '[';
		retValue += std::to_string(o.line());
		retValue += ':';
		retValue += std::to_string(o.period());
		retValue += ']';

	}
	return retValue.c_str();
}

}
