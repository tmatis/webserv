#include "HTTPRequest.hpp"
#include <sstream>

HTTPRequest::HTTPRequest(void)
	: HTTPGeneral() ,_method(""), _version(""),
		_uri("/"), _is_ready(false)
{

}

HTTPRequest::HTTPRequest(HTTPRequest const &src)
	: HTTPGeneral(src), _method(src._method),
		_version(src._version), _uri(src._uri), _is_ready(false)
{

}

HTTPRequest &HTTPRequest::operator=(HTTPRequest const &rhs)
{
	if (this != &rhs)
	{
		HTTPGeneral::operator=(rhs);
		_method = rhs._method;
		_version = rhs._version;
		_uri = rhs._uri;
		_is_ready = rhs._is_ready;
	}
	return (*this);
}

HTTPRequest::~HTTPRequest(void)
{

}

void HTTPRequest::setMethod(std::string const &method)
{
	_method = method;
}

std::string const &HTTPRequest::getMethod(void) const
{
	return (_method);
}

void HTTPRequest::setVersion(std::string const &version)
{
	_version = version;
}

std::string const &HTTPRequest::getVersion(void) const
{
	return (_version);
}

void HTTPRequest::setURI(std::string const &uri)
{
	_uri = uri;
}

std::string const &HTTPRequest::getURI(void) const
{
	return (_uri);
}

bool HTTPRequest::isReady(void) const
{
	return (_is_ready);
}

void HTTPRequest::parseChunk(std::string const &chunk)
{
	_buffer += chunk;
	// replace \r\n by \n
	size_t pos = 0;
	while ((pos = _buffer.find("\r\n", pos)) != std::string::npos)
	{
		_buffer.replace(pos, 2, "\n");
		pos++;
	}
	// replace \r by \n
	pos = 0;
	while ((pos = _buffer.find("\r", pos)) != std::string::npos)
	{
		_buffer.replace(pos, 1, "\n");
		pos++;
	}

	// parse header if not already done
	if (_method == "" && _buffer.find("\n") != std::string::npos)
	{
		// get first line
		size_t pos = _buffer.find("\n");
		std::string line = _buffer.substr(0, pos);
		_buffer.erase(0, pos + 1);
		
		std::stringstream ss(line);
		std::vector<std::string> tokens;
		std::string token;
		while (std::getline(ss, token, ' '))
			tokens.push_back(token);
		if (tokens.size() == 3)
		{
			_method = tokens[0];
			_uri = tokens[1];
			_version = tokens[2];
		}
		else
			throw std::exception(); // header malformed
	}
	else if (_method != "" && _buffer.find("\n") != std::string::npos)
	{
		// get first line
		size_t pos = _buffer.find("\n");
		std::string line = _buffer.substr(0, pos);
		_buffer.erase(0, pos + 1);

		if (line == "")
			_is_ready = true;
		else
			_header.parseLine(line);
	}
}
