/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:21:45 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 23:20:10 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) :
	state(PENDING_REQUEST) {}

int
Client::connect(int host_fd)
{
	socklen_t	size = sizeof(_addr);

	// create connected socket
	_fd = accept(host_fd, (sockaddr*)&_addr, &size);
	if (_fd == -1)
		return (-1);
	return (0);
}

Client::operator pollfd() const
{
	pollfd	pfd;

	pfd.fd		= _fd;
	pfd.events	= 0;
	pfd.revents	= 0;
	return (pfd);
}
