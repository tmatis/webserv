/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:21:45 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/17 18:30:24 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*** F_POLLFD *****************************************************************/

f_pollfd::f_pollfd(const std::string& filename, int fd, int event)
{
	name		= filename;
	pfd.fd		= fd;
	pfd.events	= event; // fd is read only
}

f_pollfd::operator pollfd() const
{
	return (pfd);
}

/*** CLIENT *******************************************************************/

Client::Client(void) :
	write_trials(0), _state(PENDING_REQUEST), _file(NULL), _route(NULL) {}

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

/*** GETTERS ******************************************************************/

client_state
Client::state(void) const
{
	return (_state);
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

const f_pollfd*
Client::file(void) const
{
	return (_file);
}

const Route*
Client::rules(void) const
{
	return (_route);
}

/*** SETTERS ******************************************************************/

void
Client::state(client_state st)
{
	_state = st;
}

void
Client::file(const f_pollfd* f_pfd)
{
	_file = f_pfd;
}

void
Client::rules(const Route* rules)
{
	_route = rules;
}

void
Client::clear(void)
{
	write_trials	= 0;
	_state			= PENDING_REQUEST;
	_response.clear();
}
