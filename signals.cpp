#include "signals.h"


void signalHandler(int signum)
{
	std::cout << "Interrupt signal (" << signum << ") received.\n"; 
}