/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_get.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:47 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/30 03:19:51 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"
#include "../CGI.hpp"

pid_t
Server::_handle_get(Client &client, const Route& rules, const HTTPURI& uri)
{
	std::pair<std::string, std::string>		cgi;
	int 									code = 0;

	cgi = _check_cgi_extension(rules, uri.getPath());
	if (!cgi.first.empty())
		return (_handle_cgi(client, cgi));

	// search content requested by the client
	code = _find_resource(rules, uri.getPath(), client);
	if (code != OK)
		_handle_error(client, code);
	else if (client.files().size()) // resource found
		client.state(IDLE);
		
	return (getpid());
}

int
Server::_find_resource(const Route& rules, const std::string& uri_path, Client& client)
{
	std::string path = uri_path;

	// build path from root dir and uri path
	path.erase(0, rules.location.length());		// location/path/to/file -> /path/to/file
	path = HTTPGeneral::append_paths(rules._root, path);	// root/path/to/file

	struct stat	pathinfo;
	if (stat(path.data(), &pathinfo) == -1)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			return (NOT_FOUND); // path doesn't exist
		else
		{
			if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
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
			if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
				std::cerr << "server > cannot open directory \"" << path << "\": " << strerror(errno) << "\n";
			if (errno == EACCES)
				return (FORBIDDEN);
			return (INTERNAL_SERVER_ERROR);
		}

		errno = 0;
		while ((file = readdir(dirptr))) // read directory entries
		{
			if (_is_index_file(rules, file))
				break ; // index file found !
			dirls.push_back(*file);
		}
		if (errno) // readdir failed
		{
			if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
				std::cerr << "server > cannot read directory \"" << path << "\": " << strerror(errno) << "\n";
			return (INTERNAL_SERVER_ERROR);
		}

		if (!file) // index file not found...
		{
			closedir(dirptr);
			if (rules._autoindex) // autoindex enabled
			{
				client.response().gen_autoindex(dirls, path, uri_path);
				_create_response(client);
				return (OK);
			}
			return (NOT_FOUND);
		}
		else
			path = HTTPGeneral::append_paths(path, file->d_name); // path + filename
		closedir(dirptr);
	}
	
	if (_file_already_requested(client, path))
		return (OK); // another client asked for the same file

	// open file
	int fd = open(path.data(), O_RDONLY | O_NONBLOCK);
	if (fd == -1)
	{
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
			std::cerr << "server > cannot open file \"" << path << "\": " << strerror(errno) << "\n";
		if (errno == EACCES)
			return (FORBIDDEN);
		return (INTERNAL_SERVER_ERROR);
	}

	_files.push_back(new f_pollfd(path, fd));
	client.add_file(_files.back());
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

bool
Server::_file_already_requested(Client& client, std::string const &filepath)
{
	for (size_t i = 0; i < _files.size(); i++)
	{
		if (_files[i]->name == filepath) // file already in server's list
		{
			// assign file to the client
			client.add_file(_files[i]);
			return (true);
		}
	}
	return (false);
}
