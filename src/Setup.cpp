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
int constexpr htmlResponseSize{3000};
char htmlResponse[htmlResponseSize]{};
static const char openParagraph[] = R"rawliteral(<p>)rawliteral";
static const char closeParagraph[] = R"rawliteral(</p>)rawliteral";

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
<form method="post" action="deleteProgram">
<p><span style="font-size:24px;"><strong>Poista ohjelma:</strong></span></p>
<br><input name="line" type="text" size="1" value="" ><br><br>
<input type="submit" name="clk_action" value="Poista ohjelma">
</form>
)rawliteral";
static const char INDEX_TAIL_HTML[] = R"rawliteral(
<form method="post" action="deleteAllPrograms">
<br><button id=\"deleteAllPrograms\">Poista kaikki ohjelmat</button>
</form>
</body>
</html>
)rawliteral";

static const char endHtml[] = R"rawliteral(
</body>
</html>
)rawliteral";

//---------------------------------------------------------
const char noProgramsStr[] = "Ei ohjelmia";
const char fileOpenFailedStr[] = "Ohjelmien luku epaonnistui!";

unsigned int numOfPrograms{0};
int constexpr programMaxLength{300};
int constexpr maxNumOfPrograms{5};
int constexpr programsMessageAreaSize{programMaxLength*maxNumOfPrograms};
char programsMessageArea[programsMessageAreaSize];
char itoaBuffer[33];
void getPrograms()
{
    numOfPrograms = 1;
    if (not SPIFFS.exists(programFile))
    {
        snprintf(programsMessageArea, sizeof(noProgramsStr), noProgramsStr);
        return;
    }
    File file = SPIFFS.open(programFile, "r");
    if (!file)
    {
        snprintf(programsMessageArea, sizeof(fileOpenFailedStr), fileOpenFailedStr);
        return;
    }
    Serial.print("file size: "); Serial.println(file.size());
    const char firstItemHeader[] = "1: ";
    int writtenBytes{0};
    writtenBytes += snprintf(programsMessageArea, sizeof(firstItemHeader), firstItemHeader);
    for(int i=0;i<file.size();i++)
    {
        const char c = (char)file.read();
        programsMessageArea[writtenBytes-1] = c;
        writtenBytes++;
        Serial.print(c);Serial.print(" i: "); Serial.print(i); Serial.print("/"); Serial.println(file.size());
        bool const nonLastByte = (i < file.size()-1);
        if (c == '\n' and nonLastByte)
        {
            numOfPrograms++;
            if (nonLastByte)
            {
                itoa(numOfPrograms, itoaBuffer, 10);
                writtenBytes += snprintf(&programsMessageArea[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
                const char headerSeparator[] = ": ";
                writtenBytes += snprintf(&programsMessageArea[writtenBytes-1], sizeof(headerSeparator), headerSeparator);
            }
        }
    }
    file.close();
}
String const settingsFile = "/asetukset.txt";
int constexpr defaultDelay{10};
struct Settings
{
    int delay{defaultDelay};
};
int constexpr sizeOfSettingsReadArea{33};
char settingsReadArea[sizeOfSettingsReadArea];
void getSettings(Settings& settings)
{
    if (not SPIFFS.exists(settingsFile))
    {
        return;
    }
    File file = SPIFFS.open(settingsFile, "r");
    if (!file)
    {
        return;
    }
    memset(settingsReadArea, 0, sizeof(settingsReadArea));
    for(int i=0;i<file.size();i++)
    {
        settingsReadArea[i] = (char)file.read();
    }
    settings.delay = atoi(settingsReadArea);
    file.close();
}
const String wrongParamsStr("Vaarat parametrit!");
bool removeOldSettings()
{
    bool retValue{false};
    if (not SPIFFS.exists(settingsFile))
    {
        retValue = true;
    }
    else
    {
        retValue = SPIFFS.remove(settingsFile);
    }
    return retValue;
}
static const char htmlHeadBegin[] = R"rawliteral( 
<!DOCTYPE html>
<html>
<head>)rawliteral";

static const char htmlHeadEnd[] = R"rawliteral(</head>
<body>
)rawliteral";

static const char configureSettingsTitle[] = R"rawliteral(<title>Asetusten tallennus</title>)rawliteral";
static const char emptyInputMessage[] = R"rawliteral(<p>Tyhja syote, ei talleteta!</p>)rawliteral";
static const char configureSettingsGreeting[] = R"rawliteral(<p>Tallennetaan asetukset ...</p>)rawliteral";
static const char settingsRemoveFailureMessage[] = R"rawliteral(<p>Vanhojen asetusten poisto ei onnistunut!</p>)rawliteral";
static const char settingsCreateFailureMessage[] = R"rawliteral(<p>Asetusten tallennus ei onnistunut!</p>)rawliteral";
static const char wrongParamsFailureMessage[] = R"rawliteral(<p>Vaarat parametrit!</p>)rawliteral";
static const char tooLongInputMessage[] = R"rawliteral(<p>Liian pitka syote!</p>)rawliteral";
static const char okMessage[] = R"rawliteral( ok!)rawliteral";

void closeAndSendHtmlResponse(int writtenBytes, AsyncWebServerRequest *request)
{
	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(endHtml), endHtml);
	Serial.println(htmlResponse);
	request->send(200, "text/html", htmlResponse);
}

const size_t configureSettingsInputBufferSize{33};
char configureSettingsInputBuffer[configureSettingsInputBufferSize];
void handleConfigureSettings(AsyncWebServerRequest *request)
{
	memset(htmlResponse, 0, sizeof(htmlResponse));
	memset(configureSettingsInputBuffer, 0, sizeof(configureSettingsInputBuffer));
	int writtenBytes = snprintf(&htmlResponse[0], sizeof(htmlHeadBegin), htmlHeadBegin);
	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(configureSettingsTitle), configureSettingsTitle);
	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(htmlHeadEnd), htmlHeadEnd);
    if (request->args() != 2 )
    {
    	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(wrongParamsFailureMessage), wrongParamsFailureMessage);
    	closeAndSendHtmlResponse(writtenBytes, request);
    	return;
    }
    size_t const arg_idx{0};
    const String& argument = request->arg(arg_idx);
    size_t const settingsLength = argument.length();
    Serial.print("settingsLength: "); Serial.println(settingsLength);
    Serial.print("configureSettingsInputBufferSize: "); Serial.println(configureSettingsInputBufferSize);
    if (not settingsLength > 0)
    {
    	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(emptyInputMessage), emptyInputMessage);
    	closeAndSendHtmlResponse(writtenBytes, request);
    	return;
    }
    if (not (settingsLength < configureSettingsInputBufferSize))
    {
    	writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(tooLongInputMessage), tooLongInputMessage);
    	closeAndSendHtmlResponse(writtenBytes, request);
    	return;
    }
    bool const retValue = removeOldSettings();
    if (not retValue)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(settingsRemoveFailureMessage), settingsRemoveFailureMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    File file = SPIFFS.open(settingsFile, "a+");
    if (!file)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(settingsCreateFailureMessage), settingsCreateFailureMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    memcpy(configureSettingsInputBuffer, argument.c_str(), settingsLength);
    file.write((const uint8_t*)configureSettingsInputBuffer, settingsLength);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(openParagraph), openParagraph);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(configureSettingsInputBuffer), configureSettingsInputBuffer);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(okMessage), okMessage);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraph), closeParagraph);
    file.close();
    closeAndSendHtmlResponse(writtenBytes, request);
}
const String noProgramsString("Ei ohjelmia!");
const String inputErrorStr("Vaara syote!");
const String fileOpenErrorStr("Tiedoston aukaisu ei toimi!");
const String cannotFindProgramStr("Ohjelmaa ei löydy!");
const String fileRemoveFailureStr("Ohjelman poisto ei onnistunut!");
const String fileCreateFailureStr("Ohjelman luonti ei onnistunut!");
static const char fileCreateFailureMessage[] = R"rawliteral(Ohjelman luonti ei onnistunut!)rawliteral";
static const char wrongParamsMessage[] = R"rawliteral(Vaarat parametrit!)rawliteral";

static const char openParagraphBigBoldFont[] = R"rawliteral(<p><span style=\"font-size:24px;\"><strong>)rawliteral";
static const char closeParagraphBigBoldFont[] = R"rawliteral(</strong></span></p>)rawliteral";
static const char fsInfoHeading[] = R"rawliteral(Tiedostojen tilan kayttoaste:)rawliteral";
static const char programsHeading[] = R"rawliteral(Ohjelmat:)rawliteral";
static const char bytesMsgStr[] = R"rawliteral(tavuja: )rawliteral";
static const char slashStr[] = R"rawliteral(/)rawliteral";
static const char textAreaStartAndColumns[] = R"rawliteral(<p><textarea cols=)rawliteral";
static const char textAreaNameAndRows[] = R"rawliteral( name="Ohjelmat" rows=)rawliteral";
static const char textAreaRowsCloser[] = R"rawliteral(>)rawliteral";
static const char textAreaEnd[] = R"rawliteral(</textarea></p>
)rawliteral";
static const char settingsHeading[] = R"rawliteral(<form method="post" action="configureSettings">
<p><span style="font-size:24px;"><strong>Yleiset asetukset:</strong></span></p>)rawliteral";
static const char currentDelayHeading[] = R"rawliteral(<p>Viive tyovaiheiden valissa: )rawliteral";
static const char inputDelay[] = R"rawliteral(<br><input name="line" type="text" size="1" value="" ><input type="submit" name="clk_action" value="Aseta viive">)rawliteral";
static const char endForm[] = R"rawliteral(</form>)rawliteral";

void handleRoot(AsyncWebServerRequest *request) {
  memset(htmlResponse, 0, sizeof(htmlResponseSize));
  memset(programsMessageArea, 0, sizeof(programsMessageArea));
  int writtenBytes = snprintf(&htmlResponse[0], sizeof(INDEX_HEAD_HTML), INDEX_HEAD_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(INDEX_MIDDLE_HTML), INDEX_MIDDLE_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  //
  Settings settings;
  getSettings(settings);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(settingsHeading), settingsHeading);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(currentDelayHeading), currentDelayHeading);
  itoa(settings.delay, itoaBuffer, 10);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraph), closeParagraph);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(inputDelay), inputDelay);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(endForm), endForm);

  getPrograms();
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(openParagraphBigBoldFont), openParagraphBigBoldFont);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(programsHeading), programsHeading);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraphBigBoldFont), closeParagraphBigBoldFont);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(textAreaStartAndColumns), textAreaStartAndColumns);
  itoa(programMaxLength, itoaBuffer, 10);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(textAreaNameAndRows), textAreaNameAndRows);
  itoa(numOfPrograms, itoaBuffer, 10);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(textAreaRowsCloser), textAreaRowsCloser);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], programsMessageAreaSize, programsMessageArea);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(textAreaEnd), textAreaEnd);
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(openParagraphBigBoldFont), openParagraphBigBoldFont);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fsInfoHeading), fsInfoHeading);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraphBigBoldFont), closeParagraphBigBoldFont);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(openParagraph), openParagraph);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(bytesMsgStr), bytesMsgStr);
  itoa(fs_info.usedBytes, itoaBuffer, 10);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(slashStr), slashStr);
  itoa(fs_info.totalBytes, itoaBuffer, 10);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraph), closeParagraph);
  Serial.print("after fsInfo sendResp: "); Serial.println(writtenBytes);
  writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(INDEX_TAIL_HTML), INDEX_TAIL_HTML);
  Serial.print("sendResp: "); Serial.println(writtenBytes);
  Serial.println(htmlResponse);
  request->send(200, "text/html", htmlResponse);
}
size_t saveProgram(File& file, const String& content)
{
    Serial.print("Line: "); Serial.println(content);
    size_t writtenBytes = file.write((const uint8_t*)(content.c_str()), content.length());
    //writtenBytes += file.write((const uint8_t*)endLineStr.c_str(), endLineStr.length());
    return writtenBytes;
}
size_t saveProgram2(File& file, const char* const content, const int length)
{
    Serial.print("Line: "); Serial.println(content);
    size_t writtenBytes = file.write((const uint8_t*)content, length);
    //writtenBytes += file.write((const uint8_t*)endLineStr.c_str(), endLineStr.length());
    return writtenBytes;
}
static const char createProgramHtmlHeader[] = R"rawliteral( 
<!DOCTYPE html>
<html>
<head>
<title>Ohjelman tallennus</title>
</head>
<body>
)rawliteral";

static const char createProgramGreeting[] = R"rawliteral(
<p>Tallennetaan ohjelma ...</p>
)rawliteral";
static const char emptyProgramMessage[] = R"rawliteral(
<p>Tyhja ohjelma, ei talleteta!</p>
)rawliteral";
static const char tooLongProgramMessage[] = R"rawliteral(
<p>Liian pitka ohjelma, ei talleteta!</p>
)rawliteral";
static const char inputErrorMessage[] = R"rawliteral(
<p>Virhe syotteessa!</p>
)rawliteral";
static const char createProgramOkMessageBegin[] = R"rawliteral(<p>)rawliteral";
static const char createProgramOkMessageMiddle[] = R"rawliteral( vaihetta. Kirjoitettu: )rawliteral";
static const char createProgramOkMessageEnd[] = R"rawliteral( tavua. </p>)rawliteral";

char programSaveBuffer[programMaxLength];

void handleCreateProgram(AsyncWebServerRequest *request) {
    memset(htmlResponse, 0, sizeof(htmlResponse));
    int writtenBytes = snprintf(&htmlResponse[0], sizeof(createProgramHtmlHeader), createProgramHtmlHeader);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(createProgramGreeting), createProgramGreeting);
    if (request->args() != 2 )
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(wrongParamsMessage), wrongParamsMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    size_t const arg_idx = 0;
    const String& argument = request->arg(arg_idx);
    size_t programLength = argument.length();
    if (not (programLength <= programMaxLength-1)) // one byte needed for end line
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(tooLongProgramMessage), tooLongProgramMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    if (programLength == 0)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(emptyProgramMessage), emptyProgramMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    stamping::Verification verification;
    verification.check(argument, actuators);
    if (not verification.passed())
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(inputErrorMessage), inputErrorMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    File file = SPIFFS.open(programFile, "a+");
    if (!file)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileCreateFailureMessage), fileCreateFailureMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }

    memset(programSaveBuffer, 0, sizeof(programSaveBuffer));

    memcpy(programSaveBuffer, argument.c_str(), programLength);
    programSaveBuffer[programLength] = '\n';
    programLength++;
    size_t const writtenBytesToFile = saveProgram2(file, programSaveBuffer, programLength);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(createProgramOkMessageBegin), createProgramOkMessageBegin);
    itoa(verification.numOfPhases, itoaBuffer, 10);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(createProgramOkMessageMiddle), createProgramOkMessageMiddle);
    itoa(writtenBytesToFile, itoaBuffer, 10);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(createProgramOkMessageEnd), createProgramOkMessageEnd);
    file.close();
    closeAndSendHtmlResponse(writtenBytes, request);
}

void handleOpenProgram(AsyncWebServerRequest *request)
{
    String str = "";
    if (SPIFFS.exists(programFile))
    {
        File file = SPIFFS.open(programFile, "r");
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
        str += noProgramsString;
    }
    request->send(200, "text/plain", str.c_str());
}

void printFileToSerial(File& file)
{
    Serial.print("File size: "); Serial.println(file.size());
    for(int i=0;i<file.size();i++)
    {
        Serial.print("["); Serial.print(i); Serial.print("]: "); Serial.println((char) file.read());
    }
}
void handleDeleteProgram(AsyncWebServerRequest *request)
{
    if (request->args() != 2 )
    {
        request->send(200, "text/plain", wrongParamsStr);
        return;
    }
    if (not SPIFFS.exists(programFile))
    {
        request->send(200, "text/plain", noProgramsStr);
        return;
    }
    const size_t ard_idx{0};
    const String& argument = request->arg(ard_idx);
    const int lineIndex = atoi(argument.c_str()) - 1;
    Serial.print("Poistetaan ohjelma rivi: "); Serial.println(lineIndex+1);
    if (lineIndex < 0)
    {
        request->send(200, "text/plain", inputErrorStr);
        return;
    }
    File file = SPIFFS.open(programFile, "r+");
    if (not file)
    {
        request->send(200, "text/plain", fileOpenErrorStr);
        return;
    }
    printFileToSerial(file);
    file.seek(0);
    String newContent = "";
    for(int i=0, atLine = 0;i<file.size() and atLine < lineIndex;i++)
    {
        const char c = (char)file.read();
        Serial.print(c); Serial.print(", fpos: "); Serial.println(file.position());
        newContent += c;
        if (c == '\n')
        {
            atLine++;
        }
    }
    Serial.print("final fpos: "); Serial.println(file.position());
    if (file.position() >= file.size())
    {
        request->send(200, "text/plain", cannotFindProgramStr);
        return;
    }
    Serial.println("skipataan rivi");
    file.readStringUntil('\n');
    Serial.print("Luetaan loput: "); Serial.print(file.position());Serial.print("=");Serial.println(file.peek());
    while (file.position() < file.size())
    {
        Serial.print("fpos: "); Serial.print(file.position());
        const char c = (char)file.read();
        Serial.print("= "); Serial.println(c);
        newContent += c;
    }
    Serial.print("Uusi: "); Serial.print(newContent.length());
    Serial.println(newContent);
    file.close();
    bool const fileDeleteSuccessful = SPIFFS.remove(programFile);
    if (not fileDeleteSuccessful)
    {
        request->send(200, "text/plain", fileRemoveFailureStr.c_str());
        return;
    }
    bool const emptyContent = (newContent.length() == 0);
    if (not emptyContent)
    {
        Serial.println("luodaan uusi");
        file = SPIFFS.open(programFile, "a+");
        if (!file) {
            request->send(200, "text/plain", fileCreateFailureStr.c_str());
            return;
        }
    }
    saveProgram(file, newContent);
    file.close();
    String str = "Poistettu ohjelma nro " + String(lineIndex+1);
    request->send(200, "text/plain", str.c_str());
}
void handleDeleteAllPrograms(AsyncWebServerRequest *request)
{
    String str = "";
    bool const retValue = SPIFFS.remove(programFile);
    if (retValue)
    {
        str += "Kaikki ohjelmat poistettu";
    }
    else
    {
        str += fileRemoveFailureStr;
    }
    request->send(200, "text/plain", str.c_str());
}
void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Sivua ei ole!");
}
#endif

const char* ssid = "sarmari_ap";
const char* password = "mutiainen";
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
    server.on("/", HTTP_GET, handleRoot);
    server.on("/createProgram", handleCreateProgram);
    server.on("/openProgram", handleOpenProgram);
    server.on("/deleteProgram", handleDeleteProgram);
    server.on("/deleteAllPrograms", handleDeleteAllPrograms);
    server.on("/configureSettings", handleConfigureSettings);
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
