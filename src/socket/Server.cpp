/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 22:40:27 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define BUFFER_SIZE	1024

const std::string	Server::_all_methods[3] = 
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

/*** CONNECTIONS **************************************************************/

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

/*** HTTP MESSAGES ************************************************************/

int
Server::handle_request(Client& client)
{
	try
	{
		// read request
		bool ready = _read_request(client);
		if (!ready)
			return (OK); // request is not complete
	}
	catch(const std::exception& e)
	{
		return (BAD_REQUEST);
	}
	
	// find correct route
	HTTPRequest&	req		= client.request();
	const Route&	route	= _resolve_routes(req.getURI());

	// check request compliance to route rules
	int code = _check_request_validity(route, req);
	if (code != OK)
		return (code);

#if 0
	if (_check_cgi_extension(route, req.getURI()))
	{
		// handle cgi
	}
#endif

	std::string	response_body;
	int			ret;

	// search content requested by the client
	ret = _find_resource(route, req.getURI(), response_body);
	if (ret != OK)
		return (ret); // error
	else if (response_body.length()) // autoindex HTML page has been generated
	{
		// prepare response
		client.state(WAITING_ANSWER);
	}
	else // resource found and opened, waiting next poll to read it
		client.file(&_files.back());
	return (OK);
}

void
Server::send_response(Client& client)
{
	std::string	response = client.response().toString();

	if (write(client.fd(), response.data(), response.size()) < 0)
		client.state(DISCONNECTED);
	else
		client.state(PENDING_REQUEST);
	client.request().clear();
	client.response().clear();
}

/*** GETTERS ******************************************************************/

std::vector<Client>&
Server::get_clients(void)
{
	return (_clients);
}

const std::vector<f_pollfd>&
Server::get_files(void) const
{
	return (_files);
}

const Listener&
Server::get_listener(void) const
{
	return (_host);
}

/*** PRIVATE FUNCTIONS ********************************************************/

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
	// check HTTP protocol version
	if (request.getVersion() != "HTTP/1.1")
		return (HTTP_VERSION_NOT_SUPPORTED);

	// check host header field
	if (!request.getHost().length())
		return (BAD_REQUEST);

	// check that the specified method is implemented by our server
	bool method = false;
	for (int i = 0; i < 3; i++)
	{
		if (request.getMethod() == _all_methods[i])
		{
			method = true;
			break ;
		}
	}
	if (!method)
		return (NOT_IMPLEMENTED);

	// check that the specified method is allowed by the route rules
	method = false;
	for (std::vector<std::string>::const_iterator it = rules.methods.begin();
			it != rules.methods.end();
			it++)
	{
		if (*it == request.getMethod())
		{
			method = true;
			break ;
		}
	}
	if (!method)
		return (METHOD_NOT_ALLOWED);

	// a body limit of 0 means unlimited
	if (_config.body_limit && request.getBodySize() > _config.body_limit)
		return (PAYLOAD_TOO_LARGE);

	// POST requests MUST have a Content-Length header
	if (request.getMethod() == "POST" && !request.getHeader().getValue("Content-Length"))
		return (LENGTH_REQUIRED);
	return (OK);
}

#if 0
int
Server::_check_cgi_extension(const Route& rules, const URI& uri)
{
	// look for cgi_extension in uri path
	size_t	pos = uri.getPath().rfind(rules.cgi_extension);

	if (pos == uri.getPath().length() - rules.cgi_extension.length())
		return (true); // cgi_extension found at the end of path
	return (false); // not found or not where it should be
}
#endif

int
Server::_find_resource(const Route& rules, std::string path, std::string& body)
{
	// build path from root dir and uri path
	if (rules.root.back() == '/') // avoid getting "root/dir//uri/path"
		path = rules.root.substr(0, rules.root.length() - 1) + path;
	else
		path = rules.root + path;

	struct stat	pathinfo;
	if (stat(path.data(), &pathinfo) == -1)
	{
		if (errno == ENOENT)
			return (NOT_FOUND); // path doesn't exist
		else
			return (INTERNAL_SERVER_ERROR); // other error
	}
	
	// path is a directory
	if (pathinfo.st_mode & S_IFDIR)
	{
		DIR*						dirptr;
		struct dirent*				file;
		std::vector<struct dirent>	dirls;

		// open directory
		if (!(dirptr = opendir(path.data())))
			return (INTERNAL_SERVER_ERROR);

		errno = 0;
		while (file = readdir(dirptr)) // read directory entries
		{
			if (file->d_type == DT_REG && file->d_name == path)
				break ; // index file found !
			dirls.push_back(*file);
		}
		closedir(dirptr);
		if (errno) // readdir failed
			return (INTERNAL_SERVER_ERROR);

		if (!file) // index file not found...
		{
			if (rules.autoindex) // autoindex enabled
			{
				// generate autoindex HTML
				return (OK);
			}
			return (NOT_FOUND);
		}
		else
			path += file->d_name; // append index file to dir path
	}

	// add file to server's list
	int fd = open(path.data(), O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return (INTERNAL_SERVER_ERROR);
	_files.push_back(f_pollfd(path, fd));
	return (OK);
}
