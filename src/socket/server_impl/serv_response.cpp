/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:12:06 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/18 20:43:19 by tmatis           ###   ########.fr       */
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

void
Server::_create_response(Client& client, const std::string *body)
{
	HTTPResponse&		response = client.response();
	HTTPHeader			headers;
	std::stringstream	ss;
	
	if (body)
		response.setBody(*body);
	ss << response.getBodySize();

	// set headers
	response.setContentType("text/html");					// default Content-Type
	headers.setValue("Content-Length", ss.str());			// Content-Length
	if (response.getStatus() == BAD_REQUEST)
		headers.setValue("Connection", "close");			// Connection (nginx behavior)
	else if (response.getStatus() == METHOD_NOT_ALLOWED)	// Allow (only for 405 errors)
	{
		std::string	allow_header_val;

		for (std::set<std::string>::iterator it = client.rules()->methods.begin();
			it != client.rules()->methods.end();
			++it)
				allow_header_val += *it;
		headers.setValue("Allow", allow_header_val);
	}
	else if (response.getStatus() / 100 == 3) // status indicates a redirection
	{
		const std::string&	new_url = client.rules()->redirection.second;

		// add a Location header with the new url
		// url may contain variables like $host for exemple
		// we need to replace them by their actual values
		headers.setValue("Location", _replace_conf_vars(client, new_url));
	}
	else if (response.getStatus() == CREATED)
	{
		std::string ref = _get_uri_reference(client.file()->name);
		if (ref.length()) // new file can be referenced as an uri
			headers.setValue("Location", ref); // add this uri reference to the response
	}
	response.setHeader(headers);

	response.setReady(true);
	client.rules(NULL);
	client.state(WAITING_ANSWER);
}
