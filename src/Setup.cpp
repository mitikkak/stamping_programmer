#include "Arduino.h"
#include "Components.h"
#include "Pins.h"

using namespace stamping;
void setup()
{
    Serial.begin(115200);
    buttons.init();
    actuators.add(a1);
    actuators.add(a2);
    actuators.add(a3);
    actuators.add(a4);
    pinMode(programReadyPin, OUTPUT);
    Serial.println("begins");
}
