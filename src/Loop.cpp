#include "Arduino.h"
#include "Components.h"

struct Selection
{
    Selection(const char* const n)
    : name{n}
    {}
    void run() const
    {
        Serial.print("Suoritetaan: "); Serial.println(name);
    }

    const char* const name;
};

class Selections
{
public:
    Selections()
    : selections{
    Selection("Aja ohjelma"),
    Selection("Luo ohjelma")
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
	stamping::Output o;
	phases.add(o);
}
