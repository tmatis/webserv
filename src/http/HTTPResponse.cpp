#include "HTTPResponse.hpp"
#include <sstream>

/* *********************** UTILITIES ************************* */

const short HTTPResponse::_status_code[35] =
	{
		100,
		101,
		102,
		103,
		200,
		201,
		202,
		204,
		301,
		302,
		303,
		304,
		307,
		400,
		401,
		403,
		404,
		405,
		406,
		408,
		409,
		410,
		411,
		412,
		413,
		414,
		415,
		416,
		417,
		500,
		501,
		502,
		503,
		504,
		505};

const std::string HTTPResponse::_status_message[35] =
	{
		"Continue",
		"Switching Protocols",
		"Processing",
		"Early Hints",
		"OK",
		"Created",
		"Accepted",
		"No Content",
		"Moved Permanently",
		"Found",
		"See Other",
		"Not Modified",
		"Temporary Redirect",
		"Bad Request",
		"Unauthorized",
		"Forbidden",
		"Not Found",
		"Method Not Allowed",
		"Not Acceptable",
		"Request Timeout",
		"Conflict",
		"Gone",
		"Length Required",
		"Precondition Failed",
		"Request Entity Too Large",
		"Request-URI Too Long",
		"Unsupported Media Type",
		"Requested Range Not Satisfiable",
		"Expectation Failed",
		"Internal Server Error",
		"Not Implemented",
		"Bad Gateway",
		"Service Unavailable",
		"Gateway Timeout",
		"HTTP Version Not Supported"};

std::string const &HTTPResponse::status_code_to_string(status_code code)
{
	for (size_t i = 0; i < sizeof(_status_code) / sizeof(_status_code[0]); i++)
	{
		if (_status_code[i] == code)
			return _status_message[i];
	}
	return _status_message[0];
}


// date at formatDate: <day-name>, <jour> <mois> <année> <heure>:<minute>:<seconde> GMT
// "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" or "Sun"
// day: 01 to 31
// month: 01 to 12
// year: 4 digits
// hour: 00 to 23
// minute: 00 to 59
// second: 00 to 59
// GMT: timezone
static std::string getDate(void)
{
	time_t t = time(NULL);
	struct tm *tmp = gmtime(&t);
	char buff[128];
	strftime(buff, sizeof(buff), "%a, %d %b %Y %H:%M:%S GMT", tmp);
	return (std::string(buff));
}

template <typename T>
std::string itoa(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

/* ********************** CONSTRUCTORS *********************** */

HTTPResponse::HTTPResponse(void)
	: HTTPGeneral(), _status(OK), _is_ready(false)
{
}

HTTPResponse::HTTPResponse(HTTPResponse const &other)
	: HTTPGeneral(other), _status(other._status), _is_ready(other._is_ready)
{
}

HTTPResponse::~HTTPResponse(void)
{
}

/* ********************** OPERATORS ************************* */


HTTPResponse &HTTPResponse::operator=(HTTPResponse const &other)
{
	if (this != &other)
	{
		HTTPGeneral::operator=(other);
		_status = other._status;
		_is_ready = other._is_ready;
	}
	return *this;
}

/* ************************* GETTERS ************************* */

bool HTTPResponse::isReady(void) const
{
	return _is_ready;
}


status_code HTTPResponse::getStatus(void) const
{
	return _status;
}

/* ************************* SETTERS ************************* */

void HTTPResponse::setReady(bool b)
{
	_is_ready = b;
}

void HTTPResponse::setStatus(status_code code)
{
	_status = code;
}

void HTTPResponse::setContentType(std::string const &type)
{
	if (type.empty())
		return;
	// check if we need to add the charset
	if (type == "text/html"
		|| type == "text/plain"
		|| type == "text/css"
		|| type == "text/javascript"
		|| type == "text/xml"
		|| type == "application/json"
		|| type == "application/xml"
		|| type == "application/x-www-form-urlencoded")

		_header.addValue("Content-Type", type + "; charset=utf-8");
	else
		_header.addValue("Content-Type", type);
}

void HTTPResponse::setConnection(HTTPConnectionType type)
{
	if (type == HTTP_CONNECTION_KEEP_ALIVE)
		_header.addValue("Connection", "keep-alive");
	else if (type == HTTP_CONNECTION_CLOSE)
		_header.addValue("Connection", "close");
}

/* ************************* METHODS ************************* */

void HTTPResponse::clear(void)
{
	HTTPGeneral::clear();
	_status = OK;
	_is_ready = false;
}

// transform response to string ready to be sent

std::string HTTPResponse::toString(void)
{
	std::string res;

	_header.addValue("Server", "Webserv");
	_header.addValue("Date", getDate());
	if (_body.size())
		_header.addValue("Content-Length", itoa(_body.size()));
	if (!_header.getValue("Connection"))
		setConnection(HTTP_CONNECTION_KEEP_ALIVE);
	res += "HTTP/1.1 " + itoa(_status) + " " + status_code_to_string(_status) + "\r\n";
	res += _header.toString();
	res += "\r\n";
	res += _body;
	return (res);
}
