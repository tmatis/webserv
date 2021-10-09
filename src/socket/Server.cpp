/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/09 03:02:54 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Config* conf, int n) :
	_host(Listener(conf[0].address, conf[0].port)) // create listener socket
{
	for (int i = 0; i < n; i++)
		_configs.push_back(conf[n]); // add all configs relative to the same address:port
}

Server::~Server(void)
{
	// close all sockets
	close(_host.fd());
	for (client_iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->fd());

	// close files ?
	// delete configs ?
}

int
Server::add_new_client(void)
{
	Client	client;

	if (client.connect(_host.fd()) == -1)
		return (-1);
	_clients.push_back(client);
	return (0);
}

void
Server::flush_clients(void)
{
	client_iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if (it->state() == DISCONNECTED)
			it = _clients.erase(it);
		else
			it++;
	}
}
