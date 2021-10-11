#include "HTTPRequest.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>


HTTPRequest::HTTPRequestException::HTTPRequestException(const char *errinfo)
	: _info(errinfo)
{
}

HTTPRequest::HTTPRequestException::~HTTPRequestException() throw()
{
}

const char *HTTPRequest::HTTPRequestException::what() const throw()
{
	return (_info.c_str());
}

HTTPRequest::HTTPRequest(void)
	: HTTPGeneral(), _method(""), _version(""),
	  _uri("/"), _is_ready(false), _command_set(false),
	  _header_set(false), _buffer(""),
	  _host("")
{
}

HTTPRequest::HTTPRequest(HTTPRequest const &src)
	: HTTPGeneral(src), _method(src._method),
	  _version(src._version), _uri(src._uri),
	  _is_ready(false), _command_set(src._command_set), _header_set(src._header_set), _buffer(src._buffer)
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
		_command_set = rhs._command_set;
		_header_set = rhs._header_set;
		_buffer = rhs._buffer;
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

static void format_request(std::string &buffer)
{
	size_t pos = 0;
	while ((pos = buffer.find("\r\n", pos)) != std::string::npos)
	{
		buffer.replace(pos, 2, "\n");
		pos++;
	}
	// replace \r by \n
	pos = 0;
	while ((pos = buffer.find("\r", pos)) != std::string::npos)
	{
		buffer.replace(pos, 1, "\n");
		pos++;
	}
}

static std::string get_line_cut(std::string &buffer)
{
	size_t pos = buffer.find("\n");
	if (pos == std::string::npos)
		return ("");
	std::string line = buffer.substr(0, pos);
	buffer.erase(0, pos + 1);
	return (line);
}

static std::vector<std::string> parse_request_command(std::string &buffer)
{
	std::string line = get_line_cut(buffer);

	std::stringstream ss(line);
	std::vector<std::string> tokens;
	std::string token;
	while (std::getline(ss, token, ' '))
		tokens.push_back(token);
	return (tokens);
}

// TODO: weird things happen when the command in not well formed in nginx

void HTTPRequest::parseChunk(std::string const &chunk)
{
	_buffer += chunk;

	format_request(_buffer);

	// parse headers if not already done
	if (!_command_set && _buffer.find("\n") != std::string::npos)
	{
		std::vector<std::string> tokens = parse_request_command(_buffer);
		if (tokens.size() == 3)
		{
			_method = tokens[0];
			_uri = tokens[1];
			if (_uri[0] != '/')
				throw HTTPRequestException("URI must start with '/'");
			_version = tokens[2];
			_command_set = true;
		}
		else
			throw HTTPRequestException("Command malformed");
	}
	if (_command_set && !_header_set && _buffer.find("\n") != std::string::npos)
	{
		size_t pos;

		while ((pos = _buffer.find("\n")) != std::string::npos)
		{
			std::string line = get_line_cut(_buffer);

			if (line == "")
			{
				_header_set = true;
				if (!_header.getValue("Content-Length"))
					_is_ready = true;
			}
			else
				_header.parseLine(line);
		}
	}
	if (_header_set && !_is_ready)
	{
		const std::string *content_length = _header.getValue("Content-Length");
		if (content_length)
		{
			size_t content_length_value = std::strtoul(content_length->c_str(), NULL, 10);
			if (_body.size() + _buffer.size() >= content_length_value)
			{
				std::string tmp = _buffer.substr(0, content_length_value - _body.size());
				_body += tmp;
				_buffer.erase(0, content_length_value - _body.size());
			}
			else
			{
				_body += _buffer;
				_buffer.clear();
			}
			if (_body.size() == content_length_value)
				_is_ready = true;
		}
		else
			_is_ready = true;
	}
	if (_is_ready)
	{
		if (_header.isValid() == false)
			throw HTTPRequestException("Invalid header");
		const std::string *values = _header.getValue("Host");
		if (values)
			_host = *values;
		else
			throw HTTPRequestException("Host header not found");
	}
}

void HTTPRequest::clear(void)
{
	HTTPGeneral::clear();
	_header.clear();
	_method = "";
	_version = "";
	_uri = "/";
	_is_ready = false;
	_command_set = false;
	_header_set = false;
	this->parseChunk(_buffer);
}

std::string const &HTTPRequest::getHost(void) const
{
	return (_host);
}

std::string const *HTTPRequest::getUserAgent(void) const
{
	const std::string *value = _header.getValue("User-Agent");
	if (value)
		return (value);
	else
		return (NULL);
}

static std::vector<std::string> split_header_value(std::string const &value)
{
	std::vector<std::string> tokens;
	std::stringstream ss(value);
	std::string token;
	while (std::getline(ss, token, ','))
	{
		while (token.size() && token[0] == ' ')
			token.erase(0, 1);
		while (token.size() && token[token.size() - 1] == ' ')
			token.erase(token.size() - 1, 1);
		tokens.push_back(token);
	}
	return (tokens);
}

std::vector<std::string> HTTPRequest::getAccept(void) const
{
	std::vector<std::string> values;
	const std::string *value = _header.getValue("Accept");
	if (value)
		values = split_header_value(*value);
	return (values);
}

HTTPConnectionType HTTPRequest::getConnection(void) const
{
	const std::string *value = _header.getValue("Connection");
	if (value)
	{
		if (*value == "keep-alive")
			return (HTTP_CONNECTION_KEEP_ALIVE);
		else if (*value == "close")
			return (HTTP_CONNECTION_CLOSE);
	}
	return (HTTP_CONNECTION_CLOSE);
}
