/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:21:45 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/11 18:19:02 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) :
	_state(PENDING_REQUEST) {}

int
Client::connect(int host_fd)
{
	socklen_t	size = sizeof(_addr);

	// create connected socket
	_fd = accept(host_fd, (sockaddr*)&_addr, &size);
	if (_fd == -1 || TCP_Socket::set_non_blocking(*this) == -1)
		return (-1);
	return (0);
}

Client::operator pollfd() const
{
	pollfd	pfd;

	pfd.fd		= _fd;
	pfd.events	= 0;
	if (this->_state == WAITING_ANSWER || this->_state == PENDING_REQUEST)
		pfd.events = this->_state == PENDING_REQUEST ? POLLIN : POLLOUT;
	pfd.revents	= 0;
	return (pfd);
}

client_state
Client::state(void) const
{
	return (_state);
}

void
Client::state(client_state st)
{
	_state = st;
}

HTTPRequest&
Client::request(void)
{
	return (_request);
}

HTTPResponse&
Client::response(void)
{
	return (_response);
}
