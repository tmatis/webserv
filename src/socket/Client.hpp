/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:19:23 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 00:33:03 by mamartin         ###   ########.fr       */
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

class Client : public TCP_Socket
{
	public:
	
		Client(void);
		
		int				connect(int host_fd);
		operator		pollfd() const;

		// getters
		client_state	state(void) const;
		HTTPRequest&	request(void);
		HTTPResponse&	response(void);

		// setters
		void			state(client_state st);

	private:

		client_state	_state;
		HTTPRequest		_request;
		HTTPResponse	_response;
};

#endif
