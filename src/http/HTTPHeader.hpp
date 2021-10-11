/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:15:26 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/11 20:31:48 by tmatis           ###   ########.fr       */
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
	std::vector<std::pair<std::string, std::string > > _headers;
	bool _is_valid;

public:
	
/* ********************** CONSTRUCTORS *********************** */

	HTTPHeader(void);
	HTTPHeader(HTTPHeader const &src);
	~HTTPHeader(void);
	
/* ********************** OPERATORS ************************* */
	
	HTTPHeader &operator=(HTTPHeader const &rhs);

/* ************************* GETTERS ************************* */

	std::string const *getValue(std::string key) const;
	bool isValid(void) const;

/* ************************* METHODS ************************* */

	void parseLine(std::string line);
	void addValue(std::string key, std::string const &value);
	std::string toString(void) const;
	void clear(void);

};

#endif
