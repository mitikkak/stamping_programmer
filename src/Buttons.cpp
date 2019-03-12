
#include "Buttons.h"
#include "Arduino.h"
#include "Pins.h"

namespace stamping
{

void Buttons::init()
{
    pinMode(upPin, INPUT_PULLUP);
    pinMode(downPin, INPUT_PULLUP);
    pinMode(selectPin, INPUT_PULLUP);
    pinMode(backPin, INPUT_PULLUP);
}
void Buttons::read()
{
    unsigned long timeNow = millis();
    if ((timeNow >= prevTime + thresholdTime))
    {
        upValue = digitalRead(upPin);
        downValue = digitalRead(downPin);
        selectValue = digitalRead(selectPin);
        backValue = digitalRead(backPin);
        prevTime = timeNow;
    }
}
bool Buttons::select() const
{
    return (selectValue == LOW);
}
bool Buttons::up() const
{
    return (upValue == LOW);
}
bool Buttons::down() const
{
    return (downValue == LOW);
}
void Buttons::log() const
{
    Serial.print("Up: "); Serial.print(upValue);
    Serial.print(", Down: "); Serial.print(downValue);
    Serial.print(", Sel: "); Serial.print(selectValue);
    Serial.print(", Back: "); Serial.println(backValue);
}

}
