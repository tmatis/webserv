#include "../car.hpp"

car_test test_response(void)
{
	HTTPResponse response;

	car_assert(response.isReady() == false);
	car_assert_cmp(response.getStatus(), 201);
	car_assert(HTTPResponse::status_code_to_string(response.getStatus()) == "OK");
	response.setBody("Hello World!");
	response.setReady(true);
	car_assert(response.isReady() == true);
	car_assert(response.getBody() == "Hello World!");
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
