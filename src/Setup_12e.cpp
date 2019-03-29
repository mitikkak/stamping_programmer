#include "Arduino.h"
#include "Components_12e.h"

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
<form method="post" action="runProgram">
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
static const char noProgramsMessage[] = R"rawliteral(<p> Ei ohjelmia </p>)rawliteral";
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
<head>
)rawliteral";

static const char htmlHeadEnd[] = R"rawliteral(
</head>
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
static const char deleteProgramTitle[] = R"rawliteral(<title>Ohjelman poisto</title>)rawliteral";
static const char fileOpenErrorMessage[] = R"rawliteral(<p>Ohjelman avaus ei onnistunut!</p>)rawliteral";
static const char cannotFindProgramMessage[] = R"rawliteral(<p>Ohjelmaa ei loydy!</p>)rawliteral";
static const char fileRemoveFailureMessage[] = R"rawliteral(<p>Ohjelman poisto ei onnistunut!</p>)rawliteral";
static const char programRemoveMessageBegin[] = R"rawliteral(<p>Poistettu ohjelma nro )rawliteral";
static const char deleteAllProgramsTitle[] = R"rawliteral(<title>Kaikkien ohjelmien poisto</title>)rawliteral";
static const char deleteAllProgramsOkMessage[] = R"rawliteral(<p>Kaikki ohjelmat poistettu</p>)rawliteral";

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
static const char createProgramTitle[] = R"rawliteral(<title>Ohjelman tallennus</title>)rawliteral";

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
static const char runProgramTitle[] = R"rawliteral(<title>Aja ohjelma</title>)rawliteral";
static const char runProgramOkMessageBegin[] = R"rawliteral(<p>Ajetaan ohjelma:
<br>
)rawliteral";

char programSaveBuffer[programMaxLength];

void handleCreateProgram(AsyncWebServerRequest *request) {
    memset(htmlResponse, 0, sizeof(htmlResponse));
    int writtenBytes = snprintf(&htmlResponse[0], sizeof(htmlHeadBegin), htmlHeadBegin);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(createProgramTitle), createProgramTitle);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(htmlHeadEnd), htmlHeadEnd);
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
    verification.check(argument, numberOfOutputs);
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

void handleRunProgram(AsyncWebServerRequest *request)
{
    memset(htmlResponse, 0, sizeof(htmlResponse));
    int writtenBytes = snprintf(&htmlResponse[0], sizeof(htmlHeadBegin), htmlHeadBegin);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(runProgramTitle), runProgramTitle);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(htmlHeadEnd), htmlHeadEnd);

    if (not SPIFFS.exists(programFile))
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(noProgramsMessage), noProgramsMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    if (request->args() != 2 )
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(wrongParamsMessage), wrongParamsMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    const size_t ard_idx{0};
    const String& argument = request->arg(ard_idx);
    const int lineIndex = atoi(argument.c_str()) - 1;
    if (lineIndex < 0)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(inputErrorMessage), inputErrorMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }

    File file = SPIFFS.open(programFile, "r");
    if (not file)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileOpenErrorMessage), fileOpenErrorMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    // Seek to correct program
    for(int i=0, atLine = 0;i<file.size() and atLine < lineIndex;i++)
    {
        const char c = (char)file.read();
        Serial.print(c); Serial.print(", fpos: "); Serial.println(file.position());
        if (c == '\n')
        {
            atLine++;
        }
    }
    if (file.position() >= file.size())
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(cannotFindProgramMessage), cannotFindProgramMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    memset(programSaveBuffer, 0, sizeof(programSaveBuffer));
    int programLength = 0;
    for(int i=0; (i<file.size()) and (file.position() <= file.size()) and (i < programMaxLength);i++)
    {
        const char c = (char)file.read();
        programSaveBuffer[i] = c;
        programLength++;
        Serial.print(i); Serial.print(": "); Serial.println(programSaveBuffer[i]);
        if (c == '\n')
        {
            break;
        }
    }
    file.close();

    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(runProgramOkMessageBegin), runProgramOkMessageBegin);
    Serial.println(writtenBytes);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], programLength+1, programSaveBuffer);
    Serial.println(writtenBytes);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraph), closeParagraph);
    Serial.println(writtenBytes);
    closeAndSendHtmlResponse(writtenBytes, request);
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
    memset(htmlResponse, 0, sizeof(htmlResponse));
    int writtenBytes = snprintf(&htmlResponse[0], sizeof(htmlHeadBegin), htmlHeadBegin);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(deleteProgramTitle), deleteProgramTitle);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(htmlHeadEnd), htmlHeadEnd);
    if (request->args() != 2 )
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(wrongParamsMessage), wrongParamsMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    if (not SPIFFS.exists(programFile))
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(noProgramsMessage), noProgramsMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    const size_t ard_idx{0};
    const String& argument = request->arg(ard_idx);
    const int lineIndex = atoi(argument.c_str()) - 1;
    Serial.print("Poistetaan ohjelma rivi: "); Serial.println(lineIndex+1);
    if (lineIndex < 0)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(inputErrorMessage), inputErrorMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    File file = SPIFFS.open(programFile, "r+");
    if (not file)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileOpenErrorMessage), fileOpenErrorMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
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
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(cannotFindProgramMessage), cannotFindProgramMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
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
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileRemoveFailureMessage), fileRemoveFailureMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    bool const emptyContent = (newContent.length() == 0);
    if (not emptyContent)
    {
        Serial.println("luodaan uusi");
        file = SPIFFS.open(programFile, "a+");
        if (!file)
        {
            writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileCreateFailureMessage), fileCreateFailureMessage);
            closeAndSendHtmlResponse(writtenBytes, request);
            return;
        }
    }
    saveProgram(file, newContent);
    file.close();
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(programRemoveMessageBegin), programRemoveMessageBegin);
    itoa(lineIndex+1, itoaBuffer, 10);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(itoaBuffer), itoaBuffer);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(closeParagraph), closeParagraph);

    closeAndSendHtmlResponse(writtenBytes, request);
}
void handleDeleteAllPrograms(AsyncWebServerRequest *request)
{
    memset(htmlResponse, 0, sizeof(htmlResponse));
    int writtenBytes = snprintf(&htmlResponse[0], sizeof(htmlHeadBegin), htmlHeadBegin);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(deleteAllProgramsTitle), deleteAllProgramsTitle);
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(htmlHeadEnd), htmlHeadEnd);
    bool const retValue = SPIFFS.remove(programFile);
    if (not retValue)
    {
        writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(fileRemoveFailureMessage), fileRemoveFailureMessage);
        closeAndSendHtmlResponse(writtenBytes, request);
        return;
    }
    writtenBytes += snprintf(&htmlResponse[writtenBytes-1], sizeof(deleteAllProgramsOkMessage), deleteAllProgramsOkMessage);
    closeAndSendHtmlResponse(writtenBytes, request);
}
void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Sivua ei ole!");
}

const char* ssid = "sarmari_ap";
const char* password = "mutiainen";
void setup()
{
    Serial.begin(115200);
//    buttons.init();
//    actuators.add(a1);
//    actuators.add(a2);
//    actuators.add(a3);
//    actuators.add(a4);
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
    server.on("/runProgram", handleRunProgram);
    server.on("/deleteProgram", handleDeleteProgram);
    server.on("/deleteAllPrograms", handleDeleteAllPrograms);
    server.on("/configureSettings", handleConfigureSettings);
    server.onNotFound(handleNotFound);
    server.begin();
    SPIFFS.begin();
}
