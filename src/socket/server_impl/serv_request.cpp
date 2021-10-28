/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_request.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:12:04 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/28 16:07:18 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"
#include <iterator>
#include "../../utils/random_access_iterator.hpp"

bool
Server::_read_request(Client &client)
{
	char	buffer[BUFFER_SIZE];
	ssize_t	readBytes	= read(client.fd(), &buffer, BUFFER_SIZE);

	if (readBytes < 0)
	{
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
			std::cerr << "server > reading client request failed: " << strerror(errno) << "\n";
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (false);
	}
	else if (readBytes == 0)
		client.state(DISCONNECTED);
	else
	{
		ft::random_access_iterator<char> iterator_begin(buffer);
		ft::random_access_iterator<char> iterator_end(buffer + readBytes);
		client.request().parseChunk(std::string(iterator_begin, iterator_end));
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
	if (!_check_http_version(request.getVersion()))
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

bool
Server::_is_mime_type_supported(const Route& rules, const std::string& mime_type)
{
	for (std::map<std::string, std::string>::const_iterator it = rules._mime_types.begin();
		it != rules._mime_types.end();
		++it)
	{
		if (it->second == mime_type)
			return (true);
	}
	return (false);
}

bool
Server::_check_http_version(const std::string& version)
{
	char	vnum = version[version.length() - 1];

	if ((vnum == '1' || vnum == '0') && version.length() == 8)
	{
		if (version.find("HTTP/1.") == 0)
				return (true);
	}
	return (false);
}
