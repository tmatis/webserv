/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:46:07 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/13 15:31:32 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP

# include "TCP_Socket.hpp"

class Listener : public TCP_Socket
{
	public:
		Listener(unsigned int address, unsigned short port);
};

#endif
