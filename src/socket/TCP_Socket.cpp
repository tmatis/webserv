/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCP_Socket.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:57 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/09 11:32:00 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCP_Socket.hpp"

TCP_Socket::TCP_Socket(int fd, const sockaddr_in& address) :
	_fd(fd), _addr(address) {}

TCP_Socket::TCP_Socket(const TCP_Socket& rhs)
{
	*this = rhs;
}

TCP_Socket&
TCP_Socket::operator=(const TCP_Socket& rhs)
{
	_fd		= rhs._fd;
	_addr	= rhs._addr;
	return (*this);
}

int
TCP_Socket::fd(void) const
{
	return (_fd);
}

const sockaddr_in&
TCP_Socket::addr(void) const
{
	return (_addr);
}

int
TCP_Socket::set_non_blocking(TCP_Socket& sock)
{
	// set non-blocking flag on file descriptor
	return (fcntl(sock._fd, F_SETFL, O_NONBLOCK));
}

TCP_Socket::
CreationFailure::CreationFailure(const char* errinfo) :
	_info(errinfo)
{
	_info.append(strerror(errno));
}

TCP_Socket::
CreationFailure::~CreationFailure() throw() {}

const char*
TCP_Socket::
CreationFailure::what(void) const throw()
{
	return (_info.data());
}

