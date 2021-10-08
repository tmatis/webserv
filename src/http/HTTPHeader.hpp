/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 11:15:26 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 12:32:46 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHEADER_HPP
# define HTTPHEADER_HPP

#include <string>
#include <map>
#include <vector>

class HTTPHeader
{
private:
	std::vector<std::pair<std::string, std::vector<std::string> > > _headers;
	
public:
	HTTPHeader(void);
	HTTPHeader(HTTPHeader const &src);
	HTTPHeader &operator=(HTTPHeader const &rhs);

	~HTTPHeader(void);

	void parseLine(std::string line);
	std::vector<std::string> const *getHeaderValue(std::string key) const;
	std::string toString(void) const;
};


#endif
