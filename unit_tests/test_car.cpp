#include "car.hpp"

car_test test_hello(void)
{
	assert_cmp(42, 42);
}

int main(void)
{
	test_hello();
}
