/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 18:54:45 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/25 11:51:16 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>

/* ****************** EXCEPTION DEFINITIONS ****************** */

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

/* *********************** UTILITIES ************************* */


static std::string get_line_cut(std::string &buffer)
{
	std::pair<size_t, short> nl_info = find_nl(buffer);
	if (nl_info.first == std::string::npos)
		return ("");
	std::string line = buffer.substr(0, nl_info.first);
	buffer.erase(0, nl_info.first + nl_info.second);
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

/* ********************** CONSTRUCTORS *********************** */

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
	  _is_ready(false), _command_set(src._command_set),
	  _header_set(src._header_set), _buffer(src._buffer)
{
}

HTTPRequest::~HTTPRequest(void)
{
}

/* ********************** OPERATORS ************************* */

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

/* ************************* GETTERS ************************* */

HTTPURI const &HTTPRequest::getURI(void) const
{
	return (_uri);
}

bool HTTPRequest::isReady(void) const
{
	return (_is_ready);
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

/* ************************* SETTERS ************************* */

void HTTPRequest::setURI(HTTPURI const &uri)
{
	_uri = uri;
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

bool HTTPRequest::isChunked(void) const
{
	const std::string *value = _header.getValue("Transfer-Encoding");
	if (value)
	{
		if (value->find("chunked") != std::string::npos)
			return (true);
	}
	return (false);
}

std::string const *HTTPRequest::getContentType(void) const
{
	const std::string *value = _header.getValue("Content-Type");

	if (value)
		return (value);
	else
		return (NULL);
}

/* ************************* METHODS ************************* */

// parse command if we have a complete request

void HTTPRequest::_parseCommand(void)
{
	if (find_nl(_buffer).first == 0)
		return ;
	std::vector<std::string> tokens = parse_request_command(_buffer);
	if (tokens.size() == 3)
	{
		_method = tokens[0];
		_uri = HTTPURI(tokens[1]);
		_version = tokens[2];
		_command_set = true;
	}
	else
		throw HTTPRequestException("Command malformed");
}

// parse header if we have a complete request

void HTTPRequest::_parseHeader(void)
{
	size_t pos;

	while ((pos = find_nl(_buffer).first) != std::string::npos)
	{
		std::string line = get_line_cut(_buffer);

		if (line == "")
		{
			_header_set = true;
			if (!_header.getValue("Content-Length") && !this->isChunked())
				_is_ready = true;
			return;
		}
		else
		{
			std::pair<std::string, std::string> const
				*pair = _header.parseLine(line);

			if (pair && pair->first == "Host")
				_host = pair->second;
		}
	}
	if (!_header.isValid())
		throw HTTPRequestException("Header malformed");
}

// the chunked request look like this:
// <SIZE>\r\n <-- chunk size in HEX
// <chunk>\r\n <-- chunk (could also contain \r\n)
// 0\n <-- end of chunk

void HTTPRequest::_parseBodyChunked(void)
{
	static size_t size = 0;
	static bool has_size = false;

	while (1)
	{
		if (!has_size && find_nl(_buffer).first != std::string::npos)
		{
			// check charset : [0-9] [A-F] [a-f]
			std::string line = get_line_cut(_buffer);
			if (line.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
				throw HTTPRequestException("Chunked body malformed");
			size = std::strtoul(line.c_str(), NULL, 16);
			has_size = true;
		}
		else if (has_size &&
				 _buffer.size() - 2 >= size && find_nl(_buffer).first != std::string::npos)
		{
			if (size == 0)
				_is_ready = true;
			_body += _buffer.substr(0, size);
			_buffer.erase(0, size + 2);
			has_size = false;
			size = 0;
		}
		else
			return;
	}
}

// add bytes to body and exeding bytes to buffer

void HTTPRequest::_parseBody(void)
{
	const std::string *content_length = _header.getValue("Content-Length");

	if (this->isChunked())
		_parseBodyChunked();
	else if (content_length)
	{
		size_t content_length_value = std::strtoul(content_length->c_str(), NULL, 10);
		// if we have more read that the body size we bufferize
		if (_body.size() + _buffer.size() > content_length_value)
		{
			size_t size = content_length_value - _body.size();
			std::string tmp = _buffer.substr(0, size);
			_body += tmp;
			_buffer.erase(0, size);
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

// parse a chunk of data so we can have multiple read to form a complete request

void HTTPRequest::parseChunk(std::string const &chunk)
{
	_buffer += chunk;

	// parse headers if not already done
	if (!_command_set && find_nl(_buffer).first != std::string::npos)
		_parseCommand();
	if (_command_set && !_header_set && find_nl(_buffer).first != std::string::npos)
		_parseHeader();
	if (_header_set && !_is_ready)
		_parseBody();
	if (_is_ready)
	{
		if (_header.isValid() == false)
			throw HTTPRequestException("Invalid header");
		if (_host == "")
			throw HTTPRequestException("Host header not found");
	}
}

// when you clear the request, the buffer is parsed

void HTTPRequest::clear(void)
{
	HTTPGeneral::clear();
	_header.clear();
	_method = "";
	_version = "";
	_uri.clear();
	_is_ready = false;
	_command_set = false;
	_header_set = false;
	std::string tmp = _buffer;
	_buffer.clear();
	this->parseChunk(tmp);
}
