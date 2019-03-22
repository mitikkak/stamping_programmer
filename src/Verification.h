
#pragma once

#include "StringWrapper.h"
#include "Actuators.h"
namespace stamping
{
struct Verification
{
    Verification()
    {
    }
    STRING_TYPE check(const STRING_TYPE& progStr, const Actuators& actuators);
    static constexpr char startingSeparator{'['};
    static constexpr char endingSeparator{']'};
    static constexpr char middleSeparator{':'};
    bool passed() const { return status; }
    int numOfPhases{0};
    bool status{false};
private:
    bool isSeparator(const char&) const;
    char prevSeparator{};
};


}
