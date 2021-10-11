/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPGeneral.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:12:37 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/11 20:30:26 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPGENERAL_HPP
#define HTTPGENERAL_HPP

# include "HTTPHeader.hpp"
# include <string>

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
	std::string const &getBody(void) const;
	size_t getBodySize(void) const;

/* ************************* SETTERS ************************* */
	void setHeader(HTTPHeader const &header);
	void setBody(std::string const &body);

/* ************************* METHODS ************************* */
	void clear(void);

};

#endif
