/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:52:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/13 20:51:23 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

Listener::Listener(unsigned int address, unsigned short port)
{
	try
	{
		// create socket
		_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_fd == -1)
			throw CreationFailure("socket: ");

		// fill address structure for the socket
		_addr.sin_family		= AF_INET;
		_addr.sin_port			= port;
		_addr.sin_addr.s_addr	= address;
		memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

		// non-blocking fd
		if (TCP_Socket::set_non_blocking(*this) == -1)
			throw CreationFailure("fcntl: ");

		// reusable address
		int enable = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
			throw CreationFailure("setsockopt: ");

		// assign a name to the socket
		if (bind(_fd, (sockaddr *)&_addr, sizeof(sockaddr_in)) == -1)
			throw CreationFailure("bind: ");

		// marks socket as passive
		if (listen(_fd, SOMAXCONN) == -1)
			throw CreationFailure("listen: ");
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}
