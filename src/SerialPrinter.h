
#include "Print.h"

class SerialPrinter : public Print
{
public:
	SerialPrinter();
	size_t write(uint8_t);
	const char* output() const;
private:
	static const int b_size{50};
	char buffer[b_size]{};
	int b_len;

};
