/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 19:57:13 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/09 13:22:43 by nouchata         ###   ########.fr       */
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
	// int welcome_socket = setup_welcome_socket(PORT);
	// if (welcome_socket < 0)
	// {
	// 	std::cerr << "Error: " << strerror(errno) << std::endl;
	// 	return (1);
	// }
	// std::cout << "Server is running on port " << PORT << std::endl;
	// if (serve_clients(welcome_socket) < 0)
	// {
	// 	std::cerr << "Error: " << strerror(errno) << std::endl;
	// 	return (1);
	// }
	Config		conf;
	Config 		conf2;
	PollClass	master_poll(10000);
	Client		cp;

	conf.address = "127.0.0.1";
	conf.port = 8080;
	conf2.address = "192.168.0.5";
	conf2.port = 8989;
	try {
		Server		new_serv(&conf, 1);
		Server		new_serv2(&conf2, 1);

		std::cout << "server launched" << std::endl;
		for (unsigned int i = 0 ; i < 5 ; i++)
		{
			cp._fd = i+10;
			cp._state = IDLE;
			new_serv._clients.push_back(cp);
		}
		for (unsigned int i = 112 ; i < 112 + 6 ; i++)
			new_serv._files.push_back(PollClass::make_pollfd(i));
		master_poll.add_server(new_serv);

		for (unsigned int i = 88 ; i < 5 + 88 ; i++)
		{
			cp._fd = i;
			cp._state = IDLE;
			new_serv2._clients.push_back(cp);
		}
		for (unsigned int i = 254 ; i < 254 + 6 ; i++)
			new_serv2._files.push_back(PollClass::make_pollfd(i));
		master_poll.add_server(new_serv2);

		new_serv._files.erase(new_serv._files.end() - 1);
		new_serv._clients.erase(new_serv._clients.end() - 1);
		cp._fd = 7979;
		new_serv._clients.push_back(cp);
		cp._fd = 6464;
		new_serv._clients.push_back(cp);
		new_serv2._clients.pop_back();
		new_serv2._clients.pop_back();
		new_serv2._clients.pop_back();
		new_serv._clients.erase(new_serv._clients.begin());
		master_poll.remove_server(&new_serv);

		// new_serv._files.erase(new_serv._files.begin());
		// new_serv._files.push_back(PollClass::make_pollfd(5656));

		master_poll.update_pfd();
		{
			std::vector<struct pollfd>::iterator it = master_poll._pfd_list.begin();
			while (it != master_poll._pfd_list.end())
			{
				std::cout << (*it).fd << " ";
				it++;
			}
			std::cout << std::endl;
		}
	} catch (std::exception &e) { std::cout << e.what() << std::endl; }

	return (0);
}
