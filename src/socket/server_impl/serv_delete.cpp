/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_delete.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:43 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/28 15:46:01 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

int
Server::_handle_delete(Client& client, const Route& rules, const HTTPURI& uri)
{
	std::string path = uri.getPath();

	// build path from root dir and uri path
	path.erase(0, rules.location.length());					// location/path/to/file -> /path/to/file
	path = HTTPGeneral::append_paths(rules._root, path);	// root/path/to/file

	if (remove(path.data()) == -1)
	{
		if (PollClass::get_pollclass()->get_raw_revents(STDERR_FILENO) == POLLOUT)
			std::cerr << "server > deletion of \"" << path << "\" failed: " << strerror(errno) << "\n";
		if (errno == EACCES || errno == EROFS || errno == EPERM)
			_handle_error(client, FORBIDDEN); // 403
		else if (errno == ENOENT || errno == ENOTDIR)
			_handle_error(client, NOT_FOUND); // 404
		else
			_handle_error(client, INTERNAL_SERVER_ERROR); // 500
	}
	else
	{
		client.response().setStatus(NO_CONTENT); // 204
		_create_response(client);
	}
	return (1);
}
