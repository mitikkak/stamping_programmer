
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
void Phases::insert(unsigned const index, Output o)
{
	if (index < maxOutputs)
	{
		if (index < numOfOutputs)
		{
			for (int i = numOfOutputs; i > index; i--)
			{
//				printf("insert[%d] o[%d]: [%d.%d] = o[%d]: [%d.%d] \n", i, i+1, outputs[i+1].line(), outputs[i+1].period(), i, outputs[i].line(), outputs[i].period());
				outputs[i] = outputs[i-1];
			}
			sureInsert(index, o);
		}
		else
		{
			sureInsert(numOfOutputs, o);
		}
	}
}
void Phases::sureInsert(unsigned const index, Output o)
{
	outputs[index] = o;
	numOfOutputs++;
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
