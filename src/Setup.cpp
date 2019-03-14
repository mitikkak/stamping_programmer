#include "Arduino.h"
#include "Components.h"

bool checkProgramFile()
{
    Serial.print("Tarkistetaan: ");Serial.println(programFile);
    ifstream sdin(programFile);
    bool fileOk{true};
    if(sdin.fail())
    {
        sdin.close();
        fileOk = false;
    }
    else if (sdin.good())
    {
        Serial.println("=> ok!");
    }
    return fileOk;
}
void setup()
{
    Serial.begin(115200);
    buttons.init();
    actuators.add(a1);
    actuators.add(a2);
    actuators.add(a3);
    actuators.add(a4);
    if (!sdFat.begin(stamping::sdSelectPin)) {
      Serial.println("SD kortin avaaminen ei toimi!");
      return;
    }
    bool fileOk = checkProgramFile();
    if (not fileOk)
    {
        Serial.println("ei tiedostoa, luodaan...");
        ofstream sdout(programFile);
        // use flash for text to save RAM
        sdout << F("");
        sdout.close();
        checkProgramFile();
    }
}
