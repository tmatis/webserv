/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 23:25:25 by mamartin         ###   ########.fr       */
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
	_close_socket(_host);
	std::for_each(_clients.begin(), _clients.end(), _close_socket);

	// close files ?
	// delete configs ?
}

int
Server::add_new_client(void)
{
	Client	client;

	if (client.connect(_host.get_fd()) == -1)
		return (-1);
	_clients.push_back(client);
	return (0);
}

void
Server::delete_client(client_iterator client)
{
	_close_socket(*client);
	_clients.erase(client);
}

void
Server::flush_clients(void)
{
	client_iterator it = _clients.begin();

	for (it; it != _clients.end(); it++)
	{
		if (it->state == DISCONNECTED)
			delete_client(it);
	}
}

void
Server::_close_socket(TCP_Socket& sock)
{
	close(sock.get_fd()); // close file descriptor
}
