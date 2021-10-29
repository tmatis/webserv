/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 00:40:24 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/28 15:20:58 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <cerrno>
# include <cstdio>
# include <csignal>

# include "polling/polling.hpp"
# include "config/MasterConfig.hpp"
# include "config/Config.hpp"
# include "socket/Server.hpp"

# include "http/HTTPURI.hpp"

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
void					close_all_fds(std::vector<Server*>& list);
void					print_error(const char* message);

#endif
