/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_post.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:54 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/26 15:38:34 by nouchata         ###   ########.fr       */
=======
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:54 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/25 22:38:17 by mamartin         ###   ########.fr       */
>>>>>>> master
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"
#include "../CGI.hpp"
#include <sstream>

int
Server::_handle_post(Client &client, const Route& rules, const HTTPURI& uri)
{
	std::pair<std::string, std::string>		cgi;
	cgi = _check_cgi_extension(rules, uri.getPath());
	if (!cgi.first.empty())
	{
		this->_cgis.push_back(CGI((*this), client, rules, client.request(), cgi));
		this->_cgis.back().construct();
		try {
			this->_cgis.back().launch();
		} catch (std::exception &e) { this->_cgis.pop_back(); return(_handle_error(client, 500)); }
		client.state(IDLE);
		return (OK);
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

	if (content_type)
	{
		if (content_type->find("multipart/form-data") == 0)
		{
			// parse body to create one or multiple files
			_form_upload(client);
			return (true);
		}
		else if (*content_type == "application/x-www-form-urlencoded")
			return (false); // this content-type cannot be used for file transfer
		else
			mime_type = *content_type;
	}
	
	// create a file with raw data from body
	_raw_upload(client, rules, mime_type);
	return (true);
}

void
Server::_raw_upload(Client& client, const Route& rules, const std::string& mime)
{
	std::string	filename;
	f_pollfd*	newfile;

	// check if mime type is accepted for upload
	if (!_is_mime_type_supported(rules, mime))
	{
		_handle_error(client, UNSUPPORTED_MEDIA_TYPE);
		return ;
	}

	// build file path
	filename = client.request().getURI().getPath();
	filename.erase(0, rules.location.length());			   				// remove location prefix
	filename = HTTPGeneral::append_paths(rules.upload_path, filename);	// filename = upload_path + filename

	// create file
	newfile = _create_file(filename, client.request().getBody(), rules._upload_rights);
	if (!newfile) // an error occured when trying to open file (see error logs for more details)
	{
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return ;
	}

	client.add_file(newfile);
	client.state(IDLE);
}

void
Server::_form_upload(Client& client)
{
	// extract boundary string from Content-Type header
	std::string boundary		= _get_boundary(client.request().getHeader().getValue("Content-Type"));
	std::string boundary_end	= boundary + "--";

	size_t		start;	// current position in body
	size_t		end;	// index of next boundary

	std::string	body = client.request().getBody();
	std::string line;

	/* create an array with filenames inside upload dir */
	std::vector<std::string> files;
	if (_list_directory(files, client.rules()->upload_path) == -1)
	{
		_handle_error(client, INTERNAL_SERVER_ERROR);
		return ;
	}

	start = 0;
	while (line != boundary_end)
	{
		line =  _get_next_line(body, &start);
		if (line == boundary)
		{
			std::string	filename;
			std::string	type;
			std::string	data;
			f_pollfd*	fpfd;

			filename	= _get_file_info(body, type, &start);

			// check that the server supports this content-type
			if (!_is_mime_type_supported(*client.rules(), type))
			{
				_clear_previous_files(client);
				_handle_error(client, UNSUPPORTED_MEDIA_TYPE);
				return ;
			}

			end			= body.find(boundary, start);		// find where next boundary is
			data		= body.substr(start, end - start);	// copy data until next boundary
			start		= end;								// continue parsing from that position

			/* change filename if it already exists */
			_check_existing_file(files, filename);
			files.push_back(filename);

			// create file on local filesystem
			filename	= HTTPGeneral::append_paths(client.rules()->upload_path, filename);
			fpfd		= _create_file(filename, data, client.rules()->_upload_rights);
			if (!fpfd) // file creation failed
			{
				_clear_previous_files(client);
				_handle_error(client, INTERNAL_SERVER_ERROR);
				return ;
			}
			client.add_file(fpfd);
		}
	}
	client.state(IDLE);
}

std::string
Server::_get_boundary(const std::string* content_type)
{
	size_t		start		= content_type->find("boundary=") + 9;
	size_t		end			= content_type->find(';', start);
	std::string boundary;
	
	if (end == std::string::npos)
		boundary = content_type->substr(start, end);
	else
		boundary = content_type->substr(start, end - start);

	// boundaries start with two '-'
	boundary.insert(boundary.begin(), 2, '-');
	return (boundary);
}

std::string
Server::_get_file_info(const std::string& body, std::string& type, size_t *start)
{
	std::string line;
	std::string	filename;
	size_t		pos;
	
	line = _get_next_line(body, start);
	while (line.size() > 1)
	{
		if (line.find("Content-Disposition: ") == 0) // content disposition
		{
			// extract filename
			pos = line.find("filename");
			if (pos != std::string::npos)
			{
				filename = line.substr(pos + 10, std::string::npos);
				filename.erase(filename.length() - 1, 1); // erase '"' at the end of filename
			}
			else
				filename = "unknown";
		}
		else if (line.find("Content-Type: ") == 0) // content type
			type = line.substr(14, line.length() - 14);
		line = _get_next_line(body, start);
	}
	
	// set a default type if none
	if (!type.size())
		type = "application/octet-stream";

	return (filename);
}

std::string
Server::_get_next_line(const std::string& src, size_t* pos)
{
	size_t		end = src.find("\r\n", *pos);
	std::string	line;

	if (end == std::string::npos)
	{
		line = src.substr(*pos, end);
		*pos = end;
	}
	else
	{
		line = src.substr(*pos, end - *pos);
		*pos = end + 2;
	}
	return (line);
}

f_pollfd*
Server::_create_file(const std::string& filename, const std::string& data, uint mode)
{
	int	fd;

	// open file (read only, non blocking fd, create it if it doesn't already exist)
	fd = open(filename.data(), O_WRONLY | O_CREAT | O_NONBLOCK | O_APPEND, mode);
	if (fd == -1)
	{
		if (PollClass::get_pollclass()->get_raw_revents(2) == POLLOUT)
			std::cerr << "server > cannot open file \"" << filename << "\": " << strerror(errno) << "\n";
		return (NULL);
	}

	_files.push_back(new f_pollfd(filename, fd, POLLOUT, data));
	return (_files.back());
}

void
Server::_clear_previous_files(Client& client)
{
	for (std::vector<f_pollfd*>::iterator it = client.files().begin();
			it != client.files().end();
			++it)
				remove((*it)->name.data());
	client.files().clear();
}

int
Server::_list_directory(std::vector<std::string>& files, const std::string& path)
{
	DIR*	dirptr;
	dirent*	curr;

	// open directory
	if (!(dirptr = opendir(path.data())))
	{
		if (PollClass::get_pollclass()->get_raw_revents(2) == POLLOUT)
			std::cerr << "server > cannot open directory \"" << path << "\": " << strerror(errno) << "\n";
		return (-1);
	}

	errno = 0;
	while ((curr = readdir(dirptr))) // read directory entries
		files.push_back(curr->d_name);
	closedir(dirptr);
	if (errno) // readdir failed
	{
		if (PollClass::get_pollclass()->get_raw_revents(2) == POLLOUT)
			std::cerr << "server > cannot read directory \"" << path << "\": " << strerror(errno) << "\n";
		return (-1);
	}
	return (0);
}

void
Server::_check_existing_file(std::vector<std::string>& files, std::string& filename)
{
	std::string							curr	= filename;
	size_t								ext_pos	= filename.rfind('.');
	int									n		= 0;
	std::vector<std::string>::iterator	it		= files.begin();

	if (ext_pos == std::string::npos)
		ext_pos = filename.length();

	while (it != files.end())
	{
		if (*it == curr)
		{
			n++;
			curr = filename;
			curr.insert(ext_pos, " (" + itoa(n) + ")");
			it = files.begin();
		}
		else
			++it;
	}
	filename = curr;
}
