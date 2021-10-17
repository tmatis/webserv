/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 18:07:44 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/17 20:58:55 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <cerrno>
#include "Server.hpp"

Server::Server(const Config& conf) :
	_host(Listener(conf.address_res, conf.port)), _config(conf) {}

Server::~Server(void)
{
	// close all sockets
	close(_host.fd());
	for (client_iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->fd());

	// close files
	for (std::vector<f_pollfd>::iterator it = _files.begin();
		it != _files.end();
		++it)
			close(it->pfd.fd);
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
			++it;
	}
}

void
Server::flush_files(void)
{
	std::vector<f_pollfd>::iterator	f = _files.begin();
	client_iterator					cl;
	
	while (f != _files.end()) // check all files opened
	{
		cl = _clients.begin();
		while (cl != _clients.end())
		{
			if (cl->file() == &(*f))
				break ; // file is requested by a client
			++cl;
		}

		if (cl == _clients.end()) // no client request this file anymore
		{
			// delete file
			close(f->pfd.fd);
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

	if (_check_cgi_extension(route, uri.getPath()))
	{
		// _handle_cgi(uri, client);
	}

	if (_handle_upload(client, route) == true)
		return (OK); // file uploaded, waiting for write permission

	// search content requested by the client
	code = _find_resource(route, uri.getPath(), client);
	if (code != OK)
		return (_handle_error(client, code));
	else if (client.file()) // resource found
		client.state(IDLE);

	return (OK);
}

void
Server::send_response(Client& client)
{
	std::string	response = client.response().toString();

	client.request().clear();
	if (write(client.fd(), response.data(), response.size()) < 0)
	{
		client.write_trials++;
		std::cerr << "server > sending client response failed: " << strerror(errno) << "\n";
		if (client.write_trials == 5)
		{
			std::cerr << "server > client disconnected.\n";
			client.state(DISCONNECTED);
		}
		return ;
	}

	if (*client.response().getHeader().getValue("Connection") == "close")
	{
		client.state(DISCONNECTED);
		return ;
	}

	client.clear();
	if (client.request().isReady())
		handle_request(client); // new request
}

int
Server::create_file_response(Client& client)
{
	std::string	file_content;
	char		buffer[BUFFER_SIZE];
	int			bytes;

	while ((bytes = read(client.file()->pfd.fd, buffer, BUFFER_SIZE)) > 0)
	{
		buffer[bytes] = '\0';
		file_content += buffer; // load file content
	}
	if (bytes == -1)
	{
		std::cerr << "server > reading file requested failed: " << strerror(errno) << "\n";
		client.file(NULL);
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (-1);
	}
	_create_response(client, &file_content);
	client.file(NULL);
	return (0);
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

	if (readBytes < 0)
	{
		std::cerr << "server > reading client request failed: " << strerror(errno) << "\n";
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (false);
	}
	else if (readBytes == 0)
		client.state(DISCONNECTED);
	else
	{
		buffer[readBytes] = '\0';
		std::cout << buffer << "\n";
		client.request().parseChunk(std::string(buffer));
	}
	return (client.request().isReady());
}

const Route&
Server::_resolve_routes(const std::string& uri_path)
{
	const Route* matching = NULL;

	for (std::vector<Route>::const_iterator it = _config.routes.begin();
			it != _config.routes.end();
			++it)
	{
		if (uri_path.find(it->location) == 0) // prefix found in uri
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
	for (std::set<std::string>::iterator it = rules._methods_supported.begin();
		it != rules._methods_supported.end();
		++it)
	{
		if (*it == request.getMethod())
		{
			method = true;
			break ;
		}
	}
	if (!method)
		return (NOT_IMPLEMENTED);

	// check that the specified method is allowed by the route rules
	method = false;
	for (std::set<std::string>::const_iterator it = rules.methods.begin();
			it != rules.methods.end();
			++it)
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

int
Server::_check_cgi_extension(const Route& rules, const std::string& uri_path)
{
	if (rules.cgi_extension.length() == 0)
		return (false); // no cgi

	// look for cgi_extension in uri path
	size_t	pos = uri_path.rfind(rules.cgi_extension);
	if (pos == std::string::npos)
		return (false);
	else if (pos == uri_path.length() - rules.cgi_extension.length())
		return (true); // cgi_extension found at the end of path
	return (false); // not found or not where it should be
}

int
Server::_find_resource(const Route& rules, std::string path, Client& client)
{
	// build path from root dir and uri path
	path.erase(0, rules.location.length());		// location/path/to/file -> /path/to/file
	path = _append_paths(rules._root, path);	// root/path/to/file

	struct stat	pathinfo;
	if (stat(path.data(), &pathinfo) == -1)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			return (NOT_FOUND); // path doesn't exist
		else
		{
			std::cerr << "server > stat() failed: " << strerror(errno) << "\n";
			return (INTERNAL_SERVER_ERROR); // other error
		}
	}
	
	// path is a directory
	if (pathinfo.st_mode & S_IFDIR)
	{
		DIR*						dirptr;
		struct dirent*				file;
		std::vector<struct dirent>	dirls;

		// open directory
		if (!(dirptr = opendir(path.data())))
		{
			std::cerr << "server > cannot open directory \"" << path << "\": " << strerror(errno) << "\n";
			return (INTERNAL_SERVER_ERROR);
		}

		errno = 0;
		while ((file = readdir(dirptr))) // read directory entries
		{
			if (_is_index_file(rules, file))
				break ; // index file found !
			dirls.push_back(*file);
		}
		closedir(dirptr);
		if (errno) // readdir failed
		{
			std::cerr << "server > cannot read directory \"" << path << "\": " << strerror(errno) << "\n";
			return (INTERNAL_SERVER_ERROR);
		}

		if (!file) // index file not found...
		{
			if (rules._autoindex) // autoindex enabled
			{
				client.response().gen_autoindex(dirls, path);
				_create_response(client);
				return (OK);
			}
			return (NOT_FOUND);
		}
		else
			path = _append_paths(path, file->d_name); // path + filename
	}
	
	if (_file_already_requested(client, path))
		return (OK); // another client asked for the same file

	// open file
	int fd = open(path.data(), O_RDONLY | O_NONBLOCK);
	if (fd == -1)
	{
		std::cerr << "server > cannot open file \"" << path << "\": " << strerror(errno) << "\n";
		return (INTERNAL_SERVER_ERROR);
	}

	_files.push_back(f_pollfd(path, fd));
	client.file(&_files.back());
	return (OK);
}

bool
Server::_is_index_file(const Route& rules, struct dirent* file)
{
	for (std::set<std::string>::const_iterator it = rules._index_paths.begin();
			it != rules._index_paths.end();
			++it)
	{
		if (file->d_type == DT_REG) // regular file
		{
			if (*it == file->d_name)
				return (true); // file is an index file
		}
	}
	return (false); // not an index file
}

int
Server::_handle_error(Client& client, int status, bool autogen)
{
	if (!autogen) // search error page in server configuration
	{
		std::map<int, std::string>::const_iterator	errpage;
	
		errpage = _config._error_pages.find(status);
		if (errpage != _config._error_pages.end()) // page exists
		{
			client.response().setStatus((status_code)status);
			_create_response(client, &errpage->second);
			return (0);
		}
		else // doesn't exist
			return (_handle_error(client, status, true)); // retry with auto-generation enabled
	}
	else // generate page
	{
		client.response().gen_error_page(status);
		_create_response(client);
	}
	return (0);
}

bool
Server::_file_already_requested(Client& client, std::string const &filepath)
{
	for (size_t i = 0; i < _files.size(); i++)
	{
		if (_files[i].name == filepath) // file already in server's list
		{
			// assign file to the client
			client.file(&_files[i]);
			return (true);
		}
	}
	return (false);
}

std::string
Server::_append_paths(const std::string& str1, const std::string& str2)
{
	size_t		last_index = str1.length() - 1;
	std::string	ret;

	if (str1[last_index] == '/' || str2[0] == '/')
		ret = str1 + str2;
	else
		ret = str1 + "/" + str2;
	return (ret);
}

void
Server::_create_response(Client& client, const std::string *body)
{
	/*
		no need to set response status here because
			it's either 200 (default value)
			or already set by Server::_handle_error()

		Body is either sent by create_file_response() -> body pointer
			or generated by Server::_handle_error() and Server::_find_resource()

		If you want more headers I can add them
			retry-after and last-modified are some examples
		Content-Type is always text/html
			it will be changed when configuration will includes
			a map of MIME types
		Connection is keep-alive by default
	*/ 

	HTTPResponse&		response = client.response();
	HTTPHeader			headers;
	std::stringstream	ss;
	
	if (body)
		response.setBody(*body);
	ss << response.getBodySize();

	// set headers
	response.setContentType("text/html");					// default Content-Type
	headers.addValue("Content-Length", ss.str());			// Content-Length
	if (response.getStatus() == BAD_REQUEST)
		headers.addValue("Connection", "close");			// Connection (nginx behavior)
	else if (response.getStatus() == METHOD_NOT_ALLOWED)	// Allow (only for 405 errors)
	{
		std::string	allow_header_val;

		for (std::set<std::string>::iterator it = client.rules()->methods.begin();
			it != client.rules()->methods.end();
			++it)
				allow_header_val += *it;
		headers.addValue("Allow", allow_header_val);
	}
	else if (response.getStatus() / 100 == 3) // status indicates a redirection
	{
		const std::string&	new_url = client.rules()->redirection.second;

		// add a Location header with the new url
		// url may contain variables like $host for exemple
		// we need to replace them by their actual values
		headers.addValue("Location", _replace_conf_vars(client, new_url));
	}
	response.setHeader(headers);

	response.setReady(true);
	client.rules(NULL);
	client.state(WAITING_ANSWER);
}

bool
Server::_handle_redirection(Client& client, const Route& rules)
{
	if (!rules.redirection.first)
		return (false); // no redirection configured

	client.response().setStatus((status_code)rules.redirection.first);
	_create_response(client);
	return (true);
}

std::string
Server::_replace_conf_vars(Client& client, const std::string& redirection)
{
	std::string	url = redirection;
	std::string	port;
	size_t		pos;

 	// replace "$host" placeholder by real hostname
	pos = url.find("$host");
	if (pos != std::string::npos)
	{
		std::string	host		= client.request().getHost();
		size_t		port_pos	= host.rfind(":");

		// remove port segment if any
		if (port_pos != std::string::npos) 
			host.erase(port_pos, host.length() - port_pos);
		url.replace(pos, 5, host);
	}

 	// replace "$server_port"
	pos = url.find("$server_port");
	if (pos != std::string::npos)
	{
		std::stringstream	ss;

		ss << ntohs(_config.port);
		url.replace(pos, 12, ss.str());
	}

 	// replace "$uri"
	pos = url.find("$uri");
	if (pos != std::string::npos)
		url.replace(pos, 4, client.request().getURI().getPath());
	return (url);
}

bool
Server::_handle_upload(Client& client, const Route& rules)
{
	if (client.request().getMethod() != "POST")
		return (false); // uploads use POST method only

	(void)rules;

	return (false);
}
