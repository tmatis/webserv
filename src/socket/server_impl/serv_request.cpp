/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:12:04 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/18 18:02:27 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
