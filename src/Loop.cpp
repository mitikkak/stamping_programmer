#include "Arduino.h"
#include "Components.h"
#include "Program.h"

class Runnable
{
public:
	virtual ~Runnable() {}
	virtual void run() = 0;
};
struct Selection
{
    Selection(const char* const n, Runnable& r)
    : name{n}, runner(r)
    {}
    void run() const
    {
        Serial.print("Suoritetaan: "); Serial.println(name);
        runner.run();
    }

    const char* const name;
    Runnable& runner;
};

class ExecuteProgram : public Runnable
{
public:
	void run()
	{
		const char* savedProgramStr = "[1:1256][0:329][3:2132]";
		stamping::Program p(actuators, savedProgramStr);
		p.run();
	}
};
class CreateProgram : public Runnable
{
public:
	void run()
	{
#ifdef ESP8266
#else
	    Serial.println("Avataan: ");Serial.println(programFile);
	    ifstream sdin(programFile);
	    Serial.print("flags: "); Serial.println(sdin.rdstate());
	    const int line_buffer_size = 18;
	    char buffer[line_buffer_size];
#endif
	}
};

ExecuteProgram executeProgram;
CreateProgram createProgram;
class Selections
{
public:
    Selections()
    : selections{
    Selection("Aja ohjelma", executeProgram),
    Selection("Luo ohjelma", createProgram)
    }
    {}
    void show() const
    {
        static int occ = 0;
        occ++;
        Serial.print("["); Serial.print(occ); Serial.print("]");
        Serial.println(selections[atLine].name);
    }
    const char* operator[](const int i) const
    {
        if (i < numOfSelections)
        {
            return selections[i].name;
        }
        return "";
    }
    void next()
    {
        atLine++;
        if (atLine >= numOfSelections){ atLine = 0;}
    }
    void prev()
    {
        atLine--;
        if (atLine < 0){ atLine = numOfSelections-1;}
    }
    void runCurrent() const
    {
        selections[atLine].run();
    }
private:
    static const int numOfSelections{2};
    const Selection selections[numOfSelections];
    int atLine{0};
};
Selections selections;



void loop()
{
#ifdef ESP8266
//    server.handleClient();
#else
    if (buttons.touched())
    {
        if (buttons.up())
        {
            selections.prev();
        }
        if (buttons.down())
        {
            selections.next();
        }
        if (buttons.select())
        {
            selections.runCurrent();
        }
        selections.show();
        buttons.clear();
    }
#endif
}
