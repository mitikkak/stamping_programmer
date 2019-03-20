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
<p><span style="font-size:24px;"><strong>Luo ohjelma:</strong></span></p>
<br><input name="line" type="text" size="16000" value="" ><br><br>
<input type="submit" name="clk_action" value="Luo ohjelma">
</form>
<form method="post" action="openProgram">
<p><span style="font-size:24px;"><strong>Aja ohjelma:</strong></span></p>
<br><input name="line" type="text" size="1" value="" ><br><br>
<input type="submit" name="clk_action" value="Aja ohjelma">
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

//int const numOfPrograms{3};
//String programStr[numOfPrograms] = {
//         "[1:200][3:300][2:9000][1:2344]"
//        ,"[2:1200][1:1300][2:5000][1:24]"
//        ,"[1:35200][1:457300]"
//};
struct Programs
{
    Programs(unsigned int n, const String& s)
    : numOf{n},
      strValue{s}
    {}
    void init()
    {
        numOf = 0;
        strValue = "";
    }
    unsigned int numOf{0};
    String strValue{""};
};
const Programs noPrograms(1, "Ei ohjelmia");
const Programs fileOpenFailed(1, "Ohjelmien luku epäonnistui!");

void getPrograms(Programs& progs)
{
    if (not SPIFFS.exists(programFile))
    {
        progs = noPrograms;
        return;
    }
    File file = SPIFFS.open(programFile, "r");
    if (!file)
    {
        progs = fileOpenFailed;
        return;
    }
    progs.init();
    for(int i=0;i<file.size();i++)
    {
        const char c = (char)file.read();
        if (c == '\n')
        {
            progs.numOf++;
        }
        progs.strValue += c;
    }
    file.close();
}
int constexpr htmlResponseSize{3000};
int constexpr programMaxLength{1000};
char htmlResponse[htmlResponseSize]{};
Programs progs(0, "");
void handleRoot() {
  memset(htmlResponse, 0, sizeof(htmlResponseSize));
  int writtenBytes = snprintf(&htmlResponse[0], sizeof(INDEX_HEAD_HTML), INDEX_HEAD_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(INDEX_MIDDLE_HTML), INDEX_MIDDLE_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  //
  getPrograms(progs);
  String htmlRespProgramsStr = "<p><span style=\"font-size:24px;\"><strong>Ohjelmat:</strong></span></p>";
  htmlRespProgramsStr += "<p><textarea cols=\"";
  htmlRespProgramsStr += String(programMaxLength);
  htmlRespProgramsStr += "\" name=\"Ohjelmat\" rows=\"";
  htmlRespProgramsStr += String(progs.numOf);
  htmlRespProgramsStr += "\">";
  htmlRespProgramsStr += progs.strValue;
  htmlRespProgramsStr += "</textarea></p>";
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], htmlRespProgramsStr.length(), htmlRespProgramsStr.c_str());
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
      String const lineStr = server.arg(0);
      const Verification verification(lineStr);
      if (lineStr == "")
      {
          str += "Tyhjä ohjelma, ei talleteta!";
      }
      else if (not verification.passed())
      {
          str += "Virhe syötteessä!";
      }
      else
      {
          File file = SPIFFS.open(programFile, "a+");
          if (!file) {
              str += "Tiedoston luonti epäonnistui! \n";
          }
          else
          {
              String const endLineStr = "\n";
              Serial.print("Line: "); Serial.println(lineStr);
              size_t writtenBytes = file.write((const uint8_t*)(lineStr.c_str()), lineStr.length());
              writtenBytes += file.write((const uint8_t*)endLineStr.c_str(), endLineStr.length());
              str += String(verification.numOfPhases);
              str += " vaihetta. \n";
              str += "Kirjoitettu: ";
              str += writtenBytes;
              str += " tavua \n";
          }
          file.close();
      }
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

const char* ssid = "sarmari_ap";
const char* password = "abba_acdc";
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
    WiFi.softAP(ssid, password);
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
