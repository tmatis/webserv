/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPURI.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 18:21:09 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/14 02:03:37 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPURI_HPP
#define HTTPURI_HPP

#include <string>
#include <map>

class HTTPURI
{
private:
	std::string _scheme;
	std::string _host;
	short _port;
	std::string _path;
	std::map<std::string, std::string> _query;
	std::string _fragment;
	void _decodeHost(std::string &uri);
	void _decodeQuery(std::string &uri);

public:
	/* ****************** EXCEPTION DEFINITIONS ****************** */
	
	class HTTPURIException : public std::exception
	{
	public:
		HTTPURIException(const char *errinfo);
		virtual ~HTTPURIException() throw();
		virtual const char *what(void) const throw();

	private:
		const std::string _info;
	};


	/* ********************** CONSTRUCTORS *********************** */

	HTTPURI(std::string const &uri);
	HTTPURI(void);
	HTTPURI(HTTPURI const &src);
	~HTTPURI();

	/* ********************** OPERATORS ************************* */

	HTTPURI &operator=(HTTPURI const &src);

	/* ************************* GETTERS ************************* */

	std::string const &getScheme(void) const;
	std::string const &getHost(void) const;
	short getPort(void) const;
	std::string const &getPath(void) const;
	std::map<std::string, std::string> const &getQuery(void) const;
	std::string const &getFragment(void) const;

	std::string const *getQueryValue(std::string const &key) const;

	/* ************************** UTILS ************************** */

	static std::string percentDecode(std::string const &str);


	/* ************************* METHODS ************************* */

	void decodeURI(std::string uri);

	void clear(void);
};

#endif
