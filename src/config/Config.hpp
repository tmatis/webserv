/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 22:36:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 13:08:16 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <map>
# include <vector>

/*
** further comments define possible default values for each field
*/

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

typedef	struct Config
{
	Config(void);

	void						add_default_route(void);

	std::string					address;		// mandatory
	int							port;			// mandatory
	std::string					name;			// ""
	size_t						body_limit;		// 0 for none
	std::map<int, std::string>	error_pages;	// empty
	std::vector<Route>			routes;			// if no route is specified with location="/", then create a default one
}		Config;

#endif
