
#include "Printable.h"
#include "Phases.h"
#include "Actuators.h"
namespace stamping
{

class Program : public Printable
{
public:
	Program(Actuators& actors);
	// Constructor to re-create program from text format.
	Program(Actuators& actors, const char* const);
	virtual ~Program() {}
	size_t printTo(Print& p) const;
	void addPhase(Output const o);
	void insertPhase(unsigned index, Output const o);
	void insert(unsigned index, const Program&);
	void run();
	int length() const;
	const Actuator& actuator(const int) const; // testability
private:
	Phases phases;
	Actuators& actuators;
};

}
