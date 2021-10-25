/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_redirection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:59 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/25 12:06:00 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"
#include <sstream>

bool
Server::_handle_redirection(Client& client, const Route& rules)
{
	if (!rules.redirection.first)
		return (false); // no redirection configured

	client.response().setStatus((status_code)rules.redirection.first);
	_create_response(client);
	return (true);
}

std::string
Server::_replace_conf_vars(Client& client, const std::string& redirection)
{
	std::string	url = redirection;
	size_t		pos;

 	// replace "$host" placeholder by real hostname
	pos = url.find("$host");
	if (pos != std::string::npos)
	{
		std::string	host		= client.request().getHost();
		size_t		port_pos	= host.rfind(":");

		// remove port segment if any
		if (port_pos != std::string::npos) 
			host.erase(port_pos, host.length() - port_pos);
		url.replace(pos, 5, host);
	}

 	// replace "$server_port"
	pos = url.find("$server_port");
	if (pos != std::string::npos)
	{
		std::stringstream	ss;

		ss << ntohs(_config.port);
		url.replace(pos, 12, ss.str());
	}

 	// replace "$uri"
	pos = url.find("$uri");
	if (pos != std::string::npos)
		url.replace(pos, 4, client.request().getURI().getPath());
	return (url);
}
