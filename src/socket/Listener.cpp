/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:52:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 16:18:29 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

Listener::Listener(const char* address, const int& port)
{
	// create socket
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw CreationFailure("socket: ");

	// fill address structure for the socket
	_addr.sin_family		= AF_INET;
	_addr.sin_port			= htons(port);
	_addr.sin_addr.s_addr	= inet_addr(address);
	memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

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

Listener::
CreationFailure::CreationFailure(const char* errinfo) :
	_info(errinfo)
{
	_info.append(strerror(errno));
}

const char*
Listener::
CreationFailure::what(void) const throw()
{
	return (_info.data());
}
