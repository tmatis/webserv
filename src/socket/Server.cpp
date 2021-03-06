/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/11/02 14:27:42 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../utils/random_access_iterator.hpp"

Server::Server(const Config& conf) :
	_host(Listener(conf.address_res, conf.port)),  _timeout(conf._timeout / 1000)
{
	_config.push_back(conf);
}

Server::~Server(void)
{
	// free files
	for (std::vector<f_pollfd*>::iterator it = _files.begin();
		it != _files.end();
		++it)
			delete *it;
}

void
Server::close_fds(void)
{
	// close all sockets
	close(_host.fd());
	for (client_iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->fd());

	// close files
	for (std::vector<f_pollfd*>::iterator it = _files.begin();
		it != _files.end();
		++it)
			close((*it)->pfd.fd);
}

/*** CONNECTIONS **************************************************************/

void
Server::add_virtual_server(const Config& vserv)
{
	_config.push_back(vserv);
}

int
Server::add_new_client(void)
{
	Client	client;

	if (client.connect(_host.fd()) == -1)
	{
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
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
			if (std::difftime(time(NULL), it->last_request) >= _timeout)
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
	const Config&	vserv	= _resolve_host(req.getHost());
	const Route&	route	= _resolve_routes(vserv, uri.getPath());
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
	// get response as a std::string
	str_response& response = client.string_response();
	if (response.written == 0) // first time trying to send the response
		response.data = client.response().toString();

	size_t	count	= response.data.length() - response.written; // remaining bytes to send
	ssize_t	bytes	= write(client.fd(), response.data.c_str() + response.written, count);

	if (bytes < 0) // write failed
	{
		client.write_trials++;
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
			std::cerr << "server > sending client response failed: " << strerror(errno) << "\n";
		if (client.write_trials == 5) // too much fails
		{
			if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
				std::cerr << "server > client disconnected.\n";
			client.state(DISCONNECTED);
		}
		return ;
	}

	response.written += bytes;
	if (response.written != response.data.length())
		return ; // will try to send the bytes left after next poll()

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
	ssize_t		bytes;

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
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
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
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
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

std::vector<CGI>&
Server::get_cgis(void)
{
	return (_cgis);
}
