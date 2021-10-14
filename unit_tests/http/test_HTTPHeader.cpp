# include "../car.hpp"

car_test test_parseLine(void)
{
	HTTPHeader header;

	header.parseLine("name: value");
	car_assert((*header.getValue("name")) == "value");

	header.parseLine("name: value1, value2");
	car_assert((*header.getValue("name")) == "value1, value2");

	header.parseLine("name: value3\n");
	car_assert((*header.getValue("name")) == "value3");

	header.parseLine("name: value4\r\n");
	car_assert((*header.getValue("name")) == "value4");
}

car_test test_toString(void)
{
	HTTPHeader header;

	header.parseLine("name: value");
	car_assert(header.toString() == "name: value\r\n");

	header.parseLine("name: value1");
	car_assert(header.toString() == "name: value1\r\n");

	header.parseLine("name2: value1");
	header.parseLine("name3: value1");
	header.parseLine("name4: value1,value2,value3");
	car_assert(header.toString() 
		== "name: value1\r\nname2: value1\r\nname3: value1\r\nname4: value1,value2,value3\r\n");

}
