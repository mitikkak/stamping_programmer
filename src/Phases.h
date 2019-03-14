
#include "Output.h"
#include "StringWrapper.h"
namespace stamping
{

class Phases
{
public:
	Phases();
	void add(Output o);
	void serialize(STRING_TYPE&) const;
	int size() const { return numOfOutputs; }
	const Output& operator[](const int) const;
private:
	static constexpr int maxOutputs = 100;
	Output outputs[maxOutputs];
	int numOfOutputs{0};
};

}
