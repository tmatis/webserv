/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:46 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/26 15:45:42 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool	main_while_handler = true;

void	main_while_switch(int signal)
{ (void)signal; std::cout << std::endl; main_while_handler = false; }

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

	try { mconfig.construct(argv[1]); }
	catch (std::exception &e) { std::cerr << "config > " << e.what() << \
	" (fatal error)" << std::endl; return (1); }

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
	PollClass			pc(mconfig._timeout);

	for (size_t i = 0 ; i < hosts.size() ; i++)
	{
		if (pc.polling() == -1 && main_while_handler)
			perror("webserv: poll: ");
		// add server to poll class
		pc.add_server(*hosts[i]);
		
		// print server listener info
		if (PollClass::get_pollclass()->get_raw_revents(1) == POLLOUT)
			std::cout	<< "Listen on "
						<< inet_ntoa(hosts[i]->get_listener().addr().sin_addr)
						<< ":"
						<< ntohs(hosts[i]->get_listener().addr().sin_port)
						<< "\n";		
	}

	std::signal(SIGINT, &main_while_switch);
	std::signal(SIGTERM, &main_while_switch);
	while (main_while_handler)
	{
		if (pc.polling() == -1 && main_while_handler)
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
	destroy_servers(hosts);
	if (PollClass::get_pollclass()->get_raw_revents(1) == POLLOUT)
		std::cout << "webserv closed, bye bye o/" << std::endl;
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

		std::vector<CGI>::iterator it = host->get_cgis().begin();
		for ( ; it != host->get_cgis().end() ; ++it)
		{
			if (&((*it).get_client()) == &client)
			{
				try {
					if ((*it).get_state() == 0)
						(*it).send_request(pc.get_raw_revents((*it).get_input_pipe()));
					if ((*it).get_state() == 1)
						(*it).get_response(pc.get_raw_revents((*it).get_output_pipe()));
				} catch (std::exception &e) {
					if (PollClass::get_pollclass()->get_raw_revents(2) == POLLOUT)
						std::cerr << "cgi > error while running cgi process : \'" \
						<< e.what() << "\'" << std::endl;
					(*it).get_client().response().setStatus(500);
					(*it).set_state(2);
				}
				if ((*it).get_state() != 2)
					return (0);
				host->create_file_response((*it));
				host->get_cgis().erase(it);
				return (0);
			}
		}
		for (size_t i = 0; i < client.files().size(); i++)
		{
			revent = pc.get_raw_revents(client.files()[i]->pfd.fd);
			
			if (revent & POLLIN) // file is ready for reading
				host->create_file_response(client);
			else if (revent & POLLOUT)
				host->write_uploaded_file(client, i);
		}
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
