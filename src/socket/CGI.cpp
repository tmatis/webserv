/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:38:29 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/21 00:10:47 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#	include "CGI.hpp"

CGI::CGI(Server &server, Client &client, Route const &route, \
HTTPRequest const &httpreq, std::pair<std::string, std::string> const &cgi) : \
_server(server), _client(client), _route(route), _request(httpreq), \
cgi_infos(cgi), _var_formatted(NULL), _var_count(0), _pid(0), _state(0) {}
CGI::CGI(CGI const &cp) : _server(cp._server), _client(cp._client), \
_route(cp._route), _request(cp._request), cgi_infos(cp.cgi_infos), \
_var_formatted(cp._var_formatted), _var_count(cp._var_count), _pid(cp._pid), \
_state(cp._state) {}
CGI::~CGI()
{
	for (unsigned int i = 0 ; i < this->_var_count ; i++)
		delete[] this->_var_formatted[i];
	if (this->_var_count)
		delete[] this->_var_formatted;
}

CGI			&CGI::operator=(CGI const &rhs) { (void)rhs; return (*this); }

CGI			&CGI::construct()
{
	std::map<std::string, std::string>	vars;
	std::string							tampon = this->_request.getURI().getPath();
	size_t								pos = tampon.find(this->cgi_infos.first);
	char								script_path[350];
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
	vars["CONTENT_LENGTH"] = "0";

	if (this->_request.getHeader().getValue("Content-Length")) //
		vars["CONTENT_LENGTH"] = *(this->_request.getHeader().getValue("Content-Length")); //
	vars["SERVER_SOFTWARE"] = this->_server.get_config()._server_name_version;
	vars["SERVER_NAME"] = this->_request.getHost().substr(0, this->_request.getHost().find(':')); //
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
		if (pos + this->cgi_infos.first.size() < tampon.size() && \
		tampon[pos + this->cgi_infos.first.size()] == '/')
		{
			vars["SCRIPT_NAME"] = tampon.substr(0, pos + this->cgi_infos.first.size());
			tampon.erase(0, pos + this->cgi_infos.first.size());
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
	memset(script_path, 0, 350);
	realpath(tampon.c_str(), script_path); // douteux
	vars["PATH_TRANSLATED"] = script_path;
	while (itr != this->_request.getHeader().getHeaders().end())
	{
		if ((*itr).first != "Content-Type" && (*itr).first != "Authorization" && \
		(*itr).first != "Content-Length" && (*itr).first != "Host")
			vars[std::string("HTTP_")\
			.append(this->get_var_formatted_str((*itr).first))] = (*itr).second;
		itr++;
	}
	it = vars.begin();
	this->_var_formatted = new char *[vars.size() + 1];
	this->_var_count = vars.size();
	while (it != vars.end())
	{
		this->_var_formatted[i] = new char[(*it).first.size() + \
		(*it).second.size() + 2];
		tampon = (*it).first + std::string("=") + (*it).second;
		std::strcpy(this->_var_formatted[i], tampon.c_str());
		this->_var_formatted[i][(*it).first.size() + (*it).second.size() + 1] = 0;
		i++;
		it++;
	}
	this->_var_containers = vars;
	this->_var_formatted[vars.size()] = NULL;
	return (*this);
}

CGI			&CGI::launch()
{
	char 			*argv[] = {NULL};
	std::string		error_case = "WEBSERV-CGI-ERROR: ";

	if (pipe(this->_pipes_in) == -1)
		throw std::runtime_error(strerror(errno));
	if (pipe(this->_pipes_out) == -1)
		throw std::runtime_error(strerror(errno));
	this->_pid = fork();
	if (this->_pid == -1)
		throw std::runtime_error(strerror(errno));
	if (this->_pid)
	{
		close(this->_pipes_in[0]);
		close(this->_pipes_out[1]);
		this->_server.get_files()\
		.push_back(f_pollfd("cgi_input", this->_pipes_in[1], POLLOUT, "", true));
		this->_fds.first = &(this->_server.get_files().back());
		this->_server.get_files()\
		.push_back(f_pollfd("cgi_output", this->_pipes_out[0], POLLIN, "", true));
		this->_fds.second = &(this->_server.get_files().back());
	}
	else
	{
		close(this->_pipes_in[1]);
		close(this->_pipes_out[0]);
		dup2(this->_pipes_in[0], STDIN_FILENO);
		dup2(this->_pipes_out[1], STDOUT_FILENO);
		if (execve(this->cgi_infos.second.c_str(), argv, this->_var_formatted) == -1)
		{
			error_case.append(strerror(errno));
			write(STDOUT_FILENO, error_case.c_str(), error_case.size());
			exit(EXIT_SUCCESS);
		}
	}
	return (*this);
}

bool			CGI::send_request()
{
	if (!this->_request.getBody().size())
	{
		for (std::vector<f_pollfd>::iterator it = \
		this->_server.get_files().begin() ; it != \
		this->_server.get_files().end() ; it++)
			if ((*it).pfd.fd == this->_fds.first->pfd.fd)
			{
				this->_server.get_files().erase(it);
				break ;
			}
		close(this->get_input_pipe());
		this->_state++;
		return (true);
	}
	if (this->_fds.first->pfd.revents == POLLERR || \
	this->_fds.first->pfd.revents == POLLHUP || \
	this->_fds.first->pfd.revents == POLLNVAL)
	{
		for (std::vector<f_pollfd>::iterator it = \
		this->_server.get_files().begin() ; it != \
		this->_server.get_files().end() ; it++)
			if ((*it).pfd.fd == this->_fds.first->pfd.fd)
			{
				this->_server.get_files().erase(it);
				break ;
			}
		close(this->get_input_pipe());
		throw std::runtime_error("poll error");
	}
	if (this->_fds.first->pfd.revents == POLLOUT)
	{
		for (std::vector<f_pollfd>::iterator it = \
		this->_server.get_files().begin() ; it != \
		this->_server.get_files().end() ; it++)
			if ((*it).pfd.fd == this->_fds.first->pfd.fd)
			{
				this->_server.get_files().erase(it);
				break ;
			}
		if (write(this->get_input_pipe(), this->_request.getBody().c_str(), \
		this->_request.getBody().size()) == -1)
		{
			close(this->get_input_pipe());
			throw std::runtime_error(strerror(errno));
		}
		close(this->get_input_pipe());
		this->_state++;
		return (true);
	}
	return (false);
}

bool			CGI::get_response()
{
	char		buffer[200];
	int			i = 1;

	// std::cout << this->_fds.second->pfd.events << std::endl;
	if (this->_fds.second->pfd.revents == POLLERR || \
	this->_fds.second->pfd.revents == POLLHUP || \
	this->_fds.second->pfd.revents == POLLNVAL)
	{
		for (std::vector<f_pollfd>::iterator it = \
		this->_server.get_files().begin() ; it != \
		this->_server.get_files().end() ; it++)
			if ((*it).pfd.fd == this->_fds.second->pfd.fd)
			{
				this->_server.get_files().erase(it);
				break ;
			}
		close(this->get_output_pipe());
		throw std::runtime_error("poll error");
	}
	if (this->_fds.second->pfd.revents == POLLIN)
	{
		while (i > 0)
		{
			memset(buffer, 0, 200);
			i = read(this->get_output_pipe(), buffer, 199);
			if (i != -1)
				this->_response += buffer;
		}
		for (std::vector<f_pollfd>::iterator it = \
		this->_server.get_files().begin() ; it != \
		this->_server.get_files().end() ; it++)
			if ((*it).pfd.fd == this->_fds.second->pfd.fd)
			{
				this->_server.get_files().erase(it);
				break ;
			}
		close(this->get_output_pipe());
		if (i == -1)
			throw std::runtime_error(strerror(errno));
		// std::cout << "'" << this->_response << "'" << std::endl;
		this->_state++;
		return (true);
	}
	return (false);
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

int				CGI::get_input_pipe()
{
	return (this->_pipes_in[1]);
}

int				CGI::get_output_pipe()
{
	return (this->_pipes_out[0]);
}

char			**CGI::get_var_formatted()
{
	return (this->_var_formatted);
}

Client			&CGI::get_client()
{
	return (this->_client);
}

std::map<std::string, std::string>	&CGI::get_vars()
{
	return (this->_var_containers);
}

int				CGI::get_state() const
{
	return (this->_state);
}
