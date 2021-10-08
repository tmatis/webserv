/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 23:16:14 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>

# include "Listener.hpp"
# include "Client.hpp"
# include "../config/Config.hpp"

class Server
{
	public:

		typedef std::vector<Client>::iterator	client_iterator;

		Server(Config* conf, int n);
		virtual ~Server(void);

		int		add_new_client(void);
		void	delete_client(client_iterator client);
		void	flush_clients(void);

		/*
			following functions may be useful... or not
			i will add them if they're necessary in the future
		
			get hosts ?
			get clients ?
		*/

	private:

		void					_close_socket(TCP_Socket& sock);

		Listener				_host;		// listener socket
		std::vector<Client>		_clients;	// list of clients connected
		std::vector<pollfd>		_files;		// files opened
		std::vector<Config>		_configs;	// configs on the same address:port
};

#endif
