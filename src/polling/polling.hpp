/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   polling.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:55:51 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/09 13:56:35 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLING_HPP
# define POLLING_HPP
# include <map>
# include <string>
# include <iostream>
# include <vector>
# include <utility>
# include "poll.h"
# include "../socket/Server.hpp"

class ServerClass;
class ClientClass;

class PollClass
{
	public:
	typedef struct pollfd	pfd;
	typedef Server			server;
	typedef Client			client;

	private:
	unsigned int						_timeout;
	std::vector<std::pair<unsigned int, unsigned int> >	_server_data_size;
	std::vector<pfd>					_pfd_list;
	std::vector<server *>				_server_list;

	void			update_server_pfd(unsigned int index);
	/* WARNING :
	 * to avoid undefined behavior w/ this function, you need to fill
	 * file stream and clients vectors with push_back(). */
	void			update_pfd();

	public:
	PollClass(int const &timeout = 30000);
	PollClass(PollClass const &cp);
	~PollClass();

	PollClass		&operator=(PollClass const &rhs);
	void			add_server(server &new_server);
	/* WARNING : 
	 * before removing a server, you have to make sure that it hasn't
	 * accepted new clients since the last call to polling(). */
	void			remove_server(server *server);
	int				polling();
	int				get_raw_revents(int const &fd, server *server = NULL);
	int				get_raw_revents(client const &cl, server *server = NULL);

	static pfd		make_pollfd(int fd, int events = POLLIN);
};

#endif