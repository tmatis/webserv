#include "HTTPGeneral.hpp"

HTTPGeneral::HTTPGeneral()
	: _header(), _body()
{}

HTTPGeneral::HTTPGeneral(HTTPGeneral const &src)
	: _header(src._header), _body(src._body)
{}

HTTPGeneral &HTTPGeneral::operator=(HTTPGeneral const &rhs)
{
	if (this != &rhs)
	{
		_header = rhs._header;
		_body = rhs._body;
	}
	return (*this);
}

HTTPGeneral::~HTTPGeneral()
{}

HTTPHeader const &HTTPGeneral::getHeader() const
{
	return (_header);
}

std::string const &HTTPGeneral::getBody() const
{
	return (_body);
}

void HTTPGeneral::setHeader(HTTPHeader const &header)
{
	_header = header;
}

void HTTPGeneral::setBody(std::string const &body)
{
	_body = body;
}
