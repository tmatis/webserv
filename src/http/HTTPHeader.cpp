/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:30:52 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 20:56:50 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPHeader.hpp"
#include <sstream>

HTTPHeader::HTTPHeader(void)
	: _headers()
{
}

HTTPHeader::HTTPHeader(HTTPHeader const &src)
	: _headers(src._headers)
{
}

HTTPHeader &HTTPHeader::operator=(HTTPHeader const &rhs)
{
	if (this != &rhs)
		this->_headers = rhs._headers;
	return (*this);
}

HTTPHeader::~HTTPHeader(void)
{
}

void HTTPHeader::addValue(std::string key, std::vector<std::string> const &value)
{
	for (std::vector<std::pair<std::string, std::vector<std::string> > >::iterator
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

/* 
* need to be at the format:
* title: value, value2; comment, value3
* ; - > comment
* , -> value separator
*/

void HTTPHeader::parseLine(std::string line)
{
	std::vector<std::string> tokens;

	if (!line.empty() && *(line.end() - 1) == '\n')
		line.resize(line.size() - 1);
	if (!line.empty() && *(line.end() - 1) == '\r')
		line.resize(line.size() - 1);
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		return;
	std::string title = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	if (value.empty())
		return;
	//remove spaces at the begining and end of the value
	while (value.size() > 0 && (value[0] == ' '))
		value.erase(0, 1);
	while (value.size() > 0 && (value[value.size() - 1] == ' '))
		value.erase(value.size() - 1, 1);
	// split value by ,
	std::stringstream ss(value);
	std::string token;
	while (std::getline(ss, token, ','))
	{
		while (token.size() > 0 && (token[0] == ' '))
			token.erase(0, 1);
		while (token.size() > 0 && (token[token.size() - 1] == ' '))
			token.erase(token.size() - 1, 1);
		tokens.push_back(token);
	}
	this->addValue(title, tokens);
}

std::vector<std::string> const *HTTPHeader::getValue(std::string key) const
{
	for (std::vector<std::pair<std::string, std::vector<std::string> > >::const_iterator
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
	for (std::vector<std::pair<std::string, std::vector<std::string> > >::const_iterator
			 it = _headers.begin();
		 it != _headers.end(); ++it)
	{
		result += it->first + ": ";
		for (std::vector<std::string>::const_iterator it2 = it->second.begin();
			 it2 != it->second.end(); ++it2)
		{
			result += *it2;
			if (it2 != it->second.end() - 1)
				result += ",";
		}
		result += "\r\n";
	}
	return (result);
}

void HTTPHeader::clear(void)
{
	_headers.clear();
}
