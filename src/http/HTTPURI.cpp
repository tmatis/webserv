/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPURI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 18:41:58 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/13 12:04:26 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPURI.hpp"
#include <string>
#include <sstream>
#include <iostream>

/* ****************** EXCEPTION DEFINITIONS ****************** */

HTTPURI::HTTPURIException::HTTPURIException(const char *errinfo)
	: _info(errinfo)
{
}

HTTPURI::HTTPURIException::~HTTPURIException() throw()
{
}

const char *HTTPURI::HTTPURIException::what() const throw()
{
	return (_info.c_str());
}

/* ********************** CONSTRUCTORS *********************** */

HTTPURI::HTTPURI(std::string const &uri)
	: _scheme(), _host(), _port(80), _path(), _query(), _fragment()
{
	this->decodeURI(uri);
}

HTTPURI::HTTPURI(void)
	: _scheme(), _host(), _port(80), _path(), _query(), _fragment()
{
}

HTTPURI::HTTPURI(HTTPURI const &src)
	: _scheme(src._scheme), _host(src._host), _port(src._port),
	  _path(src._path), _query(src._query), _fragment(src._fragment)
{
}

HTTPURI::~HTTPURI(void)
{
}

/* ********************** OPERATORS ************************* */

HTTPURI &HTTPURI::operator=(HTTPURI const &rhs)
{
	if (this != &rhs)
	{
		this->_scheme = rhs._scheme;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_path = rhs._path;
		this->_query = rhs._query;
		this->_fragment = rhs._fragment;
	}
	return (*this);
}

/* ************************* GETTERS ************************* */

std::string const &HTTPURI::getScheme(void) const
{
	return (this->_scheme);
}

std::string const &HTTPURI::getHost(void) const
{
	return (this->_host);
}

short HTTPURI::getPort(void) const
{
	return (this->_port);
}

std::string const &HTTPURI::getPath(void) const
{
	return (this->_path);
}

std::map<std::string, std::string> const &HTTPURI::getQuery(void) const
{
	return (this->_query);
}

std::string const &HTTPURI::getFragment(void) const
{
	return (this->_fragment);
}

std::string const *HTTPURI::getQueryValue(std::string const &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_query.find(key);
	if (it != this->_query.end())
		return (&it->second);
	return (NULL);
}

/* ************************* METHODS ************************* */

void HTTPURI::_decodeHost(std::string &uri)
{
	size_t pos = uri.find("/");
	if (pos == std::string::npos)
	{
		pos = uri.find("?");
		if (pos == std::string::npos)
		{
			pos = uri.find("#");
			if (pos == std::string::npos)
				pos = uri.size();
		}
	}
	std::string host = uri.substr(0, pos);
	size_t pos2 = host.find(":");
	if (pos2 != std::string::npos)
	{
		this->_host = host.substr(0, pos2);
		std::string port = host.substr(pos2 + 1, host.size());
		std::stringstream ss(port);
		ss >> this->_port;
	}
	else
		this->_host = host;
	uri.erase(0, pos);
}

void HTTPURI::_decodeQuery(std::string &uri)
{
	std::string::size_type pos = uri.find('?');
	if (pos != std::string::npos)
	{
		// erase until the '?'
		uri.erase(0, pos + 1);

		while (pos != std::string::npos)
		{
			// get the key until the '='
			pos = uri.find('=');
			std::string key;
			if (pos != std::string::npos)
			{
				key = uri.substr(0, pos);
				uri.erase(0, pos + 1);
				// get the value until the '&'
				pos = uri.find('&');
				if (pos == std::string::npos)
				{
					pos = uri.find('#');
					if (pos == std::string::npos)
						pos = uri.size();
				}

				std::string value = uri.substr(0, pos);
				uri.erase(0, pos);
				this->_query[key] = value;
			}
			else
				return;
			pos = uri.find('&');
			if (pos != std::string::npos)
				uri.erase(0, pos + 1);
		}
	}
}

void HTTPURI::decodeURI(std::string uri)
{
	// find scheme *://
	size_t pos = uri.find("://");
	if (pos != std::string::npos)
	{
		this->_scheme = uri.substr(0, pos);
		uri.erase(0, pos + 3);
	}
	
	// find host ://*:*#|?
	this->_decodeHost(uri);

	// find path */*?
	pos = uri.find("?");
	if (pos == std::string::npos)
	{
		pos = uri.find("#");
		if (pos == std::string::npos)
			pos = uri.size();
	}
	this->_path = uri.substr(0, pos);
	if (this->_path.size() == 0)
		this->_path = "/";
	uri.erase(0, pos);

	// find query ?*
	this->_decodeQuery(uri);

	// find fragment #*
	pos = uri.find("#");
	if (pos != std::string::npos)
		this->_fragment = uri.substr(pos + 1, uri.size());
}

void HTTPURI::clear(void)
{
	this->_scheme.clear();
	this->_host.clear();
	this->_port = 80;
	this->_path.clear();
	this->_query.clear();
	this->_fragment.clear();
}
