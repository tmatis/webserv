#include "car.hpp"

int success_assert = 0;
int	fail_assert = 0;
std::string filter = "";

#ifdef __linux__
# define OS "Linux"
#elif __APPLE__
# define OS "MacOS"
#else
# define OS "Unknow"
#endif

int run_tests(test_function *tests[])
{
	success_assert = 0;
	fail_assert = 0;
	int nb_tests;
	for (nb_tests = 0; tests[nb_tests] != NULL; nb_tests++);
	std::cout << BOLD
		<< "========================== TEST SESSION START =========================="
		<< RESET << std::endl;
	std::cout << "Build date: " << __DATE__ << " " << __TIME__ << std::endl;
	std::cout << "OS: " << OS << std::endl ;
	std::cout << "Compiler: " << __VERSION__ << std::endl << std::endl;
	std::cout << BOLD << "Functions collected: " << nb_tests << RESET << std::endl;
	if (filter != "")
		std::cout << BOLD << "Filter: " << filter << RESET << std::endl << std::endl;

	for (int i = 0; tests[i] != NULL; i++)
		tests[i]();

	std::cout << std::endl;
	if (!fail_assert)
		std::cout << GREEN 
			<< "========================== TEST SESSION END ==========================="
			<< RESET << std::endl;
	else
		std::cout << BOLD << RED
			<< "========================== TEST SESSION END ==========================="
			<< RESET << std::endl;
	std::cout << BOLD << "Summary: " << success_assert << "/" << success_assert + fail_assert 
		<< " (" << ((success_assert * 100) / (success_assert + fail_assert)) << "%)" << std::endl;
	return (fail_assert != 0);
}
