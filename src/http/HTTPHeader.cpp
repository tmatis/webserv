/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:30:52 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/11 19:07:39 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPHeader.hpp"
#include <sstream>

HTTPHeader::HTTPHeaderException::HTTPHeaderException(const char *errinfo)
	: _info(errinfo)
{
}

HTTPHeader::HTTPHeaderException::~HTTPHeaderException() throw()
{
}

const char *HTTPHeader::HTTPHeaderException::what() const throw()
{
	return (_info.c_str());
}

HTTPHeader::HTTPHeader(void)
	: _headers(), _is_valid(false)
{
}

HTTPHeader::HTTPHeader(HTTPHeader const &src)
	: _headers(src._headers), _is_valid(src._is_valid)
{
}

HTTPHeader &HTTPHeader::operator=(HTTPHeader const &rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_is_valid = rhs._is_valid;
	}
	return (*this);
}

HTTPHeader::~HTTPHeader(void)
{
}

void HTTPHeader::addValue(std::string key, std::string const &value)
{
	for (std::vector<std::pair<std::string, std::string > >::iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
	{
		if (it->first == key)
		{
			it->second = value;
			return;
		}
	}
	_headers.push_back(std::make_pair(key, value));
}

void HTTPHeader::parseLine(std::string line)
{
	std::string tokens;

	if (!line.empty() && *(line.end() - 1) == '\n')
		line.resize(line.size() - 1);
	if (!line.empty() && *(line.end() - 1) == '\r')
		line.resize(line.size() - 1);
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		_is_valid = false;
	std::string title = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	if (value.empty() || title.empty())
		_is_valid = false;
	this->addValue(title, value);
}

std::string const *HTTPHeader::getValue(std::string key) const
{
	for (std::vector<std::pair<std::string, std::string > >::const_iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
	{
		if (it->first == key)
			return (&it->second);
	}
	return (NULL);
}

std::string HTTPHeader::toString(void) const
{
	std::string result;
	for (std::vector<std::pair<std::string, std::string > >::const_iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
		result += it->first + ": " + it->second + "\r\n";
	return (result);
}

void HTTPHeader::clear(void)
{
	_headers.clear();
}

bool HTTPHeader::isValid(void) const
{
	return (_is_valid);
}
