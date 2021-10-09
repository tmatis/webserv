/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:19:23 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/09 13:28:40 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <poll.h>

# include "TCP_Socket.hpp"
# include "../http/HTTPRequest.hpp"
# include "../http/HTTPResponse.hpp"

typedef struct pollfd
		pollfd;

typedef	enum e_client_state
{
	IDLE,
	WAITING_ANSWER,
	PENDING_REQUEST,
	DISCONNECTED
}		client_state;

typedef	struct client_data
{
	HTTPRequest		request;
	HTTPResponse	response;
}		client_data;

class Client : public TCP_Socket
{
	public:
	
		Client(void);
		
		int					connect(int host_fd);
		operator			pollfd() const;

		// getters
		client_state		state(void) const;

	private:

		client_state		_state;
		client_data			_http;
};

#endif
