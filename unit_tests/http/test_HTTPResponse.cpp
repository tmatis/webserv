#include "../car.hpp"

car_test test_response(void)
{
	HTTPResponse response;

	car_assert(response.isReady() == false);
	car_assert_cmp(response.getStatus(), 200);
	car_assert_cmp(HTTPResponse::status_code_to_string(response.getStatus()), "OK");
	response.setBody("Hello World!");
	response.setReady(true);
	car_assert_cmp(response.isReady(), true);
	car_assert_cmp(response.getBody(), "Hello World!");
	response.setContentType("text/plain");
	char date[128];
	time_t t = time(NULL);
	struct tm *tmp = gmtime(&t);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", tmp);

	std::string expected_toString =
	"HTTP/1.1 200 OK\r\n";
	expected_toString += "Content-Type: text/plain; charset=UTF-8\r\n";
	expected_toString += "Server: Webserv\r\n";
	expected_toString += "Date: ";
	expected_toString += date;
	expected_toString += "\r\nContent-Length: 12\r\n";
	expected_toString += "Connection: keep-alive\r\n";
	expected_toString += "\r\n";
	expected_toString += "Hello World!";
	car_assert(response.toString() == expected_toString);
}

car_test cgi_response_parsing(void)
{
	HTTPResponse cgi_res;

	cgi_res.parseCgIRes("Content-type: text/html\r\n");
	cgi_res.parseCgIRes("\r\n");
	cgi_res.parseCgIRes("hello\r\n");


	car_assert_cmp(cgi_res.getStatus(), 200);
	car_assert_cmp(*cgi_res.getHeader().getValue("Content-type"), "text/html");
	car_assert_cmp(cgi_res.getBody(), "hello\r\n");

	cgi_res.clear();

	cgi_res.parseCgIRes("Content-type: text/html\r\n\r\nhello\r\n");

	car_assert_cmp(cgi_res.getStatus(), 200);
	car_assert_cmp(*cgi_res.getHeader().getValue("Content-type"), "text/html");
	car_assert_cmp(cgi_res.getBody(), "hello\r\n");
}

car_test cgi_response_parsing_weird(void)
{
	HTTPResponse cgi_res;

	cgi_res.parseCgIRes("hello\r\n");

	car_assert_cmp(cgi_res.getStatus(), 200);
	car_assert_cmp(cgi_res.getBody(), "hello\r\n");
	
	cgi_res.clear();

	cgi_res.parseCgIRes("");

	car_assert_cmp(cgi_res.getStatus(), 200);
	car_assert_cmp(cgi_res.getBody(), "");
}

car_test cgi_response_with_status(void)
{
	HTTPResponse cgi_res;

	cgi_res.parseCgIRes("Content-type: text/html\r\n");
	cgi_res.parseCgIRes("Status: 404 Not found\r\n");
	cgi_res.parseCgIRes("\r\n");
	cgi_res.parseCgIRes("hello\r\n");

	car_assert_cmp(cgi_res.getStatus(), 404);
	car_assert_cmp(*cgi_res.getHeader().getValue("Content-type"), "text/html");

}
