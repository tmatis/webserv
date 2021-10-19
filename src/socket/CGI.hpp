/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:34:38 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/19 22:16:24 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WCGI
# define WCGI
# include "Server.hpp"
# include <sstream>
# include <cctype>
# include <cstring>
# include <stdexcept>

class CGI
{
	private:
	Server								&_server;
	Client								&_client;
	Route								&_route;
	HTTPRequest const					&_request;
	std::pair<std::string, std::string>	cgi_infos;
	std::vector<std::string>			_var_containers;
	char								**_var_formatted;
	int									_pipes_in[2];
	int									_pipes_out[2];
	unsigned int						_var_count;
	pid_t								_pid;
	std::string							_response;
	/* first = input ; second = output */
	std::pair<f_pollfd *, f_pollfd *>	_fds;
	
	CGI(Server &server, Client &client, Route &route, HTTPRequest const &httpreq, \
	std::pair<std::string, std::string> const &cgi);
	~CGI();

	public:
	static std::string		get_var_formatted_str(std::string const &var_name);
	CGI						&construct();

	/* in the case where execve could fail, the get_output_pipe() would be
	 * filled with "WEBSERV-CGI-ERROR: <strerror>"
	 * */
	CGI						&launch();

	/* needs to be run, if there's no body to send the function returns true
	 * */
	bool					send_request();
	bool					get_response();

	int					get_input_pipe();
	int					get_output_pipe();
	std::string const	get_response() const;
};


#endif