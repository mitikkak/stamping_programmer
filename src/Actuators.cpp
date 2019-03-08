
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
	pinMode(pin_, OUTPUT);
}

void Actuator::activate()
{
	digitalWrite(pin_, ON);
}
void Actuator::deactivate()
{
	digitalWrite(pin_, OFF);
}

Actuators::Actuators()
: numOf{0},
  actuators{nullptr}
{}

void Actuators::add(Actuator& act)
{
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
void Actuators::deactivate()
{
	for (int i = 0; i < size(); i++)
	{
		if(actuators[i])
		{
			actuators[i]->deactivate();
		}
	}
}

}
