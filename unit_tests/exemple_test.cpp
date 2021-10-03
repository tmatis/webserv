#include "car.hpp"

car_test exemple_ok(void)
{
	assert(5 == 5);
	assert(5 != 6);
}

car_test exemple_ko(void)
{
	assert(5 == 6);
	assert(5 != 5);
}
