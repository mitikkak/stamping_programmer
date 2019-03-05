
#include "SerialPrinter.h"

SerialPrinter::SerialPrinter()
: b_len{0}
{

}
size_t SerialPrinter::write(uint8_t byte)
{
	buffer[b_len++] = byte;
	return 0;
}
const char* SerialPrinter::output() const
{
	return buffer;
}
