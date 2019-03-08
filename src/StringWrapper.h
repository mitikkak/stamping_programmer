
#ifdef UNIT_TEST
#include <string>
typedef std::string String;
#else
#include "Arduino.h"
#endif

class StringWrapper
{
public:
	static String ToString( int value );

};
