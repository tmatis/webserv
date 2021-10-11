/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/11 16:01:27 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(Config* conf, int n) :
	_host(Listener(conf[0].address.c_str(), conf[0].port)) // create listener socket
{
	for (int i = 0; i < n; i++)
		_configs.push_back(conf[i]); // add all configs relative to the same address:port
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
		{
			close(it->fd());
			it = _clients.erase(it);
		}
		else
			it++;
	}
}

const std::vector<Client>&
Server::get_clients(void) const
{
	return (this->_clients);
}

const std::vector<pollfd>&
Server::get_files(void) const
{
	return (this->_files);
}

const Listener&
Server::get_listener(void) const
{
	return (this->_host);
}
