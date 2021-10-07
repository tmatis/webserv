/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll_cases.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 17:52:22 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/07 18:14:32 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int event_pollin(std::vector<struct pollfd> &pollfd,
				 std::vector<struct pollfd>::iterator it)
{
	char buffer[1024];
	int read_bytes = read(it->fd, buffer, sizeof(buffer) - 1);
	if (read_bytes < 0)
	{
		std::cerr << "read failed " << strerror(errno) << " (client " << it - pollfd.begin()
				  << ")" << std::endl;
		close(it->fd);
		pollfd.erase(it);
		return (1);
	}
	else if (read_bytes == 0)
	{
		std::cout << "client " << it - pollfd.begin()
				  << " disconnected (EOF)" << std::endl;
		close(it->fd);
		pollfd.erase(it);
		return (1);
	}
	else
	{
		buffer[read_bytes] = '\0';
		std::cout << "client " << it - pollfd.begin()
				  << ": " << buffer;
	}
	return (0);
}

int event_pollhup(std::vector<struct pollfd> &pollfd,
				  std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLUP)" << std::endl;
	close(it->fd);
	pollfd.erase(it);
	return (1);
}

int event_pollerr(std::vector<struct pollfd> &pollfd,
				  std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLERR)" << std::endl;
	close(it->fd);
	pollfd.erase(it);
	return (1);
}

int event_pollnval(std::vector<struct pollfd> &pollfd,
				   std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLNVAL)" << std::endl;
	close(it->fd);
	pollfd.erase(it);
	return (1);
}
