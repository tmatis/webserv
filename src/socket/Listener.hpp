/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:46:07 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 23:25:38 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP

# include "TCP_Socket.hpp"

class Listener : public TCP_Socket
{
	public:
		Listener(const char *address, int port);
};

#endif
