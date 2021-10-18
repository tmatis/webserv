/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:38:29 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/18 23:28:33 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#	include "CGI.hpp"

CGI::CGI(Server &server, Client &client, Route &route, \
HTTPRequest const &httpreq, std::pair<std::string, std::string> const &cgi) : \
_server(server), _client(client), _route(route), _request(httpreq), \
cgi_infos(cgi), _var_formatted(NULL) {}
CGI::~CGI() {}

CGI			&CGI::construct()
{
	std::map<std::string, std::string>	vars;
	std::string							tampon = this->_request.getURI().getPath();
	size_t								pos = tampon.find(this->cgi_infos.first);
	char								script_path[700];
	unsigned int 						i = 0;
	std::map<std::string, std::string>::const_iterator \
	it = this->_request.getURI().getQuery().begin();
	std::vector<std::pair<std::string, std::string > >::const_iterator \
	itr = this->_request.getHeader().getHeaders().begin();

	vars["REMOTE_HOST"] = std::string();
	vars["AUTH_TYPE"] = std::string();
	vars["REMOTE_USER"] = std::string();
	vars["REMOTE_IDENT"] = std::string(); // not supported since nothing asked it
	vars["QUERY_STRING"] = std::string();
	vars["CONTENT_TYPE"] = std::string();

	vars["CONTENT_LENGTH"] = *(this->_request.getHeader().getValue("Content-Length"));
	vars["SERVER_SOFTWARE"] = this->_server.get_config()._server_name_version;
	vars["SERVER_NAME"] = this->_request.getHost();
	vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	vars["SERVER_PROTOCOL"] = this->_request.getVersion();
	vars["SERVER_PORT"] = this->_server.get_config().port_str;
	vars["REQUEST_METHOD"] = this->_request.getMethod();
	vars["REMOTE_ADDR"] = inet_ntoa(this->_client.addr().sin_addr);
	while (pos != std::string::npos)
	{
		if (pos + this->cgi_infos.first.size() == tampon.size())
		{
			vars["SCRIPT_NAME"] = tampon;
			vars["PATH_INFO"] = std::string();
			break ;
		}
		if (tampon[pos + 1] == '/')
		{
			vars["SCRIPT_NAME"] = tampon.substr(0, pos + 1);
			tampon.erase(0, pos + 2);
			vars["PATH_INFO"] = tampon;
			break ;
		}
		pos = tampon.find(this->cgi_infos.first, pos + 1);
	}
	while (it != this->_request.getURI().getQuery().end())
	{
		if (!vars["QUERY_STRING"].empty())
			vars["QUERY_STRING"] += "&";
		vars["QUERY_STRING"] += (*it).first + std::string("=") + (*it).second;
		it++;
	}
	if (this->_request.getHeader().getValue("Authorization") != NULL)
	{
		tampon = *(this->_request.getHeader().getValue("Authorization"));
		if (tampon.find("Basic") == 0)
		{
			vars["AUTH_TYPE"] = "Basic";
			if (tampon.size() > 5 && tampon[5] == ' ')
			{
				tampon.erase(0, 6);
				vars["REMOTE_USER"] = tampon;
			}
		}
	}
	if (this->_request.getHeader().getValue("Content-Type") != NULL)
		vars["CONTENT_TYPE"] = *(this->_request.getHeader().getValue("Content-Type"));
	tampon = vars["SCRIPT_NAME"];
	tampon.erase(0, this->_route.location.length());
	tampon = this->_server._append_paths(this->_route._root, tampon);
	vars["PATH_TRANSLATED"] = realpath(tampon.c_str(), script_path);
	while (itr != this->_request.getHeader().getHeaders().end())
	{
		if ((*itr).first != "Content-Type" && (*itr).first != "Authorization" && \
		(*itr).first != "Content-Length" && (*itr).first != "Host")
			vars[std::string("HTTP_")\
			.append(this->get_var_formatted_str((*itr).first))] = (*itr).second;
		itr++;
	}
	it = vars.begin();
	this->_var_formatted = new char *[vars.size()];
	this->_var_count = vars.size();
	while (it != vars.end())
	{
		this->_var_formatted[i] = new char[(*it).first.size() + \
		(*it).second.size() + 2];
		tampon = (*it).first + std::string("=") + (*it).second;
		std::strcpy(this->_var_formatted[i], tampon.c_str());
		this->_var_containers.push_back((*it).first + \
		std::string("=") + (*it).second);
		it++;
	}
	return (*this);
}

std::string		CGI::get_var_formatted_str(std::string const &var_name)
{
	std::string		res = var_name;

	for (unsigned int i = 0 ; i < res.size() ; i++)
	{
		res[i] = toupper(res[i]);
		if (res[i] == '-')
			res[i] = '_';
	}
	return (res);
}