
#include "Program.h"
#include "Print.h"

namespace stamping
{
size_t Program::printTo(Print& p) const
{
	const char* serialized = "Naada";
	const int length = strlen(serialized);
	for (int i = 0; i < length; i++)
	{
		p.write(serialized[i]);
	}
	return length;
}
}
