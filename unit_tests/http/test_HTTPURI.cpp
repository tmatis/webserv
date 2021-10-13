#include "../car.hpp"

car_test test_HTTPURI_absolute(void)
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

	HTTPURI uri4("http://www.example.com/path/to/file.html?query=value&query2=yolo&query3=yola");

	assert(uri4.getScheme() == "http");
	assert(uri4.getHost() == "www.example.com");
	assert(uri4.getPort() == 80);
	assert(uri4.getPath() == "/path/to/file.html");
	assert(uri4.getQuery().size() == 3);
	assert(*uri4.getQueryValue("query") == "value");
	assert(*uri4.getQueryValue("query2") == "yolo");
	assert(*uri4.getQueryValue("query3") == "yola");
	assert(uri4.getFragment() == "");

	HTTPURI uri5("http://www.example.com/path/to/file.html#fragment");

	assert(uri5.getScheme() == "http");
	assert(uri5.getHost() == "www.example.com");
	assert(uri5.getPort() == 80);
	assert(uri5.getPath() == "/path/to/file.html");
	assert(uri5.getQuery().size() == 0);
	assert(uri5.getFragment() == "fragment");

	HTTPURI uri6("http://www.example.com#fragment");

	assert(uri6.getScheme() == "http");
	assert(uri6.getHost() == "www.example.com");
	assert(uri6.getPort() == 80);
	assert(uri6.getPath() == "/");
	assert(uri6.getQuery().size() == 0);
	assert(uri6.getFragment() == "fragment");

	HTTPURI uri7("http://www.example.com/path/to/file.html");

	assert(uri7.getScheme() == "http");
	assert(uri7.getHost() == "www.example.com");
	assert(uri7.getPort() == 80);
	assert(uri7.getPath() == "/path/to/file.html");
	assert(uri7.getQuery().size() == 0);
	assert(uri7.getFragment() == "");

	HTTPURI uri8("http://www.example.com/path/to/file.html?query=value");

	assert(uri8.getScheme() == "http");
	assert(uri8.getHost() == "www.example.com");
	assert(uri8.getPort() == 80);
	assert(uri8.getPath() == "/path/to/file.html");
	assert(uri8.getQuery().size() == 1);
	assert(*uri8.getQueryValue("query") == "value");
	assert(uri8.getFragment() == "");
}

car_test test_HTTPURI_relative(void)
{
	HTTPURI uri("/path/to/file.html?query=value#fragment");

	assert(uri.getScheme() == "");
	assert(uri.getHost() == "");
	assert(uri.getPort() == 80);
	assert(uri.getPath() == "/path/to/file.html");
	assert(uri.getQuery().size() == 1);
	assert(*uri.getQueryValue("query") == "value");
	assert(uri.getFragment() == "fragment");

	HTTPURI uri2("/path/to/file.html?query=value&query2=yolo#fragment");

	assert(uri2.getScheme() == "");
	assert(uri2.getHost() == "");
	assert(uri2.getPort() == 80);
	assert(uri2.getPath() == "/path/to/file.html");
	assert(uri2.getQuery().size() == 2);
	assert(*uri2.getQueryValue("query") == "value");
	assert(*uri2.getQueryValue("query2") == "yolo");
	assert(uri2.getFragment() == "fragment");

	HTTPURI uri3("/path/to/file.html?query=value&query2=yolo&query3=yola");

	assert(uri3.getScheme() == "");
	assert(uri3.getHost() == "");
	assert(uri3.getPort() == 80);
	assert(uri3.getPath() == "/path/to/file.html");
	assert(uri3.getQuery().size() == 3);
	assert(*uri3.getQueryValue("query") == "value");
	assert(*uri3.getQueryValue("query2") == "yolo");
	assert(*uri3.getQueryValue("query3") == "yola");
	assert(uri3.getFragment() == "");

	HTTPURI uri4("/path/to/file.html#fragment");

	assert(uri4.getScheme() == "");
	assert(uri4.getHost() == "");
	assert(uri4.getPort() == 80);
	assert(uri4.getPath() == "/path/to/file.html");
	assert(uri4.getQuery().size() == 0);
	assert(uri4.getFragment() == "fragment");

	HTTPURI uri5("/#fragment");

	assert(uri5.getScheme() == "");
	assert(uri5.getHost() == "");
	assert(uri5.getPort() == 80);
	assert(uri5.getPath() == "/");
	assert(uri5.getQuery().size() == 0);
	assert(uri5.getFragment() == "fragment");

	HTTPURI uri6("/");

	assert(uri6.getScheme() == "");
	assert(uri6.getHost() == "");
	assert(uri6.getPort() == 80);
	assert(uri6.getPath() == "/");
	assert(uri6.getQuery().size() == 0);
	assert(uri6.getFragment() == "");

	HTTPURI uri7("/path/to/file.html");

	assert(uri7.getScheme() == "");
	assert(uri7.getHost() == "");
	assert(uri7.getPort() == 80);
	assert(uri7.getPath() == "/path/to/file.html");
	assert(uri7.getQuery().size() == 0);
	assert(uri7.getFragment() == "");

	HTTPURI uri8("/path/to/file.html?query=value");

	assert(uri8.getScheme() == "");
	assert(uri8.getHost() == "");
	assert(uri8.getPort() == 80);
	assert(uri8.getPath() == "/path/to/file.html");
	assert(uri8.getQuery().size() == 1);
	assert(*uri8.getQueryValue("query") == "value");
	assert(uri8.getFragment() == "");
}
