/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_post.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:54 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/20 18:30:16 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

int
Server::_handle_post(Client &client, const Route& rules, const HTTPURI& uri)
{
	if (_check_cgi_extension(rules, uri.getPath()))
	{
		// _handle_cgi(uri, client);
	}

	if (_handle_upload(client, rules) == true) // file has been created or an error occured
		return (OK); // client state == (IDLE || WAITING_ANSWER)
	
	// what if it's a post request but it's not cgi or upload ????
	_handle_error(client, METHOD_NOT_ALLOWED); // ???

	return (OK);
}

bool
Server::_handle_upload(Client& client, const Route& rules)
{
	if (!rules._uploadfiles)
		return (false);

	const std::string*	content_type	= client.request().getContentType();
	std::string			mime_type		= "application/octet-stream"; // default content-type if not provided
	std::string			boundary		= "";

	if (content_type)
	{
		if (content_type->find("multipart/form-data") == 0)
		{
			return (false);
		}
		else if (*content_type == "application/x-www-form-urlencoded")
			return (false); // this content-type cannot be used for file transfer
		else
			mime_type = *content_type;
	}

	// check if mime type is accepted for upload
	if (!_is_mime_type_supported(rules, mime_type))
	{
		_handle_error(client, UNSUPPORTED_MEDIA_TYPE);
		return (true);
	}

	std::string filename = client.request().getURI().getPath();

	// build file path
	filename.erase(0, rules.location.length());			   // remove location prefix
	filename = HTTPGeneral::append_paths(rules.upload_path, filename); // filename = upload_path + filename

	// create file
	f_pollfd*	newfile = _create_file(filename, client.request().getBody(), rules._upload_rights);
	if (!newfile) // an error occured when trying to open file (see error logs for more details)
	{
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return (true);
	}
	
	client.files().push_back(newfile);
	client.state(IDLE);
	return (true);
}

f_pollfd*
Server::_create_file(const std::string& filename, const std::string& data, uint mode)
{
	int	fd;

	// open file (read only, non blocking fd, create it if it doesn't already exist)
	fd = open(filename.data(), O_WRONLY | O_CREAT | O_NONBLOCK | O_APPEND, mode);
	if (fd == -1)
	{
		std::cerr << "server > cannot open file \"" << filename << "\": " << strerror(errno) << "\n";
		return (NULL);
	}

	_files.push_back(new f_pollfd(filename, fd, POLLOUT, data));
	return (_files.back());
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
