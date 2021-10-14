#include "../car.hpp"

car_test test_HTTPURI_absolute(void)
{
	
	HTTPURI uri("http://www.example.com:8080/");
	
	car_assert(uri.getScheme() == "http");
	car_assert(uri.getHost() == "www.example.com");
	car_assert(uri.getPort() == 8080);
	car_assert(uri.getPath() == "/");
	car_assert(uri.getQuery().size() == 0);
	car_assert(uri.getFragment() == "");

	HTTPURI uri2("http://www.example.com/path/to/file.html?query=value#fragment");

	car_assert(uri2.getScheme() == "http");
	car_assert(uri2.getHost() == "www.example.com");
	car_assert(uri2.getPort() == 80);
	car_assert(uri2.getPath() == "/path/to/file.html");
	car_assert(uri2.getQuery().size() == 1);
	car_assert(*uri2.getQueryValue("query") == "value");
	car_assert(uri2.getFragment() == "fragment");

	HTTPURI uri3("http://www.example.com/path/to/file.html?query=value&query2=yolo#fragment");

	car_assert(uri3.getScheme() == "http");
	car_assert(uri3.getHost() == "www.example.com");
	car_assert(uri3.getPort() == 80);
	car_assert(uri3.getPath() == "/path/to/file.html");
	car_assert(uri3.getQuery().size() == 2);
	car_assert(*uri3.getQueryValue("query") == "value");
	car_assert(*uri3.getQueryValue("query2") == "yolo");
	car_assert(uri3.getFragment() == "fragment");

	HTTPURI uri4("http://www.example.com/path/to/file.html?query=value&query2=yolo&query3=yola");

	car_assert(uri4.getScheme() == "http");
	car_assert(uri4.getHost() == "www.example.com");
	car_assert(uri4.getPort() == 80);
	car_assert(uri4.getPath() == "/path/to/file.html");
	car_assert(uri4.getQuery().size() == 3);
	car_assert(*uri4.getQueryValue("query") == "value");
	car_assert(*uri4.getQueryValue("query2") == "yolo");
	car_assert(*uri4.getQueryValue("query3") == "yola");
	car_assert(uri4.getFragment() == "");

	HTTPURI uri5("http://www.example.com/path/to/file.html#fragment");

	car_assert(uri5.getScheme() == "http");
	car_assert(uri5.getHost() == "www.example.com");
	car_assert(uri5.getPort() == 80);
	car_assert(uri5.getPath() == "/path/to/file.html");
	car_assert(uri5.getQuery().size() == 0);
	car_assert(uri5.getFragment() == "fragment");

	HTTPURI uri6("http://www.example.com#fragment");

	car_assert(uri6.getScheme() == "http");
	car_assert(uri6.getHost() == "www.example.com");
	car_assert(uri6.getPort() == 80);
	car_assert(uri6.getPath() == "/");
	car_assert(uri6.getQuery().size() == 0);
	car_assert(uri6.getFragment() == "fragment");

	HTTPURI uri7("http://www.example.com/path/to/file.html");

	car_assert(uri7.getScheme() == "http");
	car_assert(uri7.getHost() == "www.example.com");
	car_assert(uri7.getPort() == 80);
	car_assert(uri7.getPath() == "/path/to/file.html");
	car_assert(uri7.getQuery().size() == 0);
	car_assert(uri7.getFragment() == "");

	HTTPURI uri8("http://www.example.com/path/to/file.html?query=value");

	car_assert(uri8.getScheme() == "http");
	car_assert(uri8.getHost() == "www.example.com");
	car_assert(uri8.getPort() == 80);
	car_assert(uri8.getPath() == "/path/to/file.html");
	car_assert(uri8.getQuery().size() == 1);
	car_assert(*uri8.getQueryValue("query") == "value");
	car_assert(uri8.getFragment() == "");
}

car_test test_HTTPURI_relative(void)
{
	HTTPURI uri("/path/to/file.html?query=value#fragment");

	car_assert(uri.getScheme() == "");
	car_assert(uri.getHost() == "");
	car_assert(uri.getPort() == 80);
	car_assert(uri.getPath() == "/path/to/file.html");
	car_assert(uri.getQuery().size() == 1);
	car_assert(*uri.getQueryValue("query") == "value");
	car_assert(uri.getFragment() == "fragment");

	HTTPURI uri2("/path/to/file.html?query=value&query2=yolo#fragment");

	car_assert(uri2.getScheme() == "");
	car_assert(uri2.getHost() == "");
	car_assert(uri2.getPort() == 80);
	car_assert(uri2.getPath() == "/path/to/file.html");
	car_assert(uri2.getQuery().size() == 2);
	car_assert(*uri2.getQueryValue("query") == "value");
	car_assert(*uri2.getQueryValue("query2") == "yolo");
	car_assert(uri2.getFragment() == "fragment");

	HTTPURI uri3("/path/to/file.html?query=value&query2=yolo&query3=yola");

	car_assert(uri3.getScheme() == "");
	car_assert(uri3.getHost() == "");
	car_assert(uri3.getPort() == 80);
	car_assert(uri3.getPath() == "/path/to/file.html");
	car_assert(uri3.getQuery().size() == 3);
	car_assert(*uri3.getQueryValue("query") == "value");
	car_assert(*uri3.getQueryValue("query2") == "yolo");
	car_assert(*uri3.getQueryValue("query3") == "yola");
	car_assert(uri3.getFragment() == "");

	HTTPURI uri4("/path/to/file.html#fragment");

	car_assert(uri4.getScheme() == "");
	car_assert(uri4.getHost() == "");
	car_assert(uri4.getPort() == 80);
	car_assert(uri4.getPath() == "/path/to/file.html");
	car_assert(uri4.getQuery().size() == 0);
	car_assert(uri4.getFragment() == "fragment");

	HTTPURI uri5("/#fragment");

	car_assert(uri5.getScheme() == "");
	car_assert(uri5.getHost() == "");
	car_assert(uri5.getPort() == 80);
	car_assert(uri5.getPath() == "/");
	car_assert(uri5.getQuery().size() == 0);
	car_assert(uri5.getFragment() == "fragment");

	HTTPURI uri6("/");

	car_assert(uri6.getScheme() == "");
	car_assert(uri6.getHost() == "");
	car_assert(uri6.getPort() == 80);
	car_assert(uri6.getPath() == "/");
	car_assert(uri6.getQuery().size() == 0);
	car_assert(uri6.getFragment() == "");

	HTTPURI uri7("/path/to/file.html");

	car_assert(uri7.getScheme() == "");
	car_assert(uri7.getHost() == "");
	car_assert(uri7.getPort() == 80);
	car_assert(uri7.getPath() == "/path/to/file.html");
	car_assert(uri7.getQuery().size() == 0);
	car_assert(uri7.getFragment() == "");

	HTTPURI uri8("/path/to/file.html?query=value");

	car_assert(uri8.getScheme() == "");
	car_assert(uri8.getHost() == "");
	car_assert(uri8.getPort() == 80);
	car_assert(uri8.getPath() == "/path/to/file.html");
	car_assert(uri8.getQuery().size() == 1);
	car_assert(*uri8.getQueryValue("query") == "value");
	car_assert(uri8.getFragment() == "");
}

car_test	test_percent_decoding(void)
{
	car_assert(HTTPURI::percentDecode("%20") == " ");
	car_assert(HTTPURI::percentDecode("%2F") == "/");
	car_assert(HTTPURI::percentDecode("%3F") == "?");
	car_assert(HTTPURI::percentDecode("+") == " ");

	// multiple percent encoded characters

	car_assert(HTTPURI::percentDecode("%2520") == "%20");
	car_assert(HTTPURI::percentDecode("%252F") == "%2F");
	car_assert(HTTPURI::percentDecode("%253F") == "%3F");
	car_assert(HTTPURI::percentDecode("%2525+") == "%25 ");
}
