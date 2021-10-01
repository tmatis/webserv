#include "car.hpp"

void run_tests(test_function *tests[])
{
	for (int i = 0; tests[i] != NULL; i++)
		tests[i]();
}
