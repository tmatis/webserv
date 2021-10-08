# include "../car.hpp"

car_test test_parseLine(void)
{
	HTTPHeader header;

	header.parseLine("name: value");
	assert((*header.getHeaderValue("name"))[0] == "value");
	assert(header.getHeaderValue("name")->size() == 1);

	header.parseLine("name: value1, value2");
	assert((*header.getHeaderValue("name"))[1] == "value2");
	assert(header.getHeaderValue("name")->size() == 2);

	header.parseLine("name: value3\n");
	assert((*header.getHeaderValue("name"))[0] == "value3");

	header.parseLine("name: value4\r\n");
	assert((*header.getHeaderValue("name"))[0] == "value4");

}

car_test test_toString(void)
{
	HTTPHeader header;

	header.parseLine("name: value");
	assert(header.toString() == "name: value\r\n");

	header.parseLine("name: value1");
	assert(header.toString() == "name: value1\r\n");

	header.parseLine("name2: value1");
	header.parseLine("name3: value1");
	header.parseLine("name4: value1,value2,value3");
	assert(header.toString() 
		== "name: value1\r\nname2: value1\r\nname3: value1\r\nname4: value1,value2,value3\r\n");

}
