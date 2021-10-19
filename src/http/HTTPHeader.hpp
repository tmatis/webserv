/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:15:26 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/19 15:09:38 by tmatis           ###   ########.fr       */
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

	std::string const *getValue(std::string const &key) const;
	bool isValid(void) const;

/* ************************* METHODS ************************* */

	std::pair<std::string, std::string > const *parseLine(std::string line);
	void setValue(std::string const &key, std::string const &value);
	void addValue(std::string const &key, std::string const &value);
	void delValue(std::string const &key);
	std::string toString(void) const;
	void clear(void);

};

#endif
