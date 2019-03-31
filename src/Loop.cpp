#include "Arduino.h"
#include "Components.h"
#include "Program.h"
#include "Parser.h"

using namespace stamping;

int constexpr bufferSize{programMaxLength + 20};

STRING_TYPE serialReadBuffer("");
typedef unsigned long TIME;
bool programAvailable = false;

Program program1(actuators);
Program program2(actuators);
Program* currentProgram{&program1};

Program* selectProgramCandidate()
{
    if (currentProgram == &program1)
    {
        return &program2;
    }
    return &program1;
}
void swapProgram()
{
    if (currentProgram == &program1)
    {
        currentProgram = &program2;
    }
    else
    {
        currentProgram = &program1;
    }
}

void loop()
{
    bool newMessageReady{false};
    bool headerPresent{false};
    bool tailPresent{false};
    while ((Serial.available() > 0) and (not newMessageReady))
    {
        const char byte = (char) Serial.read();
        if (byte != '\n')
        {
            serialReadBuffer += byte;
            Serial.println(serialReadBuffer);
        }
        if (serialReadBuffer.startsWith(header))
        {
            Serial.println(F("New program starts"));
            headerPresent = true;
        }
        if (serialReadBuffer.endsWith(tail))
        {
            Serial.println(F("New program received"));
            tailPresent = true;
        }
        newMessageReady |= headerPresent and tailPresent;
    }
    if (newMessageReady)
    {
        Parser p;
        Program* prgCandidate = selectProgramCandidate();
        p.parse(serialReadBuffer, *prgCandidate);
        if (p.ok())
        {
            swapProgram();
            programAvailable = true;
        }
    }
    digitalWrite(programReadyPin, programAvailable ? HIGH : LOW);

    if (buttons.select() and programAvailable)
    {
        currentProgram->run();
        Serial.println(F("Program executed!!"));
        buttons.clear();
    }
    serialReadBuffer = "";
}
