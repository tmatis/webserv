#ifndef CAR_HPP
# define CAR_HPP

# include <iostream>

# define assert_cmp(x, y) car_assert_cmp(x, y, __LINE__, __FILE__)

typedef void car_test;

template <typename T>
void car_assert_cmp(T x, T y, int line, std::string filename)
{
	if (x == y)
		std::cout << "OK" << std::endl;
	else
		std::cout << "KO" << std::endl;
	std::cout << "File: " << filename << std::endl;
	std::cout << "Line: " << line << std::endl;
}

#endif
