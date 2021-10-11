/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 00:33:55 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define BUFFER_SIZE	1024

const std::string	_all_methods[3] = 
{
	"GET",
	"POST",
	"DELETE"
};

Server::Server(const Config& conf) :
	_host(Listener(conf.address.c_str(), conf.port)), _config(conf) {}

Server::~Server(void)
{
	// close all sockets
	close(_host.fd());
	for (client_iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->fd());

	// close files ?
	// delete configs ?
}

int
Server::add_new_client(void)
{
	Client	client;

	if (client.connect(_host.fd()) == -1)
		return (-1);
	_clients.push_back(client);
	return (0);
}

void
Server::flush_clients(void)
{
	client_iterator it = _clients.begin();

	while (it != _clients.end())
	{
		if (it->state() == DISCONNECTED)
		{
			close(it->fd());
			it = _clients.erase(it);
		}
		else
			it++;
	}
}

int
Server::handle_request(Client &client)
{
	try
	{
		// read request
		bool ready = _read_request(client);
		if (!ready)
			return (OK);
	}
	catch(const std::exception& e)
	{
		return (BAD_REQUEST);
	}
	
	HTTPRequest& req = client.request();
	
	// check host
	if (_resolve_host(req) != OK)
		return (BAD_REQUEST);

	// find route
	const Route& route = _resolve_routes(req.getURI());

	// check request compliance to route rules
	int code = _check_request_validity(route, req);
	if (code != OK)
		return (code);

	// identify ressource
		// if cgi
			// handle it
		// else find resource
			// build path from root dir
			// if directory
				// search for a default page
				// if no default and autoindex is on
					// generate a list of files
		// if not found
			// 404

	// generate response
		// based on cgi output
		// or resource specified
		// or auto-generated page
			// in case of error with no pages defined
			// in case of directory listing
}

void
Server::send_response(Client &client)
{
	std::string	response = client.response().toString();

	if (write(client.fd(), response.data(), response.size()) < 0)
		client.state(DISCONNECTED);
	else
		client.state(PENDING_REQUEST);
}

const std::vector<Client>&
Server::get_clients(void) const
{
	return (this->_clients);
}

const std::vector<pollfd>&
Server::get_files(void) const
{
	return (this->_files);
}

const Listener&
Server::get_listener(void) const
{
	return (this->_host);
}

/* ***************** */
/* PRIVATE FUNCTIONS */
/* ***************** */

bool
Server::_read_request(Client &client)
{
	char	buffer[BUFFER_SIZE];
	int		readBytes	= read(client.fd(), &buffer, BUFFER_SIZE);

	if (readBytes <= 0)
		client.state(DISCONNECTED);
	else
	{
		buffer[readBytes] = '\0';
		client.request().parseChunk(std::string(buffer));
	}
	return (client.request().isReady());
}

int
Server::_resolve_host(HTTPRequest& request)
{
	const std::string&	uri = request.getURI();
	std::string			host;
	size_t				pos;

	if (uri.find("http", 0) != std::string::npos) // absolute URI (section 19.6.1.1 rfc2616)
	{
		// hostname starts after "http://" (7 characters)
		for (pos = 7; uri[pos] != '\0'; pos++)
		{
			if (uri[pos] == ':' || uri[pos] == '/')
				break ; // end of hostname
		}
		// substract hostname from uri
		host = uri.substr(7, pos - 7);
	}
	else // get hostname from Host header field
		host = request.getHost().substr(0, host.find_first_of(':'));
	
	if (host == _config.address || host == _config.name)
		return (OK);
	return (BAD_REQUEST); // bad hostname
}

const Route&
Server::_resolve_routes(const std::string& uri)
{
	const Route* matching = NULL;

	for (std::vector<Route>::const_iterator it = _config.routes.begin();
			it != _config.routes.end();
			it++)
	{
		if (uri.find(it->location) == 0) // prefix found in uri
		{
			if (!matching)
				matching = &(*it); // first prefix matching
			else if (matching->location.length() < it->location.length())
				matching = &(*it); // longest prefix to match
		}
	}
	return (*matching);
}

int
Server::_check_request_validity(const Route& rules, HTTPRequest& request)
{
	// check that the specified method is implemented by our server
	bool method = false;
	for (int i = 0; i < 3; i++)
	{
		if (request.getMethod() == _all_methods[i])
			method = true; break;
	}
	if (!method)
		return (NOT_IMPLEMENTED);

	// check that the specified method is allowed by the server
	method = false;
	for (std::vector<std::string>::const_iterator it = rules.methods.begin();
			it != rules.methods.end();
			it++)
	{
		if (*it == request.getMethod())
			method = true; break ;
	}
	if (!method)
		return (METHOD_NOT_ALLOWED);

	// a body limit of 0 means unlimited
	if (_config.body_limit && request.getBody().length() > _config.body_limit)
		return (PAYLOAD_TOO_LARGE);

	// POST requests MUST have a Content-Length header
	if (request.getMethod() == "POST" && !request.getHeader().getValue("Content-Length"))
		return (LENGTH_REQUIRED);
	return (OK);
}
