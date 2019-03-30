
#pragma once
#include "StringWrapper.h"
#include "Constants.h"
#include "Program.h"

namespace stamping
{
static const char* header{"PRG:"};
static const char* tail{":END"};

class Parser
{
public:
    Parser();
    void parse(const STRING_TYPE& programStr, Program& prg);
    bool ok() const;
    static size_t headerSize();
    static size_t tailSize();
    static constexpr size_t markerLength{4};
//    static const STRING_TYPE header;//{"PRG:"};

//    static const STRING_TYPE tail; //{":END"};
private:
    bool ok_;
};
}
