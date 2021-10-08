/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCP_Socket.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:16:14 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 18:16:31 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TCP_SOCKET_HPP
# define TCP_SOCKET_HPP

# include <unistd.h>
# include <cstring>
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

		const int			get_fd(void) const;
		const sockaddr_in&	get_addr(void) const;
		
	protected:

		class CreationFailure : public std::exception
		{
			public:
				CreationFailure(const char* errinfo);
				virtual const char*	what(void) const throw();
			private:
				std::string			_info;
		};

		static int			set_non_blocking(TCP_Socket& sock);

		int					_fd;
		sockaddr_in			_addr;
};

#endif
