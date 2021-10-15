/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 14:27:06 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/15 18:39:51 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HTTPGeneral.hpp"
#include "HTTPURI.hpp"

class HTTPRequest : public HTTPGeneral
{
private:
	std::string _method;
	std::string _version;
	HTTPURI _uri;
	bool _is_ready;
	bool _command_set;
	bool _header_set;
	std::string _buffer;
	void _parseCommand(void);
	void _parseHeader(void);
	void _parseBody(void);
	void _parseBodyChunked(void);

	/* headers info */
	std::string _host;

public:
/* ****************** EXCEPTION DEFINITIONS ****************** */

	class HTTPRequestException : public std::exception
	{
	public:
		HTTPRequestException(const char *errinfo);
		virtual ~HTTPRequestException() throw();
		virtual const char *what(void) const throw();

	private:
		const std::string _info;
	};

/* ********************** CONSTRUCTORS *********************** */

	HTTPRequest(void);
	HTTPRequest(const HTTPRequest &src);
	~HTTPRequest(void);


/* ********************** OPERATORS ************************* */

	HTTPRequest &operator=(const HTTPRequest &src);



/* ************************* GETTERS ************************* */
	std::string const &getMethod(void) const;
	HTTPURI const &getURI(void) const;
	std::string const &getVersion(void) const;
	std::string const &getHost(void) const;
	std::string const *getUserAgent(void) const;
	std::vector<std::string> getAccept(void) const;
	HTTPConnectionType getConnection(void) const;
	bool isReady(void) const;
	bool isChunked(void) const;
	std::string const *getContentType(void) const;

/* ************************* SETTERS ************************* */

	void setMethod(std::string const &method);
	void setVersion(std::string const &version);
	void setURI(HTTPURI const &uri);

/* ************************* METHODS ************************* */

	void parseChunk(std::string const &chunk);
	void clear(void);

};

#endif
