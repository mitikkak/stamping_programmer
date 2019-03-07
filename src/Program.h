
#include "Printable.h"
#include "Phases.h"
namespace stamping
{

class Program : public Printable
{
public:
	virtual ~Program() {}
	size_t printTo(Print& p) const;
	void addPhase(Output const o);
private:
	Phases phases;
};

}
