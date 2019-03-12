#include "Arduino.h"
#include "Components.h"

void setup()
{
    Serial.begin(115200);
    buttons.init();
}
