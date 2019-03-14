
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
}
void Actuator::on() const
{
	digitalWrite(pin_, ON);
	//Serial.print("["); Serial.print(millis());Serial.print("] ACT");Serial.print(pin_);Serial.println(": ON");
}
void Actuator::off() const
{
	digitalWrite(pin_, OFF);
	//Serial.print("["); Serial.print(millis());Serial.print("] ACT");Serial.print(pin_);Serial.println(": OFF");
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
