/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 15:06:01 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>
# include <vector>

# include "Listener.hpp"
# include "Client.hpp"
# include "../config/Config.hpp"

typedef struct f_pollfd
{
	std::string	filename;
	pollfd		pfd;
}		f_pollfd;

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
		int							_resolve_host(HTTPRequest& request);
		const Route&				_resolve_routes(const std::string& uri);
		int							_check_request_validity(const Route& rules, HTTPRequest& request);

		Listener					_host;		// listener socket
		std::vector<Client>			_clients;	// list of clients connected
		std::vector<f_pollfd>		_files;		// files opened
		const Config&				_config;	// configuration of the server

		static const std::string	_all_methods[3]; // methods implemented by the server
};

#endif
