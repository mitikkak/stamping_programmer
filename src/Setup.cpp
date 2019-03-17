#include "Arduino.h"
#include "Components.h"

#ifndef ESP8266
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
#else
static const char INDEX_HEAD_HTML[] = R"rawliteral( 
<!DOCTYPE html>
<html>
<head>
<title>Sarmarin ohjaus</title>
</head>
<body>
)rawliteral";
static const char INDEX_MIDDLE_HTML[] = R"rawliteral(
<form method="post" action="/createProgram" >
Uusi ohjelma:<br><input name="line" type="text" size="16000" value="" ><br><br>
<input type="submit" name="clk_action" value="Luo ohjelma">
</form>
<form method="post" action="/example" >
<input type="submit" name="clk_action" value="wifi">
</form>
<form method="post" action="openProgram">
<br><button id=\"openProgram\">Avaa ohjelma</button>
</form>
<form method="post" action="deleteAllPrograms">
<br><button id=\"deleteAllPrograms\">Poista kaikki ohjelmat</button>
)rawliteral";
static const char INDEX_TAIL_HTML[] = R"rawliteral(
</form>
</body>
</html>
)rawliteral";

//---------------------------------------------------------
int constexpr htmlResponseSize{3000};
int constexpr programMaxLength{1000};
char htmlResponse[htmlResponseSize]{};
void handleRoot() {
  memset(htmlResponse, 0, sizeof(htmlResponseSize));
  int writtenBytes = snprintf(&htmlResponse[0], sizeof(INDEX_HEAD_HTML), INDEX_HEAD_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(INDEX_MIDDLE_HTML), INDEX_MIDDLE_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  //
  String programs = "<p><textarea cols=\"";
  programs += String(programMaxLength);
  programs += "\" name=\"Ohjelmat\" rows=\"10\">dfsdf</textarea></p>";
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], programs.length(), programs.c_str());
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(INDEX_TAIL_HTML), INDEX_TAIL_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  Serial.println(htmlResponse);
  server.send(200, "text/html", htmlResponse);
}
void handleCreateProgram() {
  String str = "Tallennetaan ohjelma ...\r\n";
  if (server.args() == 2 )
  {
      File file = SPIFFS.open(programFile, "a+");
      if (!file) {
        str += "Tiedoston luonti epäonnistui! \n";
      }
      else
      {
          String const lineStr = server.arg(0);
          //String const periodStr = server.arg(1);
          String const endLineStr = "\n";
          Serial.print("Line: "); Serial.println(lineStr);
          //Serial.print("Period: "); Serial.println(periodStr);
//          uint8_t buf[] = "[1:1234][2:2345][3:3456]\n";
          size_t writtenBytes = file.write((const uint8_t*)(lineStr.c_str()), lineStr.length());
          //writtenBytes += file.write((const uint8_t*)(periodStr.c_str()), periodStr.length());
          writtenBytes += file.write((const uint8_t*)endLineStr.c_str(), endLineStr.length());
          str += "Kirjoitettu: ";
          str += writtenBytes;
          str += " tavua \n";
      }
      file.close();
  }
  else
  {
      str += "Väärät parametrit!";
  }

  server.send(200, "text/plain", str.c_str());
}
void handleOpenProgram() {
    String str = "";
    if (SPIFFS.exists(programFile))
    {
        File file = SPIFFS.open(programFile, "r");
#if 0
        str += programFile + " koko :";
        str += file.size() + "\n";
        str += "Ohjelmat ovat: \n";
        while (file.available() && file.position() < file.size())
        {
            str += file.readStringUntil('\n');
        }
#endif
        Serial.print("size: "); Serial.println(file.size());
        for(int i=0;i<file.size();i++)
        {
            const char c = (char)file.read();
            Serial.print(c);
            str += c;
        }
        file.close();
    }
    else
    {
        str += "Ei ohjelmia! \n";
    }
    server.send(200, "text/plain", str.c_str());
}
void handleDeleteAllPrograms()
{
    String str = "";
    bool const retValue = SPIFFS.remove(programFile);
    if (retValue)
    {
        str += "Kaikki ohjelmat poistettu";
    }
    else
    {
        str += "Ei onnistunut!";
    }
    server.send(200, "text/plain", str.c_str());
}
void handleNotFound() {
  server.send(404, "text/plain", "Sivua ei ole!");
}
#endif
void setup()
{
    Serial.begin(115200);
    buttons.init();
    actuators.add(a1);
    actuators.add(a2);
    actuators.add(a3);
    actuators.add(a4);
#ifdef ESP8266
    lcd.init();
    // Connect to Wi-Fi network with SSID and password
    lcd.clear();
    lcd.setCursor(0,0);
    //WiFi.begin(ssid, password);
    const char ssid[] = "sarmari_ap";
    WiFi.softAP(ssid);
    lcd.print(ssid);
    lcd.setCursor(0,1);
    lcd.print("IP: ");
    lcd.print(WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/createProgram", handleCreateProgram);
    server.on("/openProgram", handleOpenProgram);
    server.on("/deleteAllPrograms", handleDeleteAllPrograms);
    server.onNotFound(handleNotFound);
    server.begin();
    SPIFFS.begin();
#else
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
#endif
}
