/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:46 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/17 23:36:24 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "webserv: Bad argument\n";
		std::cerr << "usage: ./webserv path/to/config\n";
		return (EXIT_FAILURE);
	}

	// create configurations for server
	MasterConfig			mconfig;

	mconfig.construct(argv[1]);

	// create servers
	std::vector<Server*>	hosts;

	try
	{
		for (size_t i = 0; i < mconfig._configs.size(); i++)
			hosts.push_back(new Server(mconfig._configs[i]));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		destroy_servers(hosts);
		return (EXIT_FAILURE);
	}
	
	// create poll class
	PollClass			pc(POLL_TIMEOUT);

	for (size_t i = 0 ; i < hosts.size() ; i++)
	{
		// add server to poll class
		pc.add_server(*hosts[i]);
		
		// print server listener info
		std::cout	<< "Listen on "
					<< inet_ntoa(hosts[i]->get_listener().addr().sin_addr)
					<< ":"
					<< ntohs(hosts[i]->get_listener().addr().sin_port)
					<< "\n";		
	}

	while (true)
	{
		if (pc.polling() == -1)
			perror("webserv: poll: ");

		// check events for each server
		for (std::vector<Server*>::iterator h = hosts.begin();
				h != hosts.end();
				++h)
		{
			// check events for each client
			for (Server::client_iterator cl = (*h)->get_clients().begin();
					cl != (*h)->get_clients().end();
					++cl)
			{
				if (handle_events(pc, *h, *cl) == -1)
					perror("webserv: client event: ");
			}

			// check connection on server
			if (handle_events(pc, *h) == -1)
				perror("webserv: client connection: ");

			(*h)->flush_clients();	// delete disconnected clients
			(*h)->flush_files();	// delete unused files
		}
	}


	return (0);	
}

int handle_events(PollClass& pc, Server *host)
{
	int	fd		= host->get_listener().fd();
	int	revent	= pc.get_raw_revents(fd);

	if (revent & POLLIN)
		return (host->add_new_client());
	return (0);
}

int handle_events(PollClass& pc, Server *host, Client& client)
{
	typedef int (*event_handlers)(Server*, Client&);
	
	static event_handlers const handlers[] = {
		event_pollin,
		event_pollout,
		event_pollhup,
		event_pollerr,
		event_pollnval,
		NULL
	};
	
	 static short const events[] = {
		POLLIN,
		POLLOUT,
		POLLHUP,
		POLLERR,
		POLLNVAL
	};

	int	fd		= client.fd();
	int	revent	= pc.get_raw_revents(fd);

	for (int i = 0; handlers[i]; i++)
	{
		if (revent & events[i])
			return (handlers[i](host, client));
	}

	if (client.state() == IDLE) // client has requested a file
	{
		revent	= pc.get_raw_revents(client.file()->pfd.fd);
		if (revent & POLLIN) // file is ready for reading
			host->create_file_response(client);
		else if (revent & POLLOUT)
			host->write_uploaded_file(client);
	}

	return (0);
}

void destroy_servers(std::vector<Server*>& list)
{
	for (std::vector<Server*>::iterator it = list.begin();
			it != list.end();
			++it)
		delete *it;
}
