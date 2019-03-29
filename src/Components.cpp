
#include "Components.h"
#include "Pins.h"
#include "Arduino.h"

using namespace stamping;

Phases phases{};
Buttons buttons{};
Actuator a1(a1Pin);
Actuator a2(a2Pin);
Actuator a3(a3Pin);
Actuator a4(a4Pin);
Actuators actuators{};
SdFat sdFat;
const PROGMEM char* const programFile = "ohjelmat.txt";
LiquidCrystal_I2C lcd(0x27,16,4);
