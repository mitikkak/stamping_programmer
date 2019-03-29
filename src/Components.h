
#include "Constants.h"
#include "Phases.h"
#include "Buttons.h"
#include "Actuators.h"
#include "Pins.h"
#ifndef ESP8266
#include "SdFat.h"
#endif
#include "LiquidCrystal_I2C.h"
#include "Verification.h"

extern stamping::Buttons buttons;
extern stamping::Actuator a1;
extern stamping::Actuator a2;
extern stamping::Actuator a3;
extern stamping::Actuator a4;
extern stamping::Actuators actuators;
extern SdFat sdFat;
extern const char* const programFile;
extern LiquidCrystal_I2C lcd;
