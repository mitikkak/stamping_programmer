
#pragma once
#include "Output.h"
#include "StringWrapper.h"
namespace stamping
{

class Phases
{
public:
	Phases();
	void add(Output o);
	void insert(unsigned const index, Output o);
	void serialize(STRING_TYPE&) const;
	int size() const { return numOfOutputs; }
	const Output& operator[](const int) const;
private:
	void sureInsert(unsigned const index, Output o);
	static constexpr int maxOutputs = 100;
	Output outputs[maxOutputs];
	int numOfOutputs{0};
};

}
