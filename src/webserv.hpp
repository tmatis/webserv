/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:24 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 21:23:10 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <cerrno>
# include <cstdio>

# include "polling/polling.hpp"
# include "socket/Server.hpp"

# include "http/HTTPURI.hpp"

# define POLL_TIMEOUT	30000

/*** PARSING ******************************************************************/

std::vector<Config>	read_config_file(char* filename);

/*** POLL *********************************************************************/

int						handle_events(PollClass& pc, Server *host);
int						handle_events(PollClass& pc, Server *host, Client& client);
int						event_pollin(Server* host, Client& client);
int						event_pollout(Server* host, Client& client);
int						event_pollhup(Server* host, Client& client);
int						event_pollerr(Server* host, Client& client);
int						event_pollnval(Server* host, Client& client);

/*** UTILS *******************************************************************/

void					destroy_servers(std::vector<Server*>& list);

#endif
