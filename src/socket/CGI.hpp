	/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:34:38 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/24 14:01:03 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WCGI
# define WCGI
# include "Server.hpp"
# include "../utils/random_access_iterator.hpp"
# include <cctype>
# include <cstring>
# include <stdexcept>

class Server;

class CGI
{
	private:
	Server								&_server;
	Client								&_client;
	Route const							&_route;
	HTTPRequest const					&_request;
	std::pair<std::string, std::string>	cgi_infos;
	char								**_var_formatted;
	std::map<std::string, std::string>	_var_containers;
	int									_pipes_in[2];
	int									_pipes_out[2];
	unsigned int						_var_count;
	pid_t								_pid;
	bool								_response_flag;
	int									_state;
	/* first = input ; second = output */
	std::pair<f_pollfd *, f_pollfd *>	_fds;

	public:
	CGI(Server &server, Client &client, Route const &route, HTTPRequest const &httpreq, \
	std::pair<std::string, std::string> const &cgi);
	CGI(CGI const &cp);
	~CGI();

	CGI						&operator=(CGI const &rhs);
	static std::string		get_var_formatted_str(std::string const &var_name);
	CGI						&construct();

	/* in the case where execve could fail, the get_output_pipe() would be
	 * filled with "WEBSERV-CGI-ERROR: <strerror>"
	 * */
	CGI						&launch();

	/* needs to be run, if there's no body to send the function returns true
	 * */
	bool					send_request(int const &revents);
	bool					get_response(int const &revents);

	int					get_input_pipe();
	int					get_output_pipe();
	Client				&get_client();
	char				**get_var_formatted();
	std::map<std::string, std::string>	&get_vars();
	bool const			&get_response_flag() const;
	int					get_state() const;
};


#endif
