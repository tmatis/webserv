#include "../car.hpp"

car_test test_HTTPURI(void)
{
	HTTPURI uri("http://www.example.com:8080/");
	
	assert(uri.getScheme() == "http");
	assert(uri.getHost() == "www.example.com");
	assert(uri.getPort() == 8080);
	assert(uri.getPath() == "/");
	assert(uri.getQuery().size() == 0);
	assert(uri.getFragment() == "");

	HTTPURI uri2("http://www.example.com/path/to/file.html?query=value#fragment");

	assert(uri2.getScheme() == "http");
	assert(uri2.getHost() == "www.example.com");
	assert(uri2.getPort() == 80);
	assert(uri2.getPath() == "/path/to/file.html");
	assert(uri2.getQuery().size() == 1);
	assert(*uri2.getQueryValue("query") == "value");
	assert(uri2.getFragment() == "fragment");

	HTTPURI uri3("http://www.example.com/path/to/file.html?query=value&query2=yolo#fragment");

	assert(uri3.getScheme() == "http");
	assert(uri3.getHost() == "www.example.com");
	assert(uri3.getPort() == 80);
	assert(uri3.getPath() == "/path/to/file.html");
	assert(uri3.getQuery().size() == 2);
	assert(*uri3.getQueryValue("query") == "value");
	assert(*uri3.getQueryValue("query2") == "yolo");
	assert(uri3.getFragment() == "fragment");
}
