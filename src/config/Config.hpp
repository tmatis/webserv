/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 22:36:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/13 15:49:32 by nouchata         ###   ########.fr       */
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

class MasterConfig;

typedef struct Route
{
	Route(void);

	bool						autoindex;		// false
	bool						upload_file;
	std::string					location;		// "/"
	std::vector<std::string>	methods;		// "GET" "POST" "DELETE"
	std::pair<int, std::string>	redirection;	// empty
	std::string					root;			// "/var/www"
	std::vector<std::string>	default_pages;	// "index.html"
	std::string					cgi_extension;	// ""
	std::string					upload_path;	// ""
}		Route;

typedef	struct Config : public MasterConfig
{
	Config(void);
	Config(MasterConfig const &master);
	Config(Config const &cp);
	~Config();

	void				add_default_route(void);
	void				construct(std::string &config_str);
	int					fill_var(std::pair<std::string, std::string> const &var_pair);

	unsigned long				address;		// mandatory
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
}		Config;

#endif
