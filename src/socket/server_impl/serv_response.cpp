/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_response.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:12:06 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/19 23:20:38 by mamartin         ###   ########.fr       */
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
	_define_content_type(client, response);

	// Content-Length
	ss << response.getBodySize();
	headers.addValue("Content-Length", ss.str());		

	// Connection (nginx behavior for error 400)
	if (response.getStatus() == BAD_REQUEST)
		headers.addValue("Connection", "close");
	// Allow (only for error 405)
	else if (response.getStatus() == METHOD_NOT_ALLOWED)	
	{
		std::string	allow_header_val;

		for (std::set<std::string>::iterator it = client.rules()->methods.begin();
			it != client.rules()->methods.end();
			++it)
				allow_header_val += *it;
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
		std::string ref = _get_uri_reference(client.file()->name);
		if (ref.length()) // new file can be referenced as an uri
			headers.addValue("Location", ref); // add this uri reference to the response
	}
	
	response.setHeader(headers);
	response.setReady(true);
	client.rules(NULL);
	client.state(WAITING_ANSWER);
}

void
Server::_define_content_type(Client& client, HTTPResponse& response)
{
	const std::string&	filename		= client.file()->name;
	std::string			mime_type		= client.rules()->find_mime_type(filename);

	if (mime_type == client.rules()->_default_mime) // if getting mime type from filename failed
	{
		// try to detect it from file content itself
		mime_type = client.rules()->find_mime_type(response.getBody(), false);
	}
	response.getHeader().addValue("Content-Type", mime_type); 
}

#if 0

bool
Server::_check_useragent_accept_types(const std::string& content_type, const std::string* accepted)
{
	if (!accepted || *accepted == "*/*")
		return (true); // no need to check anything

	// isolate type and subtype from content-type
	size_t		slash_pos		= content_type.find('/');
	std::string	mime_type		= content_type.substr(0, slash_pos);
	std::string	mime_subtype	= content_type.substr(slash_pos + 1, std::string::npos);

	std::cout 	<< content_type << " "
				<< mime_type << " "
				<< mime_subtype << "\n";

	size_t 		start			= 0;	// index of the first character of a mime type
	size_t 		end				= 0; 	// index of the type-separator character
	
	while (end != std::string::npos)
	{
		end = accepted->find(',', start); // find separator character ','

		size_t		pos = accepted->find(';', start); // ';' indicates a weight
		std::string	tmp;

		if (pos == std::string::npos) // no weight
		{
			if (end == std::string::npos)
				tmp = accepted->substr(start, end); // copy all remaining string
			else 
				tmp = accepted->substr(start, end - start); // copy only mime type substring
		}
		else
			tmp = accepted->substr(start, pos - start); // do not copy the weight factor part

		// isolate type and subtype in mime type substring
		slash_pos				= tmp.find('/');
		std::string	tmp_type	= tmp.substr(0, slash_pos);
		std::string	tmp_subtype	= tmp.substr(slash_pos + 1, std::string::npos);

		if (mime_type == tmp_type) // compare type
		{
			if (mime_subtype == tmp_subtype || tmp_subtype == "*") // compare subtype
				return (true); // content-type is accepted by the user-agent
		}

		start = end + 1; // check next type
	}
	return (false); // content-type is not accepted by the user-agent 
}
#endif
