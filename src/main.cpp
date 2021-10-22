/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:46 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/22 12:36:20 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string		get_var_formatted_str(std::string const &var_name)
{
	std::string		res = var_name;

	for (unsigned int i = 0 ; i < res.size() ; i++)
	{
		res[i] = toupper(res[i]);
		if (res[i] == '-')
			res[i] = '_';
	}
	return (res);
}

std::pair<char **, unsigned int>			construct(HTTPURI &uri)
{
	std::vector<std::pair<std::string, std::string > > headers;
	std::pair<std::string, std::string > y(".php", "/var/php/uhdzizdiy");
	headers.push_back(std::make_pair("Accept", "*/*"));
	headers.push_back(std::make_pair("Cookies", "UAYYGYbagzhdgdh"));
	headers.push_back(std::make_pair("Content-Stuff", "ta race"));
	char **res;

	std::map<std::string, std::string>	vars;
	std::string							tampon = uri.getPath();
	size_t								pos = tampon.find(y.first);
	char								script_path[700];
	unsigned int 						i = 0;
	std::map<std::string, std::string>::const_iterator \
	it = uri.getQuery().begin();
	std::vector<std::pair<std::string, std::string > >::const_iterator \
	itr = headers.begin();

	vars["REMOTE_HOST"] = std::string();
	vars["AUTH_TYPE"] = std::string();
	vars["REMOTE_USER"] = std::string();
	vars["REMOTE_IDENT"] = std::string(); // not supported since nothing asked it
	vars["QUERY_STRING"] = std::string();
	vars["CONTENT_TYPE"] = std::string();

	vars["CONTENT_LENGTH"] = "5656";
	vars["SERVER_SOFTWARE"] = "HTTP/666";
	vars["SERVER_NAME"] = "localhost";
	vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	vars["SERVER_PROTOCOL"] = "webserv/666";
	vars["SERVER_PORT"] = "8666";
	vars["REQUEST_METHOD"] = "GET";
	vars["REMOTE_ADDR"] = "0.0.0.0";
	while (pos != std::string::npos)
	{
		if (pos + y.first.size() == tampon.size())
		{
			vars["SCRIPT_NAME"] = tampon;
			vars["PATH_INFO"] = std::string();
			break ;
		}
		if (pos + y.first.size() < tampon.size() && \
		tampon[pos + y.first.size()] == '/')
		{
			std::cout << "x" << std::endl;
			vars["SCRIPT_NAME"] = tampon.substr(0, pos + y.first.size());
			tampon.erase(0, pos + y.first.size());
			vars["PATH_INFO"] = tampon;
			break ;
		}
		pos = tampon.find(y.first, pos + 1);
	}
	while (it != uri.getQuery().end())
	{
		if (!vars["QUERY_STRING"].empty())
			vars["QUERY_STRING"] += "&";
		vars["QUERY_STRING"] += (*it).first + std::string("=") + (*it).second;
		it++;
	}
	if (true) //this->_request.getHeader().getValue("Authorization") != NULL)
	{
		tampon = "Basic CONTENTLOGIN==";
		if (tampon.find("Basic") == 0)
		{
			vars["AUTH_TYPE"] = "Basic";
			if (tampon.size() > 5 && tampon[5] == ' ')
			{
				tampon.erase(0, 6);
				vars["REMOTE_USER"] = tampon;
			}
		}
	}
	if (true)
		vars["CONTENT_TYPE"] = "XXXXXXX";
	tampon = vars["SCRIPT_NAME"];
	// tampon.erase(0, this->_route.location.length());
	// tampon = this->_server._append_paths(this->_route._root, tampon);
	// std::cout << "x" << std::endl;
	memset(script_path, 0, 700);
	realpath("socket/serv_impl/serv_delete.cpp", script_path);
	vars["PATH_TRANSLATED"] = script_path;
	while (itr != headers.end())
	{
		if ((*itr).first != "Content-Type" && (*itr).first != "Authorization" && \
		(*itr).first != "Content-Length" && (*itr).first != "Host")
			vars[std::string("HTTP_")\
			.append(get_var_formatted_str((*itr).first))] = (*itr).second;
		itr++;
	}
	it = vars.begin();
	res = new char *[vars.size() + 1];
	while (it != vars.end())
	{
		res[i] = new char[(*it).first.size() + \
		(*it).second.size() + 2];
		tampon = (*it).first + std::string("=") + (*it).second;
		std::strcpy(res[i], tampon.c_str());
		res[i][(*it).first.size() + (*it).second.size() + 1] = 0;
		// this->_var_containers.push_back((*it).first + \
		// std::string("=") + (*it).second);
		i++;
		it++;
	}
	res[vars.size()] = NULL;
	return (std::make_pair(res, vars.size()));
}

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
		std::vector<CGI>::iterator it = host->get_cgis().begin();
		for ( ; it != host->get_cgis().end() ; it++)
		{
			if (&((*it).get_client()) == &client)
			{
				// std::cout << (*it).get_state() << std::endl;
				if ((*it).get_state() == 0)
					(*it).send_request(pc.get_raw_revents((*it).get_input_pipe()));
				if ((*it).get_state() == 1)
					(*it).get_response(pc.get_raw_revents((*it).get_output_pipe()));
				if ((*it).get_state() != 2)
					return (0);
				host->get_cgis().erase(it);
				break ;
			}
		}
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
