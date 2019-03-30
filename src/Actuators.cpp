
#include "Actuators.h"
#include "Arduino.h"

namespace stamping
{

Actuator::Actuator(const int pin)
: state_{OFF},
  pin_{pin}
{

}
Actuator::Actuator(const int pin, const int state)
: state_{state},
  pin_{pin}
{
}

void Actuator::start() const
{
	pinMode(pin_, OUTPUT);
	off();
}
void Actuator::on() const
{
	digitalWrite(pin_, ON);
#ifndef UNIT_TEST
	Serial.print(F("[")); Serial.print(millis());Serial.print(F("] ACT"));Serial.print(pin_);Serial.println(": ON");
#endif
}
void Actuator::off() const
{
	digitalWrite(pin_, OFF);
#ifndef UNIT_TEST
	Serial.print(F("[")); Serial.print(millis());Serial.print(F("] ACT"));Serial.print(pin_);Serial.println(": OFF");
#endif
}

Actuators::Actuators()
: numOf{0},
  actuators{nullptr}
{}

void Actuators::add(Actuator& act)
{
	act.start();
	actuators[numOf] = &act;
	numOf++;
}
Actuator Actuators::nullActuator{Actuator::INVALID_PIN};

int Actuators::size() const
{
	return numOf;
}

const Actuator& Actuators::operator[](const int idx) const
{
	if (idx < numOf)
	{
		if (actuators[idx])
		{
			return *(actuators[idx]);
		}
	}
	return nullActuator;
}
Actuator& Actuators::get(const int idx)
{
	if (idx < numOf)
	{
		Actuator* act = actuators[idx];
		if (act)
		{
			return *act;
		}
	}
	return nullActuator;
}
void Actuators::off()
{
	for (int i = 0; i < size(); i++)
	{
		if(actuators[i])
		{
			actuators[i]->off();
		}
	}
}

}
