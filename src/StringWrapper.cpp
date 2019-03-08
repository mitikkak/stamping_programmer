
#include "StringWrapper.h"

String StringWrapper::ToString( int value )
{
#ifdef UNIT_TEST
	return std::to_string(value);
#else
	return String(value);
#endif
}
