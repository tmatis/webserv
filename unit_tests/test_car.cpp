#include "car.hpp"

car_test test(void);

car_test test_hello(void)
{
	assert(std::string("hello") == "hi");
}

int main(void)
{
	test_hello();
	test();
}
