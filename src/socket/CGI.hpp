/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 17:34:38 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/18 23:26:40 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WCGI
# define WCGI
# include "Server.hpp"
# include <sstream>
# include <cctype>
# include <cstring>

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
	int									_pipes[2];
	unsigned int						_var_count;
	
	CGI(Server &server, Client &client, Route &route, HTTPRequest const &httpreq, \
	std::pair<std::string, std::string> const &cgi);
	~CGI();

	CGI		&construct();
	int		get_input_pipe();
	int		get_output_pipe();

	public:
	static std::string		get_var_formatted_str(std::string const &var_name);

	class CGI_bad_header : public std::exception
	{
		CGI_bad_header() throw();
		virtual ~CGI_bad_header() throw();

		virtual char const *what() const throw();
	};
};


#endif