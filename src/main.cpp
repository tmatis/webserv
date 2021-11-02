/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:46 by mamartin          #+#    #+#             */
/*   Updated: 2021/11/02 15:06:44 by mamartin         ###   ########.fr       */
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

	// create servers & poll class
	std::vector<Server*>	hosts;
	PollClass				pc;
	bool				server_injection = true;
	size_t				si = 0;

	try {
		for (size_t i = 0; i < mconfig._configs.size(); i++)
		{
			// check if a server already exists on the same host:port
			std::vector<Server*>::iterator it;
			for (it = hosts.begin(); it != hosts.end(); ++it)
			{
				if ((*it)->get_listener().addr().sin_addr.s_addr == mconfig._configs[i].address_res)
				{
					if ((*it)->get_listener().addr().sin_port == mconfig._configs[i].port)
						break ;
				}
			}

			if (it == hosts.end()) // create a new host
				hosts.push_back(new Server(mconfig._configs[i]));
			else // add new virtual server configuration to the host
				(*it)->add_virtual_server(mconfig._configs[i]);
		}
		pc = PollClass(mconfig._timeout);
		PollClass::pc_ptr = &pc;
	} catch(const std::exception& e) {
		std::cerr << "poll & server creation > " << e.what() << \
	" (fatal error)" << std::endl; destroy_servers(hosts); return (1);
	}

	std::signal(SIGINT, &main_while_switch);
	std::signal(SIGTERM, &main_while_switch);
	while (main_while_handler)
	{
		try {
			if (pc.polling() == -1 && main_while_handler)
				perror("webserv: poll: ");
		} catch (std::exception &e) { std::cerr << "polling > " << e.what() << \
		" (fatal error)" << std::endl; destroy_servers(hosts); return (1); }
		
		// poll class server injection + server listener info
		if (server_injection)
		{
			pc.add_server(*hosts[si]);
			if (PollClass::get_pollclass()->get_raw_revents(1) == POLLOUT)
				std::cout	<< "Listen on "
							<< inet_ntoa(hosts[i]->get_listener().addr().sin_addr)
							<< ":"
							<< ntohs(hosts[i]->get_listener().addr().sin_port)
							<< "\n";
			si++;
			if (si >= hosts.size())
				server_injection = false;
			continue ;
		}

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
				int	ret = handle_events(pc, *h, *cl);
				if (ret == -1)
					print_error("webserv: client event");
				else if (ret == 0) // child process
				{
					destroy_servers(hosts);
					return (0);
				}
			}

			// check connection on server
			if (handle_events(pc, *h) == -1)
				print_error("webserv: client connection");

			(*h)->flush_clients();	// delete disconnected clients
			(*h)->flush_files();	// delete unused files
		}
	}

	// close sockets/files and free all memory allocated on the heap
	close_all_fds(hosts);
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
		event_pollhup,
		event_pollerr,
		event_pollnval,
		event_pollin,
		event_pollout,
		NULL
	};
	
	 static short const events[] = {
		POLLHUP,
		POLLERR,
		POLLNVAL,
		POLLIN,
		POLLOUT
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
					if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
						std::cerr << "cgi > error while running cgi process : \'" \
						<< e.what() << "\'" << std::endl;
					(*it).get_client().response().setStatus(500);
					(*it).set_state(2);
				}
				if ((*it).get_state() != 2)
					return (1);
				host->create_file_response((*it));
				host->get_cgis().erase(it);
				return (1);
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
	return (1);
}

void close_all_fds(std::vector<Server*>& list)
{
	for (std::vector<Server*>::iterator it = list.begin();
		it != list.end();
		++it)
			(*it)->close_fds();
}

void destroy_servers(std::vector<Server*>& list)
{
	for (std::vector<Server*>::iterator it = list.begin();
			it != list.end();
			++it)
		delete *it;
}

void print_error(const char* message)
{
	std::cerr << message << ": " << strerror(errno) << "\n";
}
