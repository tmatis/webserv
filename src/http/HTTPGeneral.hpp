/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPGeneral.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:12:37 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/24 11:08:07 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPGENERAL_HPP
#define HTTPGENERAL_HPP

# include "HTTPHeader.hpp"
# include <string>

enum HTTPConnectionType
{
	HTTP_CONNECTION_CLOSE = false,
	HTTP_CONNECTION_KEEP_ALIVE = true
};

/*
** this class is the base class for all the HTTP request and response
** all request have an HTTP header and a body
*/

class HTTPGeneral
{
protected:
	HTTPHeader _header;
	std::string _body;
	
public:

/* ********************** CONSTRUCTORS *********************** */

	HTTPGeneral(void);
	HTTPGeneral(HTTPGeneral const &src);
	virtual ~HTTPGeneral(void);

/* ********************** OPERATORS ************************* */

	HTTPGeneral &operator=(HTTPGeneral const &src);

/* ************************* GETTERS ************************* */

	HTTPHeader &getHeader(void);
	HTTPHeader const &getHeader(void) const;
	std::string const &getBody(void) const;
	size_t getBodySize(void) const;

/* ************************* SETTERS ************************* */

	void setHeader(HTTPHeader const &header);
	void setBody(std::string const &body);

/* ************************* METHODS ************************* */
	void clear(void);

/* ************************* STATIC METHODS ****************** */
	static std::string append_paths(const std::string& str1,
		const std::string& str2);

};

std::pair<size_t, short> find_nl(std::string const &buffer);

#endif
