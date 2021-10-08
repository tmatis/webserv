/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPGeneral.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:12:37 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 11:20:34 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPGENERAL_HPP
#define HTTPGENERAL_HPP

# include "HTTPHeader.hpp"
# include <string>

class HTTPRequest
{
private:
	HTTPHeader _header;
	std::string _body;
	
public:
	HTTPRequest(void);
	HTTPRequest(HTTPRequest const &src);
	HTTPRequest &operator=(HTTPRequest const &src);
	~HTTPRequest(void);

	/*  Getters  */
	HTTPHeader &getHeader(void);
	std::string &getBody(void);
	
	/*  Setters  */
	void setHeader(HTTPHeader const &header);
	void setBody(std::string const &body);

	virtual std::string toString(void) const = 0;

};

#endif
