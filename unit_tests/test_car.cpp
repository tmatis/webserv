#include "car.hpp"

car_test test_hello(void)
{
	assert(std::string("hello") == "hi");
}

car_test other_test(void)
{
	assert(std::string("hello") == "hello");
}
