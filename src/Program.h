
#include "Printable.h"

namespace stamping
{

class Phases
{
public:
	const char* serialize() const
	{
		return "Nada";
	}
};
class Program : public Printable
{
public:
	virtual ~Program() {}
	size_t printTo(Print& p) const;
private:
	Phases phases;
};

}
