/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 16:19:23 by mamartin          #+#    #+#             */
/*   Updated: 2021/11/02 14:27:16 by mamartin         ###   ########.fr       */
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
	f_pollfd(const std::string& filename, int fd, int event = POLLIN, const std::string& data = "", bool cgi = false);

	operator pollfd(void) const;

	std::string	name;
	std::string	data;
	pollfd		pfd;
	bool		cgi_file;
}		f_pollfd;

typedef struct string_response
{
	string_response(void);

	std::string	data;
	size_t		written;
}		str_response;

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
		std::vector<f_pollfd*>&			files(void);
		str_response&					string_response(void);
		int&							files_number(void);

		// setters
		void							state(client_state st);
		void							rules(const Route* rules);
		void							add_file(f_pollfd* fpfd);
		void							clear(void);

		int								write_trials;
		std::time_t						last_request;

	private:

		client_state					_state;
		HTTPRequest						_request;
		HTTPResponse					_response;
		std::vector<f_pollfd*>			_files;
		const Route*					_route;
		int								_n_files;
		str_response					_str;
};

#endif
