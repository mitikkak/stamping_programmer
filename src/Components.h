
#include "Phases.h"
#include "Buttons.h"
#include "Actuators.h"
#include "Pins.h"
#ifndef ESP8266
#include "SdFat.h"
#endif
#include "LiquidCrystal_I2C.h"
#ifdef ESP8266
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "FS.h"
#endif
#include "Verification.h"

extern stamping::Buttons buttons;
extern stamping::Actuator a1;
extern stamping::Actuator a2;
extern stamping::Actuator a3;
extern stamping::Actuator a4;
extern stamping::Actuators actuators;
#ifndef ESP8266
extern SdFat sdFat;
#endif
#ifndef ESP8266
extern const char* const programFile;
#else
extern String const programFile;
#endif
extern LiquidCrystal_I2C lcd;
#ifdef ESP8266
extern ESP8266WebServer server;
#endif
