/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 15:46:07 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 16:18:58 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP

# include <exception>
# include <string>

# include "TCP_Socket.hpp"

class Listener : public TCP_Socket
{
	public:

		Listener(const char* address, const int& port);

	private:
	
		class CreationFailure : public std::exception
		{
			public:
				CreationFailure(const char* errinfo);
				virtual const char*	what(void) const throw();
			private:
				std::string			_info;
		};
};

#endif
