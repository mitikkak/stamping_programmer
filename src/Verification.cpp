#include "Verification.h"

namespace stamping
{
STRING_TYPE Verification::check(const STRING_TYPE& progStr, const int numberOfOutputs)
{
 	STRING_TYPE valueStr("");
    for(int i=0;i<progStr.length();i++)
    {
    	const char c = progStr[i];
//    	printf("%c prevS: %c \n", c, prevSeparator);
        if(c == startingSeparator)
        {
        	prevSeparator = c;
        }
        else if (not isSeparator(c))
        {
        	if (prevSeparator == endingSeparator)
        	{
        		STRING_TYPE stuffBetweenPhasesStr("Vaiheiden välissä: ");
        		stuffBetweenPhasesStr += c;
        		numOfPhases = 0;
        		return stuffBetweenPhasesStr;
        	}
        	valueStr += c;
        }
        else if(c == endingSeparator and prevSeparator == middleSeparator)
        {
        	prevSeparator = c;
        	valueStr="";
        	numOfPhases++;
        }
        else if (c == middleSeparator) /*and prevSeparator == startingSeparator*/
        {
        	const int prevValue = atoi(valueStr.c_str());
        	if (prevValue > 0 and prevValue <= numberOfOutputs)
        	{
        		prevSeparator = c;
        		valueStr="";
        	}
        	else
        	{
        		STRING_TYPE wrongActuatorStr("Väärä linja: ");
        		wrongActuatorStr += valueStr;
        		numOfPhases = 0;
        		return wrongActuatorStr;
        	}
        }
        else
        {
        	STRING_TYPE unknownErrorStr("Tuntematon tarkistusvirhe: ");
        	unknownErrorStr += c;
        	numOfPhases = 0;
        	return unknownErrorStr;
        }
    }
    status = true;
    return "Ok";
}
bool Verification::isSeparator(const char& c) const
{
	return (c == startingSeparator) or (c == middleSeparator) or (c == endingSeparator);
}
}
