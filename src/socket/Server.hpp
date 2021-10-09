/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 17:57:16 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/09 13:28:21 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <algorithm>
# include <vector>

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
		void	flush_clients(void);



		/*
			following functions may be useful... or not
			i will add them if they're necessary in the future
		
			get hosts ?
			get clients ?

			~~~~~ YESSSS they are hehe
		*/
		std::vector<Client> const	&get_clients() const;
		std::vector<pollfd> const	&get_files() const;
		Listener			const	&get_listener() const;

	private:

		Listener				_host;		// listener socket
		std::vector<Client>		_clients;	// list of clients connected
		std::vector<pollfd>		_files;		// files opened
		std::vector<Config>		_configs;	// configs on the same address:port
};

#endif
