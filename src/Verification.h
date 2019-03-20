
#pragma once

struct Verification
{
    Verification(const STRING_TYPE& progStr)
    {
//        for(int i=0;i<progStr.length();i++)
//        {
//            if()
//        }
    }
    bool passed() const { return status; }
    int numOfPhases{0};
    bool status{false};
};
