/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 19:57:13 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/09 13:29:26 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

#define PORT 8080

int setup_welcome_socket(uint16_t port)
{
	int welcome_socket;
	struct sockaddr_in server_address;

	welcome_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (welcome_socket < 0)
		return (-1);
	int	enable = 1;
	setsockopt(welcome_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));

	if (bind(welcome_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
		return (-1);
	if (listen(welcome_socket, SOMAXCONN) < 0)
		return (-1);
	return (welcome_socket);
}

int handle_registered_client(std::vector<struct pollfd> &pollfd,
		std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_data,
		std::vector<struct pollfd>::iterator it)
{
	typedef int (*event_handlers)(std::vector<struct pollfd> &,
		std::vector<std::pair<HTTPRequest, HTTPResponse> > &,
		std::vector<struct pollfd>::iterator);
	static event_handlers handlers[] = {
		event_pollin,
		event_pollout,
		event_pollhup,
		event_pollerr,
		event_pollnval,
		NULL
	};
	
	static short events[] = {
		POLLIN,
		POLLOUT,
		POLLHUP,
		POLLERR,
		POLLNVAL
	};

	for (int i = 0; handlers[i]; i++)
	{
		if (it->revents & events[i])
			return (handlers[i](pollfd, client_data, it));
	}
	return (0);
}

int register_new_client(int welcome_socket,
	std::vector<struct pollfd> &pollfd,
	std::vector<std::pair<HTTPRequest, HTTPResponse> > &client_data)
{
	struct sockaddr_in client_address;
	socklen_t client_address_size = sizeof(client_address);

	int client_socket = accept(welcome_socket,
							   (struct sockaddr *)&client_address, &client_address_size);
	if (client_socket < 0)
		return (-1);
	else
	{
		std::cout << "new client: " << inet_ntoa(client_address.sin_addr)
		<< " (" << pollfd.size() << ")" << std::endl;
		// we add the new client to the list of pollfd
		struct pollfd poll_client_socket;
		poll_client_socket.fd = client_socket;
		poll_client_socket.events = POLLIN;
		pollfd.push_back(poll_client_socket);
		client_data.push_back(std::make_pair(HTTPRequest(), HTTPResponse()));
	}
	return (0);
}

int serve_clients(int welcome_socket)
{
	std::vector<struct pollfd> pollfd;
	std::vector<std::pair<HTTPRequest, HTTPResponse> > client_data;

	// we add welcome socket to the list of pollfd
	struct pollfd poll_welcome_socket;
	poll_welcome_socket.fd = welcome_socket;
	poll_welcome_socket.events = POLLIN;
	pollfd.push_back(poll_welcome_socket);

	while (true)
	{
		int poll_result = poll(&pollfd.front(), pollfd.size(), 2000);
		if (poll_result < 0)
			return (-1);
		if (poll_result == 0)
			continue;
		if (pollfd.front().revents & POLLIN)
		{
			// we have a new client
			if (register_new_client(welcome_socket, pollfd, client_data) < 0)
				std::cerr << "register_new_client failed: "
					<< strerror(errno) << std::endl;
			continue ;
		}
		for (std::vector<struct pollfd>::iterator it =
				 (pollfd.begin() + 1);
			 it != pollfd.end(); it++)
		{
			if (handle_registered_client(pollfd, client_data, it))
				break;
		}
	}
	return (0);
}

int main(void)
{
	int welcome_socket = setup_welcome_socket(PORT);
	if (welcome_socket < 0)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "Server is running on port " << PORT << std::endl;
	if (serve_clients(welcome_socket) < 0)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
