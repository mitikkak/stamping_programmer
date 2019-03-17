
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
#ifndef ESP8266
SdFat sdFat;
#endif
#ifndef ESP8266
const PROGMEM char* const programFile = "ohjelmat.txt";
#else
String const programFile = "/ohjelmat.txt";
#endif
LiquidCrystal_I2C lcd(0x27,16,4);
#ifdef ESP8266
static const int server_port = 80;
ESP8266WebServer server(server_port);
#endif
