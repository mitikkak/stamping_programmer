
#pragma once
namespace stamping
{

class Actuator
{
public:
	Actuator(const int pin);
	Actuator(const int pin, const int state);
	static constexpr int ON{1};
	static constexpr int OFF{0};
	static constexpr int INVALID_PIN{0xFF};
	int state() const { return state_; }
	int pin() const { return pin_; }
	void start() const;
	void on() const;
	void off() const;
private:
	int state_{OFF};
	int pin_{INVALID_PIN};
};
class Actuators
{
public:
	Actuators();
	void add(Actuator& act);
	int size() const;
	const Actuator& operator[](const int idx) const;
	Actuator& get(const int idx);
	void off();

private:
	static Actuator nullActuator;
	int numOf{0};
	static constexpr int maxActuators{4};
	Actuator* actuators[maxActuators];

};

}
