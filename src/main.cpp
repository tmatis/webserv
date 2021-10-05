/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 19:57:13 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/05 14:48:35 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

int main(void)
{
	std::string rep_header("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\n\r\n");
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd < 0)
	{
		std::cout << "Error: cannot create socket: " << strerror(errno) << std::endl;
		exit(1);
	}

	struct sockaddr_in address_server;

	address_server.sin_family = AF_INET;
	address_server.sin_addr.s_addr = inet_addr("0.0.0.0");
	address_server.sin_port = htons(8080);
	memset(address_server.sin_zero, 0, sizeof(address_server.sin_zero));
	
	if (bind(server_fd, (struct sockaddr *)&address_server, sizeof(address_server)) < 0)
	{
		std::cout << "Error: cannot bind name: " << strerror(errno) << std::endl;
		close(server_fd);
		exit(1);
	}

	if (listen(server_fd, SOMAXCONN) < 0)
	{
		std::cout << "Error: cannot listen: " << strerror(errno) << std::endl;
		close(server_fd);
		exit(1);
	}

	while (true)
	{
		struct sockaddr_in address_client;
		socklen_t address_client_len = sizeof(address_client);

		int client_fd = accept(server_fd, (struct sockaddr *)&address_client, &address_client_len);
		if (client_fd < 0)
		{
			std::cout << "Error: cannot accept: " << strerror(errno) << std::endl;
			exit(1);
		}
		
		char buffer[BUFSIZ];

		std::string	received_string;
		
		ssize_t readed = 0;
		
		while ((readed = read(client_fd, buffer, sizeof(buffer) - 1)) != 0)
		{
			if (readed < 0)
			{
				std::cout << "Error: cannot read: " << strerror(errno) << std::endl;
				close(client_fd);
				close(server_fd);
				exit(1);
			}
			std::cout << "received " << readed << " bytes" << std::endl;
			buffer[readed] = '\0';
			
			received_string += buffer;
			if (*received_string.rbegin() == 10) // check if end of header
				break ;
		}
		std::cout << "received string: `" << received_string << "`" << std::endl;
		
		std::string to_reply;

		to_reply = rep_header + "hello world" + "\r\n";
		write(client_fd, to_reply.c_str(), to_reply.size());
		close(client_fd);
	}
}
