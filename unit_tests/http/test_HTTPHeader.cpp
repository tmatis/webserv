# include "../car.hpp"

car_test test_parseLine(void)
{
	HTTPHeader header;

	header.parseLine("name: value");
	assert((*header.getHeaderValue("name"))[0] == "value");
	assert(header.getHeaderValue("name")->size() == 1);

	header.parseLine("name: value1");
	assert((*header.getHeaderValue("name"))[0] == "value1");
	assert(header.getHeaderValue("name")->size() == 1);
}
