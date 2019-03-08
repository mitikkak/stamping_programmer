
#include "Printable.h"
#include "Phases.h"
#include "Actuators.h"
namespace stamping
{

class Program : public Printable
{
public:
	Program(Actuators& actors);
	virtual ~Program() {}
	size_t printTo(Print& p) const;
	void addPhase(Output const o);
	void run();
	const Actuator& actuator(const int) const; // testability
private:
	Phases phases;
	Actuators& actuators;
};

}
