/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 14:27:06 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/11 19:29:43 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "HTTPGeneral.hpp"

enum HTTPConnectionType {
	HTTP_CONNECTION_CLOSE = false,
	HTTP_CONNECTION_KEEP_ALIVE = true
};


class HTTPRequest : public HTTPGeneral
{
private:
	std::string _method;
	std::string _version;
	std::string _uri;
	bool _is_ready;
	bool _command_set;
	bool _header_set;
	std::string _buffer;

	/* headers info */
	std::string _host;


public:
	class HTTPRequestException : public std::exception
	{
	public:
		HTTPRequestException(const char *errinfo);
		virtual ~HTTPRequestException() throw();
		virtual const char *what(void) const throw();
	private:
			const std::string	_info;
	};
	HTTPRequest(void);
	HTTPRequest(const HTTPRequest &src);
	HTTPRequest &operator=(const HTTPRequest &src);
	~HTTPRequest(void);

	void setMethod(std::string const &method);
	std::string const &getMethod(void) const;

	void setVersion(std::string const &version);
	std::string const &getVersion(void) const;

	void setURI(std::string const &uri);
	std::string const &getURI(void) const;

	bool isReady(void) const;

	void parseChunk(std::string const &chunk);

	void clear(void);

	std::string const &getHost(void) const;
	std::string const *getUserAgent(void) const;
	std::vector<std::string> getAccept(void) const;
	HTTPConnectionType getConnection(void) const;
};

#endif
