/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:37 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/30 10:24:49 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Server.hpp"

std::pair<std::string, std::string>
Server::_check_cgi_extension(const Route& rules, const std::string& uri_path)
{
	std::pair<std::string, std::string>		ret;
	unsigned long							pos = std::string::npos;
	
	for (std::map<std::string, std::string>::const_iterator \
	it = rules.cgis.begin() ; it != rules.cgis.end() ; ++it)
	{
		unsigned long							tmp_pos;
		
		tmp_pos = uri_path.find((*it).first);
		while (tmp_pos != std::string::npos)
		{
			if (tmp_pos + (*it).first.size() == uri_path.size() || \
			uri_path[tmp_pos + (*it).first.size()] == '/')
			{
				if (tmp_pos < pos)
				{
					pos = tmp_pos;
					ret = (*it);
				}
				break ;
			}
			tmp_pos = uri_path.find((*it).first, tmp_pos + 1);
		}
	}
	return (ret);
}

pid_t
Server::_handle_cgi(Client& client, std::pair<std::string, std::string>& cgi)
{
	int code = 0;

	try {
		this->_cgis.push_back(CGI((*this), client, *client.rules(), client.request(), cgi));
		code = 1;
		this->_cgis.back().construct();
	} catch (std::exception &e) {
		if (code)
			this->_cgis.pop_back();
		return(_handle_error(client, 500));
	}
	{ // fast 404 check
		struct stat buffer;
		code = stat(this->_cgis.back().get_vars()["PATH_TRANSLATED"].c_str(), &buffer);
	}
	if (code)
	{
		this->_cgis.pop_back();
		return (_handle_error(client, 404));
	}
	try {
		this->_cgis.back().launch();
	} catch (std::exception &e) { this->_cgis.pop_back(); return(_handle_error(client, 500)); }

	client.state(IDLE);
	return (this->_cgis.back().get_pid());
}
