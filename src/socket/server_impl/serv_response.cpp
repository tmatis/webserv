/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:12:06 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/23 21:12:42 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

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
			client.response().setBody(errpage->second);
			_create_response(client);
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

void
Server::_create_response(Client& client)
{
	HTTPResponse&		response	= client.response();
	HTTPHeader&			headers		= response.getHeader();
	std::stringstream	ss;

	// Content-Type
	if (client.files().size() && response.getStatus() != CREATED)
		_define_content_type(client, response);
	else
		response.getHeader().addValue("Content-Type", "text/html");

	// Connection (nginx behavior for error 400)
	if (response.getStatus() == BAD_REQUEST || response.getStatus() == REQUEST_TIMEOUT)
		headers.addValue("Connection", "close");
	// Allow (only for error 405)
	else if (response.getStatus() == METHOD_NOT_ALLOWED)	
	{
		std::set<std::string>::iterator	it = client.rules()->methods.begin();
		std::string						allow_header_val;
		
		while (it != client.rules()->methods.end())
		{
			allow_header_val += *it;
			if (++it != client.rules()->methods.end())
				allow_header_val += ", ";
		}
		headers.addValue("Allow", allow_header_val);
	}
	// Location in case of a redirection (30x status)
	else if (response.getStatus() / 100 == 3) // status indicates a redirection
	{
		const std::string&	new_url = client.rules()->redirection.second;

		// add a Location header with the new url
		// url may contain variables like $host for exemple
		// we need to replace them by their actual values
		headers.addValue("Location", _replace_conf_vars(client, new_url)); // Location
	}
	// Location in case of file uploads (201 status)
	else if (response.getStatus() == CREATED)
	{
		std::string ref = _get_uri_reference(client.files().front()->name);
		if (ref.length()) // new file can be referenced as an uri
		{
			std::vector<std::string> file_links;

			file_links.push_back(ref);
			for (std::vector<f_pollfd*>::iterator it = client.files().begin() + 1;
					it != client.files().end();
					++it)
						file_links.push_back(_get_uri_reference((*it)->name));

			response.gen_upload_response(client.request().getURI().getPath(), file_links);
			headers.addValue("Location", ref); // add this uri reference to the response
		}
		else
		{
			response.gen_upload_response(client.request().getURI().getPath());
			response.setStatus(OK);
		}
	}
	
	// Content-Length
	ss << response.getBodySize();
	headers.addValue("Content-Length", ss.str());

	response.setHeader(headers);
	response.setReady(true);
	client.rules(NULL);
	client.state(WAITING_ANSWER);
}

void
Server::_define_content_type(Client& client, HTTPResponse& response)
{
	const std::string&	filename		= client.files().front()->name;
	std::string			mime_type		= client.rules()->find_mime_type(filename);

	if (mime_type == client.rules()->_default_mime) // if getting mime type from filename failed
	{
		// try to detect it from file content itself
		mime_type = client.rules()->find_mime_type(response.getBody(), false);
	}
	response.getHeader().addValue("Content-Type", mime_type);
}

std::string
Server::_get_uri_reference(const std::string& filename)
{
	std::string	ref = filename;

	for (std::vector<Route>::const_iterator it = _config.routes.begin();
			it != _config.routes.end();
			++it)
	{
		if (filename.find(it->_root) == 0) // root path found in filename
		{
			// replace root by location in filename
			ref.erase(0, it->_root.length());
			ref = HTTPGeneral::append_paths(it->location, ref);
			return (ref); // return uri reference to filename
		}
	}
	return (""); // filename cannot be referenced as an uri
}
