/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:38:29 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/26 19:16:30 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#	include "CGI.hpp"

CGI::CGI(Server &server, Client &client, Route const &route, \
HTTPRequest const &httpreq, std::pair<std::string, std::string> const &cgi) : \
_server(server), _client(client), _route(route), _request(httpreq), \
cgi_infos(cgi), _var_formatted(NULL), _var_count(0), _pid(0), \
_response_flag(false), _state(0)
{
	this->_pipes_in[0] = 0;
	this->_pipes_in[1] = 0;
	this->_pipes_out[0] = 0;
	this->_pipes_out[1] = 0;
}
CGI::CGI(CGI const &cp) : _server(cp._server), _client(cp._client), \
_route(cp._route), _request(cp._request), cgi_infos(cp.cgi_infos), \
_var_formatted(cp._var_formatted), _var_count(cp._var_count), _pid(cp._pid), \
_response_flag(false), _state(cp._state)
{
	this->_pipes_in[0] = cp._pipes_in[0];
	this->_pipes_in[1] = cp._pipes_in[1];
	this->_pipes_out[0] = cp._pipes_out[0];
	this->_pipes_out[1] = cp._pipes_out[1];
}
CGI::~CGI()
{
	for (unsigned int i = 0 ; i < this->_var_count ; i++)
		delete[] this->_var_formatted[i];
	if (this->_var_count)
		delete[] this->_var_formatted;
	if (this->get_input_pipe())
		close(this->get_input_pipe());
	if (this->get_output_pipe())
	{
		close(this->get_output_pipe());
		// if (!kill(this->_pid, 0))
		// 	kill(this->_pid, SIGTERM);
	}
}

CGI &CGI::operator=(CGI const &)
{ return (*this); }

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
		++it;
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
	tampon = HTTPGeneral::append_paths(this->_route._root, tampon);
	memset(script_path, 0, 350);
	realpath(tampon.c_str(), script_path); // douteux
	vars["PATH_TRANSLATED"] = script_path;
	while (itr != this->_request.getHeader().getHeaders().end())
	{
		if ((*itr).first != "Content-Type" && (*itr).first != "Authorization" && \
		(*itr).first != "Content-Length" && (*itr).first != "Host")
			vars[std::string("HTTP_")\
			.append(this->get_var_formatted_str((*itr).first))] = (*itr).second;
		++itr;
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
		++i;
		++it;
	}
	this->_var_containers = vars;
	this->_var_formatted[vars.size()] = NULL;
	return (*this);
}

CGI			&CGI::launch()
{

	if (pipe(this->_pipes_in) == -1)
		throw std::runtime_error(strerror(errno));
	if (pipe(this->_pipes_out) == -1)
	{
		close(this->_pipes_in[0]);
		close(this->_pipes_in[1]);
		throw std::runtime_error(strerror(errno));
	}
	fcntl(this->_pipes_in[0], F_SETFL, O_NONBLOCK);
	fcntl(this->_pipes_in[1], F_SETFL, O_NONBLOCK);
	fcntl(this->_pipes_out[1], F_SETFL, O_NONBLOCK);
	fcntl(this->_pipes_out[0], F_SETFL, O_NONBLOCK);
	this->_pid = fork();
	if (this->_pid == -1)
	{
		close(this->_pipes_in[0]);
		close(this->_pipes_out[1]);
		close(this->_pipes_in[1]);
		close(this->_pipes_out[0]);
		throw std::runtime_error(strerror(errno));
	}
	if (this->_pid)
	{
		close(this->_pipes_in[0]);
		close(this->_pipes_out[1]);
		this->_server.get_files()\
		.push_back(new f_pollfd("cgi_input", this->_pipes_in[1], POLLOUT, "", true));
		this->_server.get_files()\
		.push_back(new f_pollfd("cgi_output", this->_pipes_out[0], POLLIN, "", true));
	}
	else
	{
		char 			*argv[] = {&this->cgi_infos.second[0], \
		&this->_var_containers["PATH_TRANSLATED"][0], NULL};

		close(this->_pipes_in[1]);
		close(this->_pipes_out[0]);
		dup2(this->_pipes_in[0], STDIN_FILENO);
		dup2(this->_pipes_out[1], STDOUT_FILENO);
		if (execve(this->cgi_infos.second.c_str(), argv, this->_var_formatted) == -1)
		{
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
			std::cerr << "cgi: \'" << this->cgi_infos.second << "\' error while launching : \'" << \
			strerror(errno) << "\'" << std::endl;
			exit(EXIT_SUCCESS);
		}
	}
	return (*this);
}

bool			CGI::send_request(int const &revents)
{
	if (!this->_request.getBody().size())
	{
		this->erase_pipe(&this->_pipes_in[1]);
		this->_state++;
		return (true);
	}
	if (revents & POLLERR || revents & POLLHUP || revents & POLLNVAL)
	{
		this->erase_pipe(&this->_pipes_in[1]);
		throw std::runtime_error("poll error req");
	}
	if (revents & POLLOUT)
	{
		if (write(this->get_input_pipe(), this->_request.getBody().data(), \
		this->_request.getBody().size()) == -1)
		{
			this->erase_pipe(&this->_pipes_in[1]);
			throw std::runtime_error(strerror(errno));
		}
		this->erase_pipe(&this->_pipes_in[1]);
		this->_state++;
		return (true);
	}
	return (false);
}

bool			CGI::get_response(int const &revents)
{
	if (revents & POLLERR || revents & POLLNVAL)
	{
		this->erase_pipe(&this->_pipes_out[0]);
		throw std::runtime_error("poll error res");
	}
	if (revents & POLLIN || this->_response_flag)
	{
		char		buffer[1024];
		ssize_t			i;
		bool		parsecgi_ret = false;

		memset(buffer, 0, 1024);
		i = read(this->get_output_pipe(), buffer, 1023);
		if (i != -1)
		{
			ft::random_access_iterator<char> it(buffer);
			ft::random_access_iterator<char> ite(buffer + i);
			this->_response_flag = true;
			parsecgi_ret = this->get_client().response().parseCGI(std::string(it, ite));
		}
		if (i > 0 && !parsecgi_ret)
			return (true);
		this->erase_pipe(&this->_pipes_out[0]);
		if (i == -1)
			throw std::runtime_error(strerror(errno));
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
{ return (this->_pipes_in[1]); }

int				CGI::get_output_pipe()
{ return (this->_pipes_out[0]); }

char			**CGI::get_var_formatted()
{ return (this->_var_formatted); }

Client			&CGI::get_client()
{ return (this->_client); }

std::map<std::string, std::string>	&CGI::get_vars()
{ return (this->_var_containers); }

bool const		&CGI::get_response_flag() const
{ return (this->_response_flag); }

int				CGI::get_state() const
{ return (this->_state); }

void			CGI::set_state(int state)
{ this->_state = state; }

void			CGI::erase_pipe(int *pipe)
{
	for (std::vector<f_pollfd *>::iterator it = \
	this->_server.get_files().begin() ; it != \
	this->_server.get_files().end() ; ++it)
		if ((*it)->pfd.fd == *pipe)
		{
			delete (*it);
			this->_server.get_files().erase(it);
			break ;
		}
	close(*pipe);
	*pipe = 0;
}
