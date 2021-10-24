/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 20:01:22 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/24 00:24:17 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HTTPGeneral.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <set>

/* *********************** UTILITIES ************************* */

enum status_code
{
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	TEMPORARY_REDIRECT = 307,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

class HTTPResponse : public HTTPGeneral
{
private:
	static const short _status_code[];
	static const std::string _status_message[];
	short _status;
	bool		_is_ready;
	bool 		_header_parsed;
	std::string _cgi_res_buffer;
	size_t		_content_length_cgi;
	bool		_content_length_cgi_set;
	void _applyCGI(HTTPHeader &header);

public:
/* ********************** CONSTRUCTORS *********************** */

	HTTPResponse(void);
	HTTPResponse(HTTPResponse const &src);
	~HTTPResponse(void);

/* ********************** OPERATORS ************************* */

	HTTPResponse &operator=(HTTPResponse const &src);

/* *********************** UTILITIES ************************* */
	static std::string const &status_code_to_string(short code);
	
/* ************************* GETTERS ************************* */
	
	short getStatus(void) const;
	bool isReady(void) const;
	HTTPConnectionType getConnection(void) const;

/* ************************* SETTERS ************************* */

	void setStatus(short code);
	void setReady(bool b);
	void setContentType(std::string const &type);
	void setConnection(HTTPConnectionType type);
	void setAllow(std::string const &allow);
	void setLocation(std::string const &location);

/* ************************* METHODS ************************* */

	void clear(void);

	bool parseCGI(const std::string &res);

	std::string toString(void);

	HTTPResponse &gen_error_page(int const &status);
	HTTPResponse &gen_autoindex(std::vector<struct dirent> const &files, \
		std::string const &dir, std::string const &uri_path);
	HTTPResponse &gen_upload_response(const std::string& uri_path, \
		const std::vector<std::string>& files = std::vector<std::string>());
};

#endif
