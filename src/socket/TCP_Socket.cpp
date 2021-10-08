/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCP_Socket.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:32:57 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 15:55:08 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCP_Socket.hpp"

TCP_Socket::TCP_Socket(const int& fd, const sockaddr_in& address) :
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
TCP_Socket::set_non_blocking(void)
{
	// set non-blocking flag on file descriptor
	return (fcntl(_fd, F_SETFL, O_NONBLOCK));
}

const int&
TCP_Socket::get_fd(void) const
{
	return (_fd);
}

const sockaddr_in&
TCP_Socket::get_name(void) const
{
	return (_addr);
}
