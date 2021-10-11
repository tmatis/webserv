/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 22:36:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/12 00:25:54 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <string>

/*
** further comments define possible default values for each field
*/

typedef struct Route
{
	std::string					location;		// "/"
	std::vector<std::string>	methods;		// "GET" "POST" "DELETE"
	std::map<int, std::string>	redirections;	// empty
	std::string					root;			// "/var/www"
	bool						autoindex;		// false
	std::vector<std::string>	default_pages;	// "index.html"
	std::string					cgi_extension;	// ""
	std::string					upload_path;	// ""
}		Route;

typedef	struct Config
{
	std::string					address;		// mandatory
	int							port;			// mandatory
	std::string					name;			// ""
	size_t						body_limit;		// 0 for none
	std::map<int, std::string>	error_pages;	// empty
	std::vector<Route>			routes;			// if no route is specified with location="/", then create a default one
}		Config;

#endif
