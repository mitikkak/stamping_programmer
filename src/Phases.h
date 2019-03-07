
#include "Output.h"

namespace stamping
{

class Phases
{
public:
	Phases();
	void add(Output o);
	const char* serialize() const;
private:
	static constexpr int maxOutputs = 100;
	Output outputs[maxOutputs];
	int numOfOutputs{0};
};

}
