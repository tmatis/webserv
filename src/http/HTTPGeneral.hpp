/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPGeneral.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:12:37 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 21:02:31 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPGENERAL_HPP
#define HTTPGENERAL_HPP

# include "HTTPHeader.hpp"
# include <string>

class HTTPGeneral
{
protected:
	HTTPHeader _header;
	std::string _body;
	
public:
	HTTPGeneral(void);
	HTTPGeneral(HTTPGeneral const &src);
	HTTPGeneral &operator=(HTTPGeneral const &src);
	virtual ~HTTPGeneral(void);

	/*  Getters  */
	HTTPHeader &getHeader(void);
	std::string const &getBody(void) const;
	
	/*  Setters  */
	void setHeader(HTTPHeader const &header);
	void setBody(std::string const &body);
	void clear(void);

};

#endif
