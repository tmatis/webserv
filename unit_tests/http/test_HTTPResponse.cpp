#include "../car.hpp"

car_test test_response(void)
{
	HTTPResponse response;

	assert(response.isReady() == false);
	assert(response.getStatus() == 200);
	assert(HTTPResponse::status_code_to_string(response.getStatus()) == "OK");
	response.setBody("Hello World!");
	response.setReady(true);
	assert(response.isReady() == true);
	assert(response.getBody() == "Hello World!");
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
	assert(response.toString() == expected_toString);
}
