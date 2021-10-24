/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_cgi.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 03:11:37 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/24 14:18:51 by nouchata         ###   ########.fr       */
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
