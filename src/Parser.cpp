
#include "Parser.h"
#include "Verification.h"

namespace stamping
{
//const STRING_TYPE Parser::header("PRG:");
//const STRING_TYPE Parser::tail(":END");

size_t Parser::headerSize()
{
    return markerLength;
}
size_t Parser::tailSize()
{
    return markerLength;
}
Parser::Parser()
: ok_{false}
{

}
void Parser::parse(const STRING_TYPE& programStr, Program& prg)
{
    ok_ = false;
    const int beginIndex = headerSize();
    const int endIndex = programStr.length() - tailSize();
    Verification v;
    bool checkRetVal = v.checkNonVerbal(programStr, beginIndex, endIndex, prg.length());
    if (checkRetVal)
    {
        prg.fill(programStr.c_str(), beginIndex, endIndex);
        ok_ = true;
    }

}
bool Parser::ok() const
{
    return ok_;
}

}
