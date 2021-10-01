#ifndef CAR_HPP
# define CAR_HPP

# include <iostream>

# define assert(x) car_assert(x, __LINE__, __FILE__,  __func__, #x)

typedef void car_test;

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define RESET "\033[0m"

typedef void test_function(void);

void run_tests(test_function *tests[]);


template <typename T>
void car_assert(T x, int line, std::string filename, std::string funcname,
	std::string x_str)
{
	if (!x)
	{
		std::cout << CYAN << BOLD << " >>> " << RESET
		<< RED << "[✖] " << BLUE << "Assertion" 
		<< RED << " failed: " << RESET << BOLD
		"`" << x_str << "`" << RESET << " in " << funcname 
		<< "(" << filename << ":" << line << ")" << std::endl;
	}
}

#endif
