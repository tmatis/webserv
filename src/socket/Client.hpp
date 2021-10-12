/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:19:23 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 22:33:35 by mamartin         ###   ########.fr       */
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

typedef struct f_pollfd
{
	f_pollfd(const std::string& filename, int fd);

	std::string	name;
	pollfd		pfd;
}		f_pollfd;

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
		const f_pollfd*	file(void) const;

		// setters
		void			state(client_state st);
		void			file(const f_pollfd* f_pfd);

	private:

		client_state	_state;
		HTTPRequest		_request;
		HTTPResponse	_response;
		const f_pollfd*	_file;
};

#endif
