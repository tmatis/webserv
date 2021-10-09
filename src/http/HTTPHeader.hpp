/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:15:26 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/09 13:49:21 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHEADER_HPP
#define HTTPHEADER_HPP

#include <string>
#include <map>
#include <vector>

class HTTPHeader
{
private:
	std::vector<std::pair<std::string, std::vector<std::string> > > _headers;

public:
	class HTTPHeaderException : public std::exception
	{
	public:
		HTTPHeaderException(const char *errinfo);
		virtual ~HTTPHeaderException() throw();
		virtual const char *what(void) const throw();

	private:
		const std::string _info;
	};
	HTTPHeader(void);
	HTTPHeader(HTTPHeader const &src);
	HTTPHeader &operator=(HTTPHeader const &rhs);

	~HTTPHeader(void);

	void parseLine(std::string line);

	void addValue(std::string key, std::vector<std::string> const &value);
	std::vector<std::string> const *getValue(std::string key) const;

	std::string toString(void) const;

	void clear(void);
};

#endif
