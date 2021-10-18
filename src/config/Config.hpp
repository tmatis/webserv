/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 22:36:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/18 18:43:20 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <map>
# include <vector>
# include <arpa/inet.h>
# include "MasterConfig.hpp"

/*
** further comments define possible default values for each field
*/

struct Route;

typedef	struct Config : public MasterConfig
{
	public:
	Config(void);

	public:
	explicit Config(MasterConfig const &master);
	Config(Config const &cp);
	~Config();

	Config				&operator=(Config const &rhs);
	void				construct(std::string &config_str);
	void				fill_var(std::pair<std::string, std::string> const &var_pair);

	std::set<std::string>		methods;
	std::string					address;
	std::string					port_str;
	unsigned long				address_res;	// mandatory
	unsigned short				port;			// mandatory
	std::pair<int, std::string>	redirection;
	std::set<std::string>		server_names;	// ""
	size_t						body_limit;		// 0 for none
	std::vector<Route>			routes;			// if no route is specified with location="/", then create a default one

	protected:
	void	set_listen(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_server_names(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_redirection(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_body_limit(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
	void	set_methods(std::pair<std::string, std::string> const &var_pair, \
	std::vector<std::string> const &values);
}		Config;

#endif
