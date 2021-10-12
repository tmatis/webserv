/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MasterConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nouchata <nouchata@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 11:16:04 by nouchata          #+#    #+#             */
/*   Updated: 2021/10/12 21:47:26 by nouchata         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MASTERCONFIG_HPP
# define MASTERCONFIG_HPP

# include <iostream>
# include <exception>
# include <string>
# include <set>
# include <map>
# include <vector>
# include <unistd.h>
# include <pwd.h>
# include <cctype>
# include <utility>
# include <stdexcept>
# include <fstream>
# include <sstream>

class MasterConfig
{
	private:
	// webserv proper config
	unsigned long							_flags;
	bool									_autoindex;
	int										_max_simultaneous_clients; // need to add a static var to server or client class
	std::string								_user; // setuid setgid
	std::ofstream							_error_log;

	// html proper config
	std::string								_default_mime;
	std::pair<std::string, int>				_mime_types;
	std::set<std::string>					_index_paths;
	std::map<int, std::string>				_error_pages;

	std::pair<std::string, std::string>		extract_key_value(std::string &line);
	static void								keep_only_printable_chars(std::string &edit);
	static bool								is_there_only_digits(std::string const &edit);

	public:
	MasterConfig();
	// get mime type
	MasterConfig(MasterConfig const &cp);
	~MasterConfig();

	MasterConfig			&operator=(MasterConfig const &rhs);
	void					construct(std::string const &config_path = "webserv.cnf");
	void					fill_var(std::pair<std::string, std::string> const &var_pair);
	void					server_construct(std::string &server_data);
	


	class ErrorAtLine : public std::exception
	{
		ErrorAtLine(unsigned int const &line, std::string const &instruction) throw();
		virtual ~ErrorAtLine() throw();

		virtual char const *what() const throw();
	};
};

#endif