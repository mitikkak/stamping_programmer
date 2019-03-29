
//#include "Phases.h"
//#include "Buttons.h"
//#include "Actuators.h"
#include "Constants.h"
#include "Pins_12e.h"
#include "LiquidCrystal_I2C.h"
#include "ESP8266WiFi.h"
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
//#include "ESP8266WebServer.h"
#include "FS.h"
#include "Verification.h"

//extern stamping::Buttons buttons;
//extern stamping::Actuator a1;
//extern stamping::Actuator a2;
//extern stamping::Actuator a3;
//extern stamping::Actuator a4;
//extern stamping::Actuators actuators;
extern String const programFile;
extern LiquidCrystal_I2C lcd;
#ifdef ESP8266
//extern ESP8266WebServer server;
extern AsyncWebServer server;
#endif
