
#include "Buttons.h"
#include "Arduino.h"
#ifdef ESP8266
#include "Pins_12e.h"
#else
#include "Pins.h"
#endif
#ifndef ESP8266
#include "PinChangeInt.h"
#endif

namespace stamping
{

//int Buttons::upValue{0};
//void Buttons::upButtonCB()
//{
//    upValue++;
//}
//int Buttons::downValue{0};
//void Buttons::downButtonCB()
//{
//    downValue++;
//}
volatile int Buttons::selectValue{0};
void Buttons::selectButtonCB()
{
    selectValue++;
}
//int Buttons::backValue{0};
//void Buttons::backButtonCB()
//{
//
//}
void Buttons::init()
{
#ifndef ESP8266
//    pinMode(upPin, INPUT_PULLUP);
//    pinMode(downPin, INPUT_PULLUP);
    pinMode(selectPin, INPUT_PULLUP);
//    pinMode(backPin, INPUT_PULLUP);
//    attachPinChangeInterrupt(downPin, downButtonCB, RISING);
//    attachPinChangeInterrupt(upPin, upButtonCB, RISING);
    attachPinChangeInterrupt(selectPin, selectButtonCB, RISING);
//    attachPinChangeInterrupt(backPin, backButtonCB, RISING);
#else
#ifndef EXTERNAL_PROGRAM_RUNNER
    pinMode(selectPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(selectPin), selectButtonCB, RISING);
#endif
#endif
}
//bool Buttons::touched() const
//{
//    return (upValue or downValue or selectValue or backValue);
//}
void Buttons::clear()
{
//    upValue = 0;
//    downValue = 0;
    selectValue = 0;
//    backValue = 0;
}
bool Buttons::select() const
{
    return selectValue;
}
//bool Buttons::back() const
//{
//    return backValue;
//}
//bool Buttons::up() const
//{
//    return upValue;
//}
//bool Buttons::down() const
//{
//    return downValue;
//}
void Buttons::log() const
{
//    Serial.print("Up: "); Serial.print(upValue);
//    Serial.print(", Down: "); Serial.print(downValue);
    Serial.print(", Sel: "); Serial.print(selectValue);
//    Serial.print(", Back: "); Serial.println(backValue);
}

}
