/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/13 00:02:57 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>
# include <vector>
# include <unistd.h>
# include <sys/stat.h>
# include <dirent.h>

# include "Listener.hpp"
# include "Client.hpp"
# include "../config/Config.hpp"

class Server
{
	public:

		typedef std::vector<Client>::iterator		client_iterator;

		Server(const Config& conf);
		virtual ~Server(void);

		// handle connections
		int		add_new_client(void);
		void	flush_clients(void);

		// handle HTTP messages
		int		handle_request(Client& client);
		void	send_response(Client& client);

		// getters
		std::vector<Client>&			get_clients(void);
		const std::vector<f_pollfd>&	get_files(void) const;
		const Listener&					get_listener(void) const;

	private:

		bool						_read_request(Client &client);
		const Route&				_resolve_routes(const std::string& uri);
		int							_check_request_validity(const Route& rules, HTTPRequest& request);
		//int							_check_cgi_extension(const Route& rules, const URI& uri);
		int							_find_resource(const Route& rules, std::string path, std::string& res);
		bool						_is_index_file(const Route& rules, struct dirent* file);
		int							_handle_error(Client& client, int status, bool autogen = false);

		Listener					_host;		// listener socket
		std::vector<Client>			_clients;	// list of clients connected
		std::vector<f_pollfd>		_files;		// files opened
		const Config&				_config;	// configuration of the server

		static const std::string	_all_methods[3]; // methods implemented by the server
};

#endif
