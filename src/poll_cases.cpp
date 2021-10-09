/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll_cases.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/07 17:52:22 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/08 22:28:55 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <sstream>

void delete_client(std::vector<struct pollfd> &pollfd,
				std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				std::vector<struct pollfd>::iterator it)
{
	close(it->fd);
	pollfd.erase(it);
	client_datas.erase(client_datas.begin() + (it - pollfd.begin()));
}

int event_pollin(std::vector<struct pollfd> &pollfd,
				std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				std::vector<struct pollfd>::iterator it)
{
	char buffer[1024];
	int read_bytes = read(it->fd, buffer, sizeof(buffer) - 1);
	if (read_bytes < 0)
	{
		std::cerr << "read failed " << strerror(errno) << " (client " << it - pollfd.begin()
				  << ")" << std::endl;
		return (1);
	}
	else if (read_bytes == 0)
	{
		std::cout << "client " << it - pollfd.begin()
				  << " disconnected (EOF)" << std::endl;
		delete_client(pollfd, client_datas, it);
		return (1);
	}
	else
	{
		buffer[read_bytes] = '\0';
		std::cout << "client " << it - pollfd.begin()
				  << ": " << buffer;
		client_datas[it - pollfd.begin() - 1].first.parseChunk(buffer);
		if (client_datas[it - pollfd.begin() - 1].first.isReady())
			it->events = POLLOUT; // when we finished read we can write
	}
	return (0);
}

template <typename T>
std::string itoa(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

int event_pollout(std::vector<struct pollfd> &pollfd,
				  std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				  std::vector<struct pollfd>::iterator it)
{
    client_datas[it - pollfd.begin() - 1]
		.second.getHeader().addValue("Content-Type",
		std::vector<std::string>(1, "text/plain"));
	client_datas[it - pollfd.begin() - 1]
		.second.setBody("your are client: " 
			+ itoa(it - pollfd.begin()) + "\r\n");
	client_datas[it - pollfd.begin() - 1]
		.second.setStatus(OK);
	client_datas[it - pollfd.begin() - 1]
		.second.setReady(true);

	std::string response = client_datas[it - pollfd.begin() - 1]
									.second.toString();

	client_datas[it - pollfd.begin() - 1].first.clear();
	client_datas[it - pollfd.begin() - 1].second.clear();
	if (write(it->fd, response.c_str(), response.size()) < 0)
	{
		std::cerr << "write failed " << strerror(errno) << " (client "
			<< it - pollfd.begin() << ")" << std::endl;
		return (1);
	}
	it->events = POLLIN; // when we finished write we can read
	return (0);
}

int event_pollhup(std::vector<struct pollfd> &pollfd,
				std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLUP)" << std::endl;
	delete_client(pollfd, client_datas, it);
	return (1);
}

int event_pollerr(std::vector<struct pollfd> &pollfd,
				std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLERR)" << std::endl;
	delete_client(pollfd, client_datas, it);
	return (1);
}

int event_pollnval(std::vector<struct pollfd> &pollfd,
				std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_datas,
				std::vector<struct pollfd>::iterator it)
{
	std::cout << "client " << it - pollfd.begin()
			  << " disconnected (POLLNVAL)" << std::endl;
	delete_client(pollfd, client_datas, it);
	return (1);
}
