#include "Arduino.h"
#include "Components_12e.h"

typedef unsigned long TIME;
TIME prevTimeExecuted{0};
TIME const guardTime{3000};
bool programRunGuardTimePassed()
{
    TIME const timeNow = millis();
    return (millis() > (prevTimeExecuted + guardTime));
}

void loop()
{
    if (buttons.select())
    {
#ifndef EXTERNAL_PROGRAM_RUNNER
        if (inProgramRunningMode and programRunGuardTimePassed())
        {
            prevTimeExecuted = millis();
            program.setInterphaseDelay(interphaseDelay);
            program.run();
        }
#else
        //TODO...
#endif
        buttons.clear();
    }
}
