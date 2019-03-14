
#include "Phases.h"

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

void Phases::serialize(STRING_TYPE& retValue) const
{
	retValue = "";
	if (not numOfOutputs)
	{
		retValue = "[]";
		return;
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
