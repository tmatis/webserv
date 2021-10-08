/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCP_Socket.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:16:14 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 15:51:24 by mamartin         ###   ########.fr       */
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

class TCP_Socket
{
	public:

		TCP_Socket(const int& fd = -1, const sockaddr_in& address = sockaddr_in());
		TCP_Socket(const TCP_Socket& rhs);

		TCP_Socket&			operator=(const TCP_Socket& rhs);

		int					set_non_blocking(void);

		const int&			get_fd(void) const;
		const sockaddr_in&	get_name(void) const;

	protected:

		int					_fd;
		sockaddr_in			_addr;
};

#endif
