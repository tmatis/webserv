#include "HTTPGeneral.hpp"


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

/* ************************* STATIC METHODS ****************** */

std::string HTTPGeneral::append_paths(const std::string& str1,
		const std::string& str2)
{
	size_t		last_index = str1.length() - 1;
	std::string	new_path;

	if (str1[last_index] == '/' || str2[0] == '/')
	{
		if (str1[last_index] == '/' && str2[0] == '/') // they both have one '/'
			new_path = str1.substr(0, last_index) + str2;
		else // only one '/'
			new_path = str1 + str2;
	}
	else // no '/' in both strings
		new_path = str1 + "/" + str2;
	return (new_path);
}