#include "car.hpp"
#include <stdlib.h>

car_test test_atoi(void)
{
	assert(atoi("5") == 5);
	assert(atoi("-5") == -5);
	assert(atoi("0") == 0);
	assert(atoi("-0") == 0);
	assert(atoi("") == 0);
	assert(atoi("-") == 0);
	//assert(atoi("-42") == 42); // will fail
}
