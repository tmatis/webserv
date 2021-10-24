/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/24 13:52:50 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../utils/random_access_iterator.hpp"

const int Server::timeout = REQUEST_TIMEOUT;

Server::Server(const Config& conf) :
	_host(Listener(conf.address_res, conf.port)), _config(conf) {}

Server::~Server(void)
{
	// close all sockets
	close(_host.fd());
	for (client_iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->fd());

	// close files
	for (std::vector<f_pollfd*>::iterator it = _files.begin();
		it != _files.end();
		++it)
	{
		close((*it)->pfd.fd);
		delete *it;
	}
}

/*** CONNECTIONS **************************************************************/

int
Server::add_new_client(void)
{
	Client	client;

	if (client.connect(_host.fd()) == -1)
	{
		std::cerr << "server > client connection failed: " << strerror(errno) << "\n";
		return (-1);
	}
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
		{
			if (std::difftime(time(NULL), it->last_request) >= Server::timeout)
				_handle_error(*it, REQUEST_TIMEOUT);
			++it;
		}
	}
}

void
Server::flush_files(void)
{
	if (!_files.size())
		return ;

	std::vector<f_pollfd*>::iterator	f = _files.begin();
	client_iterator						cl;
	
	while (f != _files.end()) // check all files opened
	{
		bool found	= false;
		cl		= _clients.begin();
		
		while (cl != _clients.end() && !found)
		{
			// check files requested by the client
			for (size_t i = 0; i < cl->files().size(); i++)
			{
				if (cl->files()[i] == *f)
				{
					found = true;
					break;
				}
			}
			++cl;
		}

		if (!found && !(*f)->cgi_file) // no client request this file anymore
		{
			// delete file
			close((*f)->pfd.fd);
			delete *f;
			f = _files.erase(f);
		}
		else
			++f;
	}
}

/*** HTTP MESSAGES ************************************************************/

int
Server::handle_request(Client& client)
{
	try
	{
		// read request
		if (!client.request().isReady())
		{
			bool ready = _read_request(client);
			if (!ready)
				return (OK); // request is not complete
		}
	}
	catch(const std::exception& e)
	{
		return (_handle_error(client, BAD_REQUEST));
	}
	
	client.last_request = time(NULL); // reset client timeout

	// find correct route
	HTTPRequest&	req		= client.request();
	const HTTPURI&	uri		= req.getURI();
	const Route&	route	= _resolve_routes(uri.getPath());
	client.rules(&route);

	// check request compliance to route rules
	int code = _check_request_validity(route, req);
	if (code != OK)
		return (_handle_error(client, code));

	if (_handle_redirection(client, route) == true)
		return (OK); // redirection response has been created, ready to send

	// call function associated to the request method
	char method_identifier = req.getMethod()[0];
	if (method_identifier == 'G') // GET
		return (_handle_get(client, route, uri));
	else if (method_identifier == 'P') // POST
		return (_handle_post(client, route, uri));
	else // DELETE
		return (_handle_delete(client, route, uri));
}

void
Server::send_response(Client& client)
{
	std::string	response = client.response().toString();

	if (write(client.fd(), response.data(), response.size()) < 0)
	{
		client.write_trials++;
		std::cerr << "server > sending client response failed: " << strerror(errno) << "\n";
		if (client.write_trials == 5) // too much fails
		{
			std::cerr << "server > client disconnected.\n";
			client.state(DISCONNECTED);
		}
		return ;
	}

	if (client.response().getConnection() == HTTP_CONNECTION_CLOSE)
	{
		client.state(DISCONNECTED);
		return ;
	}

	try
	{
		client.clear(); // clear request, response, state...
	}
	catch (const std::exception& e)
	{
		_handle_error(client, BAD_REQUEST);
		return ;
	}

	if (client.request().isReady())
		handle_request(client); // new request
}

int
Server::create_file_response(Client& client)
{
	f_pollfd*	file;
	char		buffer[BUFFER_SIZE];
	int			bytes;

	file	= client.files().front();
	bytes	= read(file->pfd.fd, buffer, BUFFER_SIZE);
	if (bytes > 0)
	{
		ft::random_access_iterator<char> iterator_begin(buffer);
		ft::random_access_iterator<char> iterator_end(buffer + bytes);
		file->data += std::string(iterator_begin, iterator_end); // load file content
	}
	else if (bytes == 0)
	{
		client.response().setBody(file->data);
		_create_response(client);		
	}
	else
	{
		std::cerr << "server > reading file requested failed: " << strerror(errno) << "\n";
		client.files().clear();
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (-1);
	}
	return (0);
}

int
Server::create_file_response(CGI& cgi)
{
	// cgi.get_client().response().parseCGI(cgi.get_response());
	// MISSING OTHER STATUS CODE FOR ERROR OR REDIRECTIONS
	if (cgi.get_client().response().getStatus() == 500)
		return (_handle_error(cgi.get_client(), 500));
	_create_response(cgi.get_client());
	return (0);
}

int
Server::write_uploaded_file(Client& client, int index)
{
	f_pollfd*	fpfd = client.files()[index]; 

	if (write(fpfd->pfd.fd, fpfd->data.c_str(), fpfd->data.length()) < 0)
	{
		_clear_previous_files(client); // delete all files created
		std::cerr << "server > cannot write uploaded file: " << strerror(errno) << "\n";
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (-1);
	}
	
	fpfd->pfd.events		= 0;
	client.files_number()	-= 1;
	
	if (client.files_number() == 0)
	{
		client.response().setStatus(CREATED); // upload successful
		_create_response(client);
	}
	return (0);
}

/*** GETTERS ******************************************************************/

std::vector<Client>&
Server::get_clients(void)
{
	return (_clients);
}

const std::vector<f_pollfd *>&
Server::get_files(void) const
{
	return (_files);
}

std::vector<f_pollfd *>&
Server::get_files(void)
{
	return (_files);
}

const Listener&
Server::get_listener(void) const
{
	return (_host);
}

const Config&
Server::get_config(void) const
{
	return (_config);
}

std::vector<CGI>&
Server::get_cgis(void)
{
	return (_cgis);
}