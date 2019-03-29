
#include "Components_12e.h"
#include "Arduino.h"

using namespace stamping;

//Phases phases{};
//Buttons buttons{};
//Actuator a1(a1Pin);
//Actuator a2(a2Pin);
//Actuator a3(a3Pin);
//Actuator a4(a4Pin);
//Actuators actuators{};

String const programFile = "/ohjelmat.txt";
LiquidCrystal_I2C lcd(0x27,16,4);

static const int server_port = 80;
//ESP8266WebServer server(server_port);
AsyncWebServer server(server_port);
