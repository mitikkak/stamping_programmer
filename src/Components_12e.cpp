
#include "Components_12e.h"
#include "Arduino.h"

using namespace stamping;

#ifndef EXTERNAL_PROGRAM_RUNNER
Buttons buttons{};
Actuator a1(a1Pin);
Actuator a2(a2Pin);
Actuator a3(a3Pin);
Actuator a4(a4Pin);
Actuators actuators{};
Program program(actuators);
#endif

String const programFile = "/ohjelmat.txt";
LiquidCrystal_I2C lcd(0x27,16,4);

static const int server_port = 80;
AsyncWebServer server(server_port);

bool inProgramRunningMode{false};
int interphaseDelay{0};
