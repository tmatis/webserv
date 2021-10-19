/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_get.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:47 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/19 22:43:53 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

int
Server::_handle_get(Client &client, const Route& rules, const HTTPURI& uri)
{
	std::pair<std::string, std::string>		cgi;
	cgi = _check_cgi_extension(rules, uri.getPath());
	if (!cgi.first.empty())
	{
		// _handle_cgi(uri, client);
	}

	// search content requested by the client
	int code = _find_resource(rules, uri.getPath(), client);
	if (code != OK)
		return (_handle_error(client, code));
	else if (client.file()) // resource found
		client.state(IDLE);
	return (OK);
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
