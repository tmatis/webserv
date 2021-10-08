/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/08 22:36:30 by mamartin          #+#    #+#             */
/*   Updated: 2021/10/08 22:39:04 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

/*
** CONFIGURATION STRUCTURE FOR A SERVER
** TEMPORARILY DEFINED HERE
** MUST BE REPLACED AS SOON AS
** THE CONFIGURATION PARSER IS DONE
*/

typedef	struct Config
{
	char*								address;
	int									port;
	//const std::string					name;
	//const std::string					rootdir;
	//const int							bodysize_limit;
	//const std::map<int, std::string>	error_pages;
	// routes
		// accepted methods
		// redirections
		// rules for where to search files
		// on and off directory listing
		// default file for a directory
		// cgi path
		// uploaded files path
}		Config;

#endif
