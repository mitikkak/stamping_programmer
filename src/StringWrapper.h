
#pragma once

#ifdef UNIT_TEST
#include <string>
typedef std::string STRING_TYPE;
#else
#include "Arduino.h"
typedef String STRING_TYPE;
#endif

class StringWrapper
{
public:
	static STRING_TYPE ToString( int value );

};
