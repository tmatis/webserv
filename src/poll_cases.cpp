/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll_cases.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 17:52:22 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/12 02:38:26 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int event_pollin(Server* host, Client& client)
{
	return (host->handle_request(client));
}

int event_pollout(Server* host, Client& client)
{
	host->send_response(client);
	return (0);
}

int event_pollhup(Server* host, Client& client)
{
	(void)host;
	client.state(DISCONNECTED);
	return (0);
}

int event_pollerr(Server* host, Client& client)
{
	return (event_pollhup(host, client));
}

int event_pollnval(Server* host, Client& client)
{
	return (event_pollhup(host, client));
}
