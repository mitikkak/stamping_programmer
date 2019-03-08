
#include "Phases.h"
#include "StringWrapper.h"
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
	String retValue = "";
	if (not numOfOutputs)
	{
		return "[]";
	}
	for (int i = 0; i < numOfOutputs; i++)
	{
		const Output& o = outputs[i];
		retValue += '[';
		retValue += StringWrapper::ToString(o.line());
		retValue += ':';
		retValue += StringWrapper::ToString(o.period());
		retValue += ']';

	}
	return retValue.c_str();
}
const Output& Phases::operator[](const int idx) const
{
	if (idx < numOfOutputs)
	{
		return outputs[idx];
	}
	return Output::null;
}

}
