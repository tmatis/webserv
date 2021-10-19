#include "HTTPGeneral.hpp"



/* ************************ UTILITY ************************** */

std::pair<size_t, short> find_nl(std::string const &buffer)
{
	size_t pos = buffer.find("\r\n");
	if (pos != std::string::npos)
		return (std::make_pair(pos, 2));
	pos = buffer.find("\n");
	if (pos != std::string::npos)
		return (std::make_pair(pos, 1));
	pos = buffer.find("\r");
	if (pos != std::string::npos)
		return (std::make_pair(pos, 1));
	return (std::make_pair(std::string::npos, 0));
}

/* ********************** CONSTRUCTORS *********************** */


HTTPGeneral::HTTPGeneral()
	: _header(), _body()
{}

HTTPGeneral::HTTPGeneral(HTTPGeneral const &src)
	: _header(src._header), _body(src._body)
{}

HTTPGeneral::~HTTPGeneral()
{}

/* ********************** OPERATORS ************************* */

HTTPGeneral &HTTPGeneral::operator=(HTTPGeneral const &rhs)
{
	if (this != &rhs)
	{
		_header = rhs._header;
		_body = rhs._body;
	}
	return (*this);
}

/* ************************* GETTERS ************************* */

HTTPHeader &HTTPGeneral::getHeader()
{
	return (_header);
}

std::string const &HTTPGeneral::getBody() const
{
	return (_body);
}

size_t HTTPGeneral::getBodySize(void) const
{
	return (_body.size());
}

/* ************************* SETTERS ************************* */


void HTTPGeneral::setHeader(HTTPHeader const &header)
{
	_header = header;
}

void HTTPGeneral::setBody(std::string const &body)
{
	_body = body;
}

/* ************************* METHODS ************************* */

void HTTPGeneral::clear(void)
{
	_header.clear();
	_body.clear();
}

