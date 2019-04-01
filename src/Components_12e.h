
//#define EXTERNAL_PROGRAM_RUNNER

#ifndef EXTERNAL_PROGRAM_RUNNER
#include "Buttons.h"
#include "Actuators.h"
#include "Program.h"
#endif

#include "Constants.h"
#include "Pins_12e.h"
#include "LiquidCrystal_I2C.h"
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include "Verification.h"

#ifndef EXTERNAL_PROGRAM_RUNNER
extern stamping::Buttons buttons;
extern stamping::Actuator a1;
extern stamping::Actuator a2;
extern stamping::Actuator a3;
extern stamping::Actuator a4;
extern stamping::Actuators actuators;
#endif

extern String const programFile;
extern LiquidCrystal_I2C lcd;
extern AsyncWebServer server;
extern stamping::Program program;
extern bool inProgramRunningMode;
extern int interphaseDelay;
