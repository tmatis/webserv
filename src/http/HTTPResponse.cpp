#include "HTTPResponse.hpp"
#include <sstream>

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
	505
};

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
	"HTTP Version Not Supported"
};

std::string const &HTTPResponse::status_code_to_string(status_code code)
{
	for (size_t i = 0; i < sizeof(_status_code) / sizeof(_status_code[0]); i++)
	{
		if (_status_code[i] == code)
			return _status_message[i];
	}
	return _status_message[0];
}

template <typename T>
std::string itoa(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

HTTPResponse::HTTPResponse(void)
	: HTTPGeneral(), _status(OK), _is_ready(false)
{}

HTTPResponse::HTTPResponse(HTTPResponse const &other)
	: HTTPGeneral(other), _status(other._status), _is_ready(other._is_ready)
{}

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

HTTPResponse::~HTTPResponse(void)
{}

void HTTPResponse::setStatus(status_code code)
{
	_status = code;
}

status_code HTTPResponse::getStatus(void) const
{
	return _status;
}

void HTTPResponse::setReady(bool b)
{
	_is_ready = b;
	if (_is_ready)
	{
		_header.addValue("Content-Length",
			std::vector<std::string>(1, itoa(_body.size())));
	}
}

bool HTTPResponse::isReady(void) const
{
	return _is_ready;
}

void HTTPResponse::clear(void)
{
	HTTPGeneral::clear();
	_status = OK;
	_is_ready = false;
}

std::string HTTPResponse::toString(void) const
{
	std::string res;

	res += itoa(_status) + " " + status_code_to_string(_status) += "\r\n";
	res += _header.toString();
	res += "\r\n";
	res += _body;
	return (res);
}
