/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:37 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/18 03:19:42 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

int
Server::_check_cgi_extension(const Route& rules, const std::string& uri_path)
{
	if (rules.cgi_extension.length() == 0)
		return (false); // no cgi

	// look for cgi_extension in uri path
	size_t	pos = uri_path.rfind(rules.cgi_extension);
	if (pos == std::string::npos)
		return (false);
	else if (pos == uri_path.length() - rules.cgi_extension.length())
		return (true); // cgi_extension found at the end of path
	return (false); // not found or not where it should be
}
