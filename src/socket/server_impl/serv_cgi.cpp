/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:37 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/18 19:25:50 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

int
Server::_check_cgi_extension(const Route& rules, const std::string& uri_path)
{
	if (rules.cgi_extension.length() == 0)
		return (false); // no cgi

	// look for cgi_extension in uri path
	size_t	pos = uri_path.find(rules.cgi_extension);
	while (pos != std::string::npos)
	{
		if (pos + rules.cgi_extension.size() == uri_path.size() || \
		uri_path[pos + 1] == '/')
			return (true);
		pos = uri_path.find(rules.cgi_extension, pos + 1);
	}
	return (false); // not found or not where it should be
}
