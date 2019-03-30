
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
	static constexpr int maxOutputs = 50;
private:
	void sureInsert(unsigned const index, Output o);
	Output outputs[maxOutputs];
	int numOfOutputs{0};
};

}
