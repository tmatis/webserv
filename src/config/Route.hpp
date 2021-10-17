/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmatis <tmatis@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 21:46:26 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/17 12:08:21 by tmatis           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "Config.hpp"

struct Config;

typedef struct Route : public Config
{
	explicit Route(Config const &base);
	Route(Route const &cp);
	~Route();

	Route				&operator=(Route const &rhs);
	void				construct(std::pair<std::string, std::string> &config_str);
	void				fill_var(std::pair<std::string, std::string> const &var_pair);

	void	set_cgi(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_upload_path(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);

	std::string					location;
	std::string					cgi_extension;
	std::string					cgi_path;
	std::string					upload_path;
}		Route;

#endif
