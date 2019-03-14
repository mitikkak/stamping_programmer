
#include "StringWrapper.h"

STRING_TYPE StringWrapper::ToString( int value )
{
#ifdef UNIT_TEST
	return std::to_string(value);
#else
	return STRING_TYPE(value);
#endif
}
