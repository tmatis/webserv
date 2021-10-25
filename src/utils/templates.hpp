/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   templates.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 11:44:54 by tmatis            #+#    #+#             */
/*   Updated: 2021/10/25 11:53:10 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEMPLATES_HPP
# define TEMPLATES_HPP

# include <string>
# include <sstream>

template <typename T>
std::string itoa(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

template <typename T>
T aton(std::string const &str)
{
	std::istringstream is(str);
	T value;
	is >> value;
	return value;
}

#endif
