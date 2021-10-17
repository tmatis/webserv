/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCP_Socket.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:16:14 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/17 12:07:53 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCP_SOCKET_HPP
# define TCP_SOCKET_HPP

# include <unistd.h>
# include <cstring>
# include <cerrno>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <exception>
# include <string>

class TCP_Socket
{
	public:

		TCP_Socket(int fd = -1, const sockaddr_in& address = sockaddr_in());
		TCP_Socket(const TCP_Socket& rhs);

		TCP_Socket&			operator=(const TCP_Socket& rhs);

		int					fd(void) const;
		const sockaddr_in&	addr(void) const;
		
	protected:

		class CreationFailure : public std::exception
		{
			public:
				explicit CreationFailure(const char* errinfo);
				virtual ~CreationFailure() throw();
				virtual const char*	what(void) const throw();
			private:
				std::string			_info;
		};

		static int			set_non_blocking(TCP_Socket& sock);

		int					_fd;
		sockaddr_in			_addr;
};

#endif
