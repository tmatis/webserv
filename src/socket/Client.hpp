/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:19:23 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/20 15:42:41 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <poll.h>
# include <ctime>

# include "TCP_Socket.hpp"
# include "../http/HTTPRequest.hpp"
# include "../http/HTTPResponse.hpp"
# include "../config/Config.hpp"

typedef struct pollfd
		pollfd;

typedef struct f_pollfd
{
	f_pollfd(const std::string& filename, int fd, int event = POLLIN, const std::string& data = "");

	operator pollfd() const;

	std::string	name;
	std::string	data;
	pollfd		pfd;
}		f_pollfd;

typedef	enum e_client_state
{
	IDLE,
	WAITING_ANSWER,
	PENDING_REQUEST,
	DISCONNECTED
}		client_state;

# define BUFFER_SIZE	1024

class Client : public TCP_Socket
{
	public:
	
		Client(void);
		
		int								connect(int host_fd);
		operator						pollfd() const;

		// getters
		client_state					state(void) const;
		const Route*					rules(void) const;
		HTTPRequest&					request(void);
		HTTPResponse&					response(void);
		std::vector<const f_pollfd*>&	files(void);

		// setters
		void							state(client_state st);
		void							rules(const Route* rules);
		void							clear(void);

		int								write_trials;
		std::time_t						last_request;

	private:

		client_state					_state;
		HTTPRequest						_request;
		HTTPResponse					_response;
		std::vector<const f_pollfd*>	_files;
		const Route*					_route;
};

#endif
