/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:30:52 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/25 11:59:16 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPHeader.hpp"

/* ********************** CONSTRUCTORS *********************** */

HTTPHeader::HTTPHeader(void)
	: _headers(), _is_valid(true)
{
}

HTTPHeader::HTTPHeader(HTTPHeader const &src)
	: _headers(src._headers), _is_valid(src._is_valid)
{
}


HTTPHeader::~HTTPHeader(void)
{
}

/* ********************** OPERATORS ************************* */

HTTPHeader &HTTPHeader::operator=(HTTPHeader const &rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_is_valid = rhs._is_valid;
	}
	return (*this);
}

/* ************************* GETTERS ************************* */

std::string const *HTTPHeader::getValue(std::string const &key) const
{
	for (std::vector<std::pair<std::string, std::string> >::const_iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
	{
		if (it->first == key)
			return (&it->second);
	}
	return (NULL);
}

std::vector<std::pair<std::string, std::string > > const &HTTPHeader::getHeaders() const
{
	return (this->_headers);
}

bool HTTPHeader::isValid(void) const
{
	return (_is_valid);
}

/* ************************* METHODS ************************* */

// replace or add a header
void HTTPHeader::setValue(std::string const &key, std::string const &value)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator
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

void HTTPHeader::addValue(std::string const &key, std::string const &value)
{
	_headers.push_back(std::make_pair(key, value));
}

void HTTPHeader::delValue(std::string const &key)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
	{
		if (it->first == key)
		{
			_headers.erase(it);
			return;
		}
	}
}

// parse line at format: key: value and return key

std::pair<std::string, std::string > const *HTTPHeader::parseLine(std::string line)
{
	if (!line.empty() && *(line.end() - 1) == '\n')
		line.resize(line.size() - 1);
	if (!line.empty() && *(line.end() - 1) == '\r')
		line.resize(line.size() - 1);
	size_t pos = line.find(":");
	if (pos == std::string::npos)
	{
		_is_valid = false;
		return (NULL);
	}
	std::string title = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	while (value.size() && value[0] == ' ')
		value.erase(0, 1);
	while (value.size() && value[value.size() - 1] == ' ')
		value.erase(value.size() - 1, 1);
	if (value.empty() || title.empty())
		_is_valid = false;
	this->addValue(title, value);
	return (&_headers.back());
}

// transform header to string
std::string HTTPHeader::toString(void) const
{
	std::string result;
	for (std::vector<std::pair<std::string, std::string> >::const_iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
		result += it->first + ": " + it->second + "\r\n";
	return (result);
}

void HTTPHeader::clear(void)
{
	_headers.clear();
}
